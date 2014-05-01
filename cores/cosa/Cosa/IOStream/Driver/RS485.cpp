/**
 * @file Cosa/IOStream/Driver/RS485.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
#if !defined(BOARD_ATTINY)
#include "Cosa/IOStream/Driver/RS485.hh"
#include "Cosa/RTC.hh"
#include <util/crc16.h>

static uint8_t
crc7(const void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  uint8_t crc = 0;
  while (size--) {
    uint8_t data = *bp++;
    data ^= crc << 1;
    if (data & 0x80) data ^= 9;
    crc = data ^ (crc & 0x78) ^ (crc << 4) ^ ((crc >> 3) & 0x0f);
  }
  crc = (crc << 1) ^ (crc << 4) ^ (crc & 0x70) ^ ((crc >> 3) & 0x0f);
  return (crc | 1);
}

static uint16_t
crc_xmodem(const void* buf, size_t len)
{
  if (len == 0) return (0);
  uint8_t* bp = (uint8_t*) buf;
  uint16_t crc = 0;
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

  // Build message header and calculate payload check-sum
  header_t header;
  header.length = len;
  header.dest = dest;
  header.src = m_addr;
  header.crc = crc7(&header, sizeof(header) - 1);
  uint16_t crc = crc_xmodem(buf, len);

  // Write message; SOT, header, payload and crc
  m_de.set();
  DELAY(100);
  if (putchar(SOT) < 0) return (-5);
  if (write(&header, sizeof(header)) != (int) sizeof(header)) return (-5);
  if (write(buf, len) != (int) len) return (-5);
  if (write(&crc, sizeof(crc)) != sizeof(crc)) return (-5);
  return (len);
}

int 
RS485::recv(void* buf, size_t len, uint32_t ms)
{
  uint32_t start = RTC::millis();
  uint16_t crc;

  // Receive state-machine; start symbol, header, payload and check-sum
  switch (m_state) {
  case 0: // Wait for transmission to complete and start symbol
    while (m_de.is_set()) Power::sleep(SLEEP_MODE_IDLE);
    while (getchar() != SOT) {
      if ((ms != 0) && (RTC::millis() - start > ms)) return (-2);
      Power::sleep(m_mode);
    }
    m_state = 1;
    
  case 1: // Read message header and verify header check-sum
    while (available() != sizeof(header_t)) {
      if ((ms != 0) && (RTC::millis() - start > ms)) return (-2);
      Power::sleep(m_mode);
    }
    m_state = 2;
    if (m_ibuf.read(&m_header, sizeof(header_t)) != (int) sizeof(header_t)) 
      goto error;
    if (m_header.crc != crc7(&m_header, sizeof(header_t) - 1)) 
      goto error;

  case 2: // Read message payload and verify payload check-sum
    while (available() != (int) (m_header.length + sizeof(crc))) {
      if ((ms != 0) && (RTC::millis() - start > ms)) return (-2);
      Power::sleep(SLEEP_MODE_IDLE);
    }
    m_state = 3;
  }

  // Check that the given buffer can hold incoming message
  if (m_header.length > len) goto error;
  if (m_ibuf.read(buf, m_header.length) != (int) m_header.length) goto error;
  if (m_ibuf.read(&crc, sizeof(crc)) != sizeof(crc)) goto error;
  if (crc_xmodem(buf, m_header.length) != crc) return (0);
  m_state = 0;

  // Check that the message was addressed to this device
  if ((m_header.dest == m_addr) || (m_header.dest == BROADCAST)) 
    return (m_header.length);
  return (0);
  
 error:
  // Something went wrong; flush buffer and signal data error
  m_ibuf.empty();
  m_state = 0;
  return (-1);
}
#endif
