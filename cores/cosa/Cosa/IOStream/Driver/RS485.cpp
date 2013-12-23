/**
 * @file Cosa/IOStream/Driver/RS485.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#if !defined(__ARDUINO_TINY__)
#include "Cosa/IOStream/Driver/RS485.hh"
#include "Cosa/RTC.hh"
#include <util/crc16.h>

static uint16_t
crc_xmodem(const void* buf, size_t len, uint16_t crc)
{
  if (len == 0) return (0);
  uint8_t* bp = (uint8_t*) buf;
  do crc = _crc_xmodem_update(crc, *bp++); while (--len);
  return (crc);
}

int 
RS485::send(const void* buf, size_t len, uint8_t dest)
{
  // Check illegal message size, address and state
  if (len == 0 || len > PAYLOAD_MAX) return (-1);
  if (dest == m_addr) return (-2);
  if (m_addr != MASTER && dest != MASTER) return (-3);
  if (m_de.is_set()) return (-4);

  // Write message to output buffer (count, dest, payload, crc)
  uint8_t count = len + FRAME_MAX;
  uint16_t crc = crc_xmodem(buf, len, _crc_xmodem_update(0, dest));
  m_de.set();
  DELAY(10);
  if (putchar(SOT) < 0) return (-5);
  if (putchar(count) < 0) return (-5);
  if (putchar(dest) < 0) return (-5);
  if (write(buf, len) != (int) len) return (-5);
  if (write(&crc, sizeof(crc)) != sizeof(crc)) return (-5);
  return (len);
}

int 
RS485::recv(void* buf, size_t len, uint32_t ms)
{
  // Wait for transmission to complete
  while (m_de.is_set()) Power::sleep(SLEEP_MODE_IDLE);

  // Wait for a message
  uint32_t start = RTC::millis();
  while (getchar() != SOT) {
    if ((ms != 0) && (RTC::since(start) > ms)) return (-2);
    Power::sleep(m_mode);
  }
  // Read message from the input buffer (count, dest, payload, crc)
  while (available() != 2) {
    if ((ms != 0) && (RTC::since(start) > ms)) return (-2);
    Power::sleep(m_mode);
  }
  uint8_t count = getchar() - FRAME_MAX;
  uint8_t dest = getchar();
  uint16_t crc = 0;
  if (count > len) goto error;
  while (available() != count + 2) {
    if ((ms != 0) && (RTC::since(start) > ms)) return (-2);
    Power::sleep(SLEEP_MODE_IDLE);
  }
  if (m_ibuf.read(buf, count) != count) goto error;
  if (m_ibuf.read(&crc, sizeof(crc)) != sizeof(crc)) goto error;
  if (crc_xmodem(buf, count, _crc_xmodem_update(0, dest)) != crc) return (0);

  // Check that the message was addressed to this device
  if ((dest == m_addr) || (dest == BROADCAST)) return (count);
  return (0);
  
 error:
  // Something went wrong; flush buffer and signal data error
  m_ibuf.empty();
  return (-1);
}
#endif
