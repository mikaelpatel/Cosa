/**
 * @file CosaDS18B20.ino
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
 * Cosa demonstration of the DS18B20 device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/OneWire.h"
#include "Cosa/DS18B20.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"
#include "Cosa/FixedPoint.h"

// One-wire pin and DS18B20 device
OneWire oneWire(7);
DS18B20 outdoors(&oneWire);
DS18B20 indoors(&oneWire);

void setup()
{
  // Start trace output stream
  trace.begin(9600);

  // Info message using the trace log
  INFO("Initiated trace log", 0);

  // Check amount of free memory
  TRACE(free_memory());

  // Print debug information about the sketch onewire pins
  oneWire.println();

  // Read and print the device rom and scratchpad
  TRACE(indoors.connect(0));
  indoors.print_rom();
  TRACE(indoors.read_scratchpad());
  indoors.print_scratchpad();

  TRACE(outdoors.connect(1));
  outdoors.print_rom();
  TRACE(outdoors.read_scratchpad());
  outdoors.print_scratchpad();

  // Start the watchdog ticks counter
  Watchdog::begin(16);
}

void loop()
{
  // Request a single temperature conversion
  TRACE(indoors.convert_request());
  TRACE(outdoors.convert_request());
  Watchdog::delay(1024);

  // Read the scatchpad to get the latest value and print
  int16_t integer;
  int16_t fraction;
  int16_t fraction1000;

  TRACE(indoors.read_scratchpad());
  FixedPoint intemp(indoors.get_temperature(), 4);
  integer = intemp.get_integer();
  fraction = intemp.get_fraction();
  fraction1000 = intemp.get_fraction(4);
  INFO("(indoors) raw = %hd", indoors.get_temperature());
  INFO("fixed<12:4> = %d.%d", integer, fraction);
  INFO("temperature = %d.%s%d C", integer,
       (fraction1000 != 0 & fraction1000 < 1000 ? "0" : ""),
       fraction1000);

  TRACE(outdoors.read_scratchpad());
  FixedPoint outtemp(outdoors.get_temperature(), 4);
  integer = outtemp.get_integer();
  fraction = outtemp.get_fraction();
  fraction1000 = outtemp.get_fraction(4);
  INFO("(outdoors) raw = %hd", outdoors.get_temperature());
  INFO("fixed<12:4> = %d.%d", integer, fraction);
  INFO("temperature = %d.%s%d C", integer,
       (fraction1000 != 0 & fraction1000 < 1000 ? "0" : ""),
       fraction1000);
}
