/**
 * @file Cosa/MPE/MPE_Receiver.cpp
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

#include "Cosa/MPE.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"
#include <util/crc16.h>

void 
MPE::Receiver::on_interrupt(uint16_t arg) 
{ 
  // First sample; get start time and clear state
  if (m_start == 0) {
    m_start = RTC::micros();
    m_samples = 0;
    m_ix = 0;
    return;
  }

  // Next sample; check thresholds, calculate pulse width
  uint16_t stop = RTC::micros();
  uint16_t us = (stop - m_start);
  uint8_t n = ((us - LOW_THRESHOLD) >> 8) + 1;
  if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) goto exception;
  m_start = stop;

  // Look for the start symbol
  if (m_samples == 0 && n == 3) {
    m_samples = 1;
    m_count = 0;
    m_state = 2;
    m_bits = 1;
    return;
  }

  // Skip the preamble (four pulses)
  if (!m_samples) return;
  m_samples += 1;
  if (m_samples < 3) return;

  // Manchester phase encode state machine
  uint8_t bit;
  switch (m_state) {
  case 0: 
    m_state = 1;
    bit = 1;
    break;
  case 1: 
    if (n == 1) {
      m_state = 0;
      return;
    } 
    else {
      m_state = 2;
      bit = 0;
    }
    break;
  case 2:
    if (n == 1) {
      m_state = 3;
      return;
    }
    else {
      m_state = 1;
      bit = 1;
    }
    break;
  case 3:
    m_state = 2;
    bit = 0;
    break;
  default:
    return;
  }

  // Add the received bit to the data stream
  m_value = (m_value << 1) | bit;
  m_bits += 1;
  if (m_bits != CHARBITS) return;
  if (m_ix == 0) {
    m_available = false;
    m_count = m_value;
    if (m_count >= FRAME_MAX) goto exception;
  }
  m_buffer[m_ix++] = m_value;
  m_value = 0;
  m_bits = 0;

  // Check if this is last data 
  if (m_ix != m_count) return;
  m_available = true;

 exception:
  m_start = 0;
}

int
MPE::Receiver::recv(void* buf, size_t size, uint32_t ms)
{
  uint32_t start = RTC::millis();
  uint16_t crc;

  // Wait for a message and check that it is valid. Wait max given ms
  do {
    while (!m_available && ((ms == 0L) || (RTC::since(start) < ms)))
      Power::sleep(SLEEP_MODE_IDLE);
    if (!m_available) return (0);
    uint8_t* bp = (uint8_t*) m_buffer;
    uint8_t count = m_count;
    crc = 0xffff;
    while (count-- > 0) crc = _crc_ccitt_update(crc, *bp++);
  } while (crc != CRC_CHECK_SUM);

  // Copy message to given buffer and return
  uint8_t len = m_count - 3;
  if (len < size) size = len;
  memcpy(buf, (void*) (m_buffer + 1), size);
  m_available = false;
  return (size);
}
