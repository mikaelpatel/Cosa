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
DS18B20 ds18b20(&oneWire);

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
  TRACE(ds18b20.read_rom());
  ds18b20.print_rom();
  TRACE(ds18b20.read_scratchpad());
  ds18b20.print_scratchpad();

  // Start the watchdog ticks counter
  Watchdog::begin(16);
}

void loop()
{
  // Request a single temperature conversion
  TRACE(ds18b20.convert_request());
  Watchdog::delay(1024);

  // Read the scatchpad to get the latest value and print
  TRACE(ds18b20.read_scratchpad());
  FixedPoint temp(ds18b20.get_temperature(), 4);
  INFO("temperature = %d.%d", 
       temp.get_integer(), 
       temp.get_fraction(4));
}
