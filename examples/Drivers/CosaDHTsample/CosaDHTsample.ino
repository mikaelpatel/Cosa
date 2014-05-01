/**
 * @file CosaDHTsample.ino
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
 * @section Description
 * Cosa demonstration of the DHT11/DHT22 device driver.
 *
 * @section Circuit
 *                       DHT11/outdoors
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 *
 *                       DHT22/indoors
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D3/EXT1)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/DHT.hh"

// Use DHT11 for outdoors measurement and DHT22 for indoors
DHT11 outdoors(Board::EXT0);
// ATtiny has only one external interrupt pin
#if !defined(BOARD_ATTINY)
DHT22 indoors(Board::EXT1);
#endif

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHTsample: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(DHT));
  TRACE(sizeof(DHT11));
  TRACE(sizeof(DHT22));

  // Start the watchdog for low power sleep
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  // Sample every 2 seconds
  SLEEP(2);

  // Read and print humidity and temperature
#if !defined(BOARD_ATTINY)
  indoors.sample();
  trace << PSTR("indoors: ") << indoors << endl;
#endif
  outdoors.sample();
  trace << PSTR("outdoors: ") << outdoors << endl;
}
