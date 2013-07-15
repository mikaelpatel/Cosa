/**
 * @file CosaAT24CXX.ino
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

// Use the builtin led as a heartbeat
OutputPin ledPin(Board::LED);

// The serial eeprom (sub-address 0b000) with binding to eeprom 
AT24C32 at24c32(0);
EEPROM eeprom(&at24c32);

// Symbols for data stored in AT24CXX EEPROM memory address space
int x[6] EEMEM;
uint8_t y[300] EEMEM;
float z EEMEM;

void init_eeprom()
{
  int x0[membersof(x)];
  for (uint8_t i = 0; i < membersof(x0); i++) x0[i] = i;
  eeprom.write(x, x0, sizeof(x));
  
  uint8_t y0[sizeof(y)];
  memset(y0, 0, sizeof(y));
  eeprom.write(y, y0, sizeof(y));
  
  float z0 = 1.0;
  eeprom.write(&z, z0);
}

void setup()
{
  // Start trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAT24CXX: started"));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Initiate EEPROM variables
  init_eeprom();
}

void loop()
{
  SLEEP(2);
  ledPin.toggle();

  // Buffer for update loop
  uint8_t buffer[sizeof(y)];
  memset(buffer, 0, sizeof(buffer));
  
  // Read x and print contents
  eeprom.read(buffer, &x, sizeof(x));
  trace.print(buffer, sizeof(x), IOStream::hex);

  // Read y, print contents and update 
  eeprom.read(buffer, &y, sizeof(y));
  trace.print(buffer, sizeof(y), IOStream::hex);
  for (size_t i = 0; i < sizeof(buffer); i++)
    buffer[i]++;
  eeprom.write(&y, buffer, sizeof(buffer));
  TRACE(eeprom.is_ready());
  eeprom.write_await();
  TRACE(eeprom.is_ready());
  
  // Read z and update
  float z1;
  eeprom.read(&z1, &z);
  trace.print(&z1, sizeof(z1), IOStream::hex);
  z1 += 0.5;
  TRACE(eeprom.write(&z, z1));

  ledPin.toggle();
}
