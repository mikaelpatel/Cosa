/**
 * @file Cosa/Driver/DHT11.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * DHT11 Humidity & Temperature Sensor device driver. Subclass
 * and implement the event handler, on_event(), to allow periodic
 * read of device (attach to watchdog timeout queue).
 *
 * @section Circuit
 * Connect DHT11 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT11 modules have a built-in 
 * pullup resistor.
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read from
 * the device. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/DHT11.hh"
#include "Cosa/Watchdog.hh"

// Thresholds for wire sampling
static const uint8_t COUNT_MIN = 40;
static const uint8_t COUNT_MAX = 255;

// Index of last member in data buffer
static const uint8_t DATA_LAST = DHT11::DATA_MAX - 1;

int8_t
DHT11::read_bit(uint8_t changes)
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
DHT11::read(uint8_t& temperature, uint8_t& humidity)
{
  // Default out of range values
  temperature = 100;
  humidity = 100;

  // Send start signal to the device
  m_pin.set_mode(IOPin::OUTPUT_MODE);
  m_pin.clear();
  Watchdog::delay(256);
  m_pin.set();
  DELAY(40);
  m_pin.set_mode(IOPin::INPUT_MODE);
  
  // Receive bits from the device and calculate check sum
  uint8_t chksum = 0;
  m_latest = 1;
  synchronized {
    if (read_bit(3) < 0) synchronized_return (0);
    for (uint8_t i = 0; i < DATA_MAX; i++) {
      for (uint8_t j = 0; j < CHARBITS; j++) {
	int8_t bit = read_bit(2);
	if (bit < 0) synchronized_return (0);
	m_data[i] = (m_data[i] << 1) | bit;
      }
      if (i < DATA_LAST) chksum += m_data[i];
    }
  }

  // Return values and validation of the check sum
  temperature = m_data[2];
  humidity = m_data[0];
  return (chksum == m_data[DATA_LAST]);
}

