/**
 * @file CosaDHT11.ino
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
 * Cosa demonstration of the DHT11 device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Driver/DHT11.hh"
#include "Cosa/Memory.h"

// Connect devices to pins
DHT11 indoors(Board::D7);
DHT11 outdoors(Board::D8);
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHT11: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(DHT11));

  // Start the watchdog ticks and push time events
  Watchdog::begin(16);
}

void loop()
{
  // Sample every 2 seconds
  SLEEP(2);

  // Read in- and outdoors temperature and humidity
  ledPin.toggle();
  uint8_t temperature;
  uint8_t humidity;

  indoors.read(temperature, humidity);
  trace.print_P(PSTR("indoors:  "));
  trace.printf_P(PSTR("RH = %d%%, T = %d C\n"), humidity, temperature);

  outdoors.read(temperature, humidity);
  trace.print_P(PSTR("outdoors: "));
  trace.printf_P(PSTR("RH = %d%%, T = %d C\n"), humidity, temperature);

  // Blink built-in led during active period
  ledPin.toggle();
}
