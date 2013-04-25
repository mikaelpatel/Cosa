/**
 * @file Cosa/Driver/NEXA.cpp
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

#include "Cosa/Driver/NEXA.hh"
#include "Cosa/RTC.hh"

IOStream& operator<<(IOStream& outs, NEXA::code_t code)
{
  outs << PSTR("house = ") << code.house 
       << PSTR(", group = ") << code.group
       << PSTR(", device = ") << code.device
       << PSTR(", on/off = ") << code.onoff;
  return (outs);
}

void 
NEXA::Receiver::on_interrupt(uint16_t arg) 
{ 
  // Check start condition
  if (m_start == 0L) {
    if (is_clear()) return;
    m_start = RTC::micros();
    m_ix = 0;
    return;
  }

  // Calculate the pulse width (both low and high) and check against threshold
  uint32_t stop = RTC::micros();
  uint32_t us = (stop - m_start);
  m_start = stop;
  if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) goto exception;
  m_sample[m_ix & 0x3] = us;
  m_ix += 1;

  // Decode every four pulses to a bit
  if ((m_ix & 0x3) == 0) {
    int8_t bit = decode_bit();
    if (bit < 0) goto exception;
    m_code = (m_code << 1) | bit;
  }
  if (m_ix != SAMPLE_MAX) return;

  // And when all samples have been read push an event
  Event::push(Event::READ_COMPLETED_TYPE, this);
  
 exception:
  m_start = 0L;
}

int8_t
NEXA::Receiver::decode_bit()
{
  uint8_t bit;
  bit = ((m_sample[0] < BIT_THRESHOLD) << 1) | (m_sample[1] < BIT_THRESHOLD);
  if (bit < 2) return (-1);
  bit = ((m_sample[2] < BIT_THRESHOLD) << 1) | (m_sample[3] < BIT_THRESHOLD);
  if (bit < 2) return (-1);
  return (bit > 2);  
}

NEXA::code_t
NEXA::Receiver::read_code()
{
  uint32_t start, stop;
  uint32_t bits = 0L;
  uint16_t us;
  uint16_t ix;
  do {
    // Wait for the start condition
    while (is_low());
    stop = RTC::micros();

    // Collect the samples; high followed by low pulse
    ix = 0;
    while (ix < SAMPLE_MAX) {
      start = stop;
      while (is_high());
      stop = RTC::micros();
      us = stop - start;
      if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) break;
      m_sample[ix & 0x3] = us;
      ix += 1;
      start = stop;
      while (is_low());
      stop = RTC::micros();
      us = stop - start;
      if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) break;
      m_sample[ix & 0x3] = us;
      ix += 1;
      // Decode every four samples to a code bit
      if ((ix & 0x3) == 0) {
	int8_t bit = decode_bit();
	if (bit < 0) break;
	bits = (bits << 1) | bit;
      }
    }
  } while (ix != SAMPLE_MAX);
  return (m_code = bits);
}
