/**
 * @file CosaBMP085.ino
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
 * @section Description
 * Cosa demonstration of BMP085 driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/BMP085.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Digital pressure sensor
BMP085 bmp;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBMP085: started"));
#if !defined(__ARDUINO_TINY__)
  TRACE(free_memory());
  TRACE(sizeof(TWI::Driver));
  TRACE(sizeof(BMP085));
#endif
  Watchdog::begin();
  bmp.begin(BMP085::ULTRA_LOW_POWER);
}

void loop()
{
  static uint8_t mode = BMP085::ULTRA_LOW_POWER;
  static uint8_t nr = 0;

  // Read raw temperature and pressure sensors and calculate
  uint32_t start = Watchdog::millis();
  bmp.sample_temperature();
  bmp.sample_pressure();
  uint32_t stop = Watchdog::millis();

  // Calculate and print values
  trace << PSTR("BMP085(mode = ") << mode
	<< PSTR(", temperature = ") << bmp.get_temperature()
	<< PSTR(", pressure = ") << bmp.get_pressure()
	<< PSTR(", delay = ") << (stop - start)
	<< PSTR(")") << endl;

  SLEEP(2);

  // Check if it is time to increment the operation mode
  nr += 1;
  if (nr & 0x03) return;
  mode = (mode + 1) & 0x03;
  bmp.begin((BMP085::Mode) mode);
}
