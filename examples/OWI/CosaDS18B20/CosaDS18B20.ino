/**
 * @file CosaDS18B20.ino
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
 * Cosa demonstration of the DS18B20 1-Wire device driver.
 * Assumes three thermometers are connected to 1-Wire bus on
 * pin(7) in search order. Will not work in parasite power mode.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Board.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// One-wire pin and connected DS18B20 devices
#if defined(__ARDUINO_TINY__)
OWI owi(Board::D1);
#else
OWI owi(Board::D7);
#endif
DS18B20 outdoors(&owi);
DS18B20 indoors(&owi);
DS18B20 basement(&owi);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS18B20: started"));

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(OWI));
  TRACE(sizeof(DS18B20));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // List connected devices
  trace << owi << endl;

  // Connect to the devices and print rom contents
  ledPin.toggle();
  TRACE(indoors.connect(0));
  trace << (OWI::Driver&) indoors << endl;
  TRACE(outdoors.connect(1));
  trace << (OWI::Driver&) outdoors << endl;
  TRACE(basement.connect(2));
  trace << (OWI::Driver&) basement << endl;
  ledPin.toggle();
}

void loop()
{
  // Boardcast convert request to all devices, read and print results
  ledPin.toggle();
  DS18B20::convert_request(&owi);
  indoors.read_scratchpad();
  outdoors.read_scratchpad();
  basement.read_scratchpad();
  trace << PSTR("indoors = ") << indoors
	<< PSTR(", outdoors = ") << outdoors
	<< PSTR(", basement = ") << basement 
	<< endl;
  ledPin.toggle();

  // Sleep before requesting a new sample
  SLEEP(1);
}
