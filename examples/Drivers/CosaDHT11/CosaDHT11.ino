/**
 * @file CosaDHT11.ino
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
 * @section Description
 * Cosa demonstration of the DHT11/DHT22 device driver.
 *
 * @section Circuit
 * Connect Arduino:D2 to DHT11 data pin. Connect D3 to DHT22 data 
 * pin. Pullup resistors (4K7) necessary if long wires. Internal 
 * pullup active. Connect power (VCC) and ground (GND).   
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/DHT.hh"

DHT11 outdoors(Board::EXT0);
#if !defined(__ARDUINO_TINY__)
DHT22 indoors(Board::EXT1);
#endif

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHT11: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(DHT));

  // Start the watchdog for low power sleep
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  // Sample every 2 seconds
  SLEEP(2);

  // Read and print humidity and temperature
#if !defined(__ARDUINO_TINY__)
  indoors.sample();
  trace << PSTR("indoors: ") << indoors << endl;
#endif
  outdoors.sample();
  trace << PSTR("outdoors: ") << outdoors << endl;
}
