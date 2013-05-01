/**
 * @file Cosa/Driver/DHT.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/Driver/DHT.hh"
#include "Cosa/Watchdog.hh"

static const uint16_t START_REQUEST = 16;
static const uint8_t START_RESPONSE = 40;
#if defined(__ARDUINO_TINYX5__)
static const uint8_t COUNT_MIN = 30;
static const uint8_t COUNT_MAX = 255;
#else
static const uint8_t COUNT_MIN = 40;
static const uint8_t COUNT_MAX = 255;
#endif

int8_t
DHT::read_bit(uint8_t changes)
{
  uint8_t counter = 0;
  while (changes--) {
    while (m_pin.is_set() == m_latest) {
      counter++;
      DELAY(1);
      if (counter == COUNT_MAX) return (-1);
    }
    m_latest = !m_latest;
  }
  return (counter > COUNT_MIN);
}

bool 
DHT::read_data()
{
  // Send start signal to the device
  m_pin.set_mode(IOPin::OUTPUT_MODE);
  m_pin.clear();
  Watchdog::delay(START_REQUEST);
  m_pin.set();
  DELAY(START_RESPONSE);
  m_pin.set_mode(IOPin::INPUT_MODE);

  // Receive bits from the device and calculate check sum
  uint8_t chksum = 0;
  m_latest = 1;
  synchronized {
    if (read_bit(3) < 0) synchronized_return (false);
    for (uint8_t i = 0; i < DATA_MAX; i++) {
      for (uint8_t j = 0; j < CHARBITS; j++) {
	int8_t bit = read_bit(2);
	if (bit < 0) synchronized_return (false);
	m_data.as_byte[i] = (m_data.as_byte[i] << 1) | bit;
      }
      if (i < DATA_LAST) chksum += m_data.as_byte[i];
    }
  }

  // Adjust data depending on version of device
  adjust_data();
  return (chksum == m_data.chksum);
}
