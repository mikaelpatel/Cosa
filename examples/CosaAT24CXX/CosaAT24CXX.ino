/**
 * @file CosaAT24CXX.ino
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
 * Cosa demonstration of the AT24CXX 2-Wire (TWI) Serial EEPROM 
 * driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.h"
#include "Cosa/Trace.h"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.h"
#include "Cosa/TWI/AT24CXX.h"

// Use the buildin led as a heartbeat
OutputPin ledPin(13, 0);

// The serial eeprom
AT24CXX rom(0);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaAT24CXX: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter (2 second ticks)
  Watchdog::begin(2048);
}

void loop()
{
  Watchdog::await();
  ledPin.toggle();

  // Read the eeprom into memory
  uint8_t buffer[32];
  TRACE(rom.read(buffer, sizeof(buffer), 0));
  trace.print(buffer, sizeof(buffer), 16);

  // Update the eeprom (d => d+1)
  for (uint8_t i = 0; i < sizeof(buffer); i++)
    buffer[i]++;
  TRACE(rom.write(buffer, sizeof(buffer), 0));

  // Read back and check
  TRACE(rom.read(buffer, sizeof(buffer), 0));
  trace.print(buffer, sizeof(buffer), 16);
  ledPin.toggle();
}
