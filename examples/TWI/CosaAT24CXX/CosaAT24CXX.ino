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

#include "Cosa/TWI/Driver/AT24CXX.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"

// Use the builtin led as a heartbeat
OutputPin ledPin(Board::LED);

// The serial eeprom (sub-address 0b000)
AT24CXX eeprom(0b000);

// Symbols for data stored in EEPROM
int x[8] EEPROM(0);
uint8_t y[300] EEPROM(0);
int z EEPROM(0);

// A final marker
int last;

void setup()
{
  // Start trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAT24CXX: started"));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Check memory map
  TRACE(&ledPin);
  TRACE(sizeof(ledPin));
  TRACE(&eeprom);
  TRACE(sizeof(eeprom));
  TRACE(&uart);
  TRACE(sizeof(uart));
  TRACE(&twi);
  TRACE(sizeof(twi));
  TRACE(&trace);
  TRACE(sizeof(trace));
  TRACE(&x);
  TRACE(&y);
  TRACE(&z);
  TRACE(&last);
  TRACE(free_memory());
}

void loop()
{
  // Wait for 2 seconds; we don't want to burn too many write cycles
  SLEEP(2);
  ledPin.toggle();

  // Read the eepeeprom into memory
  uint8_t buffer[sizeof(y)];
  uint16_t addr = (uint16_t) &y;
  TRACE(eeprom.read(buffer, addr, sizeof(buffer)));
  trace.print(buffer, sizeof(buffer), 16);

  // Update the eepeeprom (d => d+1)
  for (size_t i = 0; i < sizeof(buffer); i++)
    buffer[i]++;
  TRACE(eeprom.write(addr, buffer, sizeof(buffer)));

  // Is the write completed? 
  TRACE(eeprom.is_ready());
  eeprom.write_await();
  TRACE(eeprom.is_ready());
  
  // Read back and check
  TRACE(eeprom.read(buffer, addr, sizeof(buffer)));
  trace.print(buffer, sizeof(buffer), 16);
  ledPin.toggle();
}
