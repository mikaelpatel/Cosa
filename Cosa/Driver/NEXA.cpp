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
#include "Cosa/Watchdog.hh"

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
  m_sample[m_ix & IX_MASK] = us;
  m_ix += 1;

  // Decode every four pulses to a bit
  if ((m_ix & IX_MASK) == 0) {
    int8_t bit = decode_bit();
    if (bit < 0) goto exception;
    m_code = (m_code << 1) | bit;
  }
  if (m_ix != IX_MAX) return;

  // And when all samples have been read push an event
  Event::push(Event::READ_COMPLETED_TYPE, this);
  
 exception:
  m_start = 0L;
}

int8_t
NEXA::Receiver::decode_bit()
{
  uint8_t bit;
  // The pedantic version checks even the first pulse. This could be removed
  bit = ((m_sample[0] < BIT_THRESHOLD) << 1) | (m_sample[1] < BIT_THRESHOLD);
  if (bit < 2) return (-1);
  // The second pulse has the actual transmitted bit
  bit = ((m_sample[2] < BIT_THRESHOLD) << 1) | (m_sample[3] < BIT_THRESHOLD);
  if (bit < 2) return (-1);
  // And map back to a bit (2 => 0, 3 => 1)
  return (bit > 2);  
}

void
NEXA::Receiver::recv(code_t& cmd)
{
  uint32_t start, stop;
  int32_t bits = 0L;
  uint16_t us;
  uint16_t ix;
  do {
    // Wait for the start condition
    while (is_low());
    stop = RTC::micros();

    // Collect the samples; high followed by low pulse
    ix = 0;
    while (ix < IX_MAX) {
      // Capture length of high period
      start = stop;
      while (is_high());
      stop = RTC::micros();
      us = stop - start;
      if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) break;
      m_sample[ix & IX_MASK] = us;
      ix += 1;
      // Capture length of low period
      start = stop;
      while (is_low());
      stop = RTC::micros();
      us = stop - start;
      if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) break;
      m_sample[ix & IX_MASK] = us;
      ix += 1;
      // Decode every four samples to a code bit
      if ((ix & IX_MASK) == 0) {
	int8_t bit = decode_bit();
	if (bit < 0) break;
	bits = (bits << 1) | bit;
      }
    }
  } while (ix != IX_MAX);
  m_code = bits;
  cmd = bits;
}

void
NEXA::Transmitter::send_code(code_t cmd, int8_t onoff, uint8_t mode)
{
  // Send the code four times with a pause between each
  for (uint8_t i = 0; i < SEND_CODE_MAX; i++) {
    int32_t bits = cmd.as_long;
    // Send start pulse with extended delay, code bits and stop pulse
    send_pulse(0);
    DELAY(START);
    for (uint8_t j = 0; j < 32; j++) {
      // Check for dim level (-1..-15)
      if ((j == 27) && (onoff < 0)) {
	send_pulse(0);
	send_pulse(0);
      }
      else send_bit(bits < 0);
      bits <<= 1;
    }
    // Check for dim level transmission
    if (onoff < 0) {
      int8_t level = (-onoff) << 4;
      for (uint8_t j = 0; j < 4; j++) {
	send_bit(level < 0);
	level <<= 1;
      }
    }
    send_pulse(0);
    // Wait for the transmission of the code
    uint32_t start = RTC::millis();
    while ((RTC::millis() - start) < PAUSE) Power::sleep(mode);
  }
}
