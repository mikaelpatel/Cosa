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
 * Cosa demonstration of the DS18B20 1-Wire device driver.
 * Assumes three thermometers are connected to 1-Wire bus on
 * pin(7) in search order.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OneWire/DS18B20.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Memory.h"
#include "Cosa/Trace.h"

// One-wire pin and connected DS18B20 devices
OneWire oneWire(7);
DS18B20 outdoors(&oneWire);
DS18B20 indoors(&oneWire);
DS18B20 basement(&oneWire);

// Use the builtin led for a heartbeat
OutputPin ledPin(13);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaDS18B20: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // List connected devices
  oneWire.print_devices();

  // Connect to the devices and print rom contents
  ledPin.toggle();
  TRACE(indoors.connect(0));
  indoors.print_rom();
  TRACE(outdoors.connect(1));
  outdoors.print_rom();
  TRACE(basement.connect(2));
  basement.print_rom();
  ledPin.toggle();

  // Start the watchdog ticks counter with 16 ms period
  Watchdog::begin(16);

  // Start the conversion pipeline; indoors->outdoors->basement sampling
  indoors.convert_request();
  Watchdog::delay(1024);
}

void loop()
{
  // Start outdoors temperature conversion and read the indoors temperature
  ledPin.toggle();
  outdoors.convert_request();
  indoors.read_temperature();
  indoors.print_temperature_P(PSTR("indoors = "));
  ledPin.toggle();
  Watchdog::delay(1024);

  // Start basement temperature conversion and read the outdoors temperature
  ledPin.toggle();
  basement.convert_request();
  outdoors.read_temperature();
  outdoors.print_temperature_P(PSTR(", outdoors = "));
  ledPin.toggle();
  Watchdog::delay(1024);

  // Start indoors temperature conversion and read the basement temperature
  ledPin.toggle();
  indoors.convert_request();
  basement.read_temperature();
  basement.print_temperature_P(PSTR(", basement = "));
  trace.println();
  ledPin.toggle();
  Watchdog::delay(1024);
}
