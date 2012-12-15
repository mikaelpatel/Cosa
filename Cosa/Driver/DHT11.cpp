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
 * DHT11 Humidity & Temperature Sensor device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/DHT11.h"
#include "Cosa/Watchdog.h"
#include <util/delay_basic.h>

#define DELAY(us) _delay_loop_2((us) << 2)

// Thresholds for wire sampling
static const uint8_t COUNT_MIN = 20;
static const uint8_t COUNT_MAX = 255;

int8_t
DHT11::read_bit(uint8_t changes)
{
  uint8_t counter = 0;
  while (changes--) {
    while (is_set() == _latest) {
      counter++;
      DELAY(1);
      if (counter == COUNT_MAX) return (-1);
    }
    _latest = !_latest;
  }
  return (counter > COUNT_MIN);
}

bool 
DHT11::read(uint8_t& temperature, uint8_t& humidity)
{
  // Default out of range values
  temperature = 100;
  humidity = 100;

  // Send start signal to DHT
  set_mode(OUTPUT_MODE);
  clear();
  Watchdog::delay(256);
  set();
  DELAY(40);
  set_mode(INPUT_MODE);
  
  // Receive bits from the DHT and calculate check sum
  uint8_t chksum = 0;
  _latest = 1;
  synchronized {
    if (read_bit(3) < 0) synchronized_return (0);
    for (uint8_t i = 0; i < DATA_MAX; i++) {
      for (uint8_t j = 0; j < CHARBITS; j++) {
	int8_t bit = read_bit(2);
	if (bit < 0) synchronized_return (0);
	_data[i] = (_data[i] << 1) | bit;
      }
      if (i < DATA_MAX - 1) chksum += _data[i];
    }
  }

  // Return values, and validation of the check sum
  temperature = _data[2];
  humidity = _data[0];
  return (chksum == _data[DATA_MAX - 1]);
}

