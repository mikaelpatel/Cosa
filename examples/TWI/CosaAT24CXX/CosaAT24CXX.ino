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

#include "Cosa/TWI/AT24CXX.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Use the builtin led as a heartbeat
OutputPin ledPin(Board::LED);

// The serial eeprom (sub-address 0b000)
AT24CXX rom(0b000);

void setup()
{
  // Start trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAT24CXX: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(rom));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();
}

void loop()
{
  // Wait for 2 seconds; we don't want to burn too many write cycles
  SLEEP(2);
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
