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

// Digital pressure sensor
BMP085 bmp;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBMP085: started"));
  Watchdog::begin();
  bmp.begin();
}

void loop()
{
  int32_t temperature;
  bmp.sample(temperature);
  trace << bmp.calculate(temperature) << PSTR(" 10xC, ");
  uint32_t pressure;
  bmp.sample(pressure);
  trace << bmp.calculate(pressure, temperature) << PSTR(" Pa") << endl;
  SLEEP(2);
}
