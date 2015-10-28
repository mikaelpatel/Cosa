/**
 * @file CosaAT24CXX.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * @section Description
 * Cosa demonstration of the AT24CXX 2-Wire (TWI) Serial EEPROM
 * driver.
 *
 * @section Circuit
 * The TinyRTC with DS1307 also contains a 24C32 EEPROM.
 * @code
 *                       TinyRTC(24C32)
 *                       +------------+
 *                     1-|SQ          |
 *                     2-|DS        DS|-1
 * (A5/SCL)------------3-|SCL      SCL|-2
 * (A4/SDA)------------4-|SDA      SDA|-3
 * (VCC)---------------5-|VCC      VCC|-4
 * (GND)---------------6-|GND      GND|-5
 *                     7-|BAT         |
 *                       +------------+
 * @code
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <AT24CXX.h>
#include "Cosa/OutputPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"

// Use the builtin led as a heartbeat
OutputPin ledPin(Board::LED);

// The serial eeprom (sub-address 0b000) with binding to eeprom
AT24C32 at24c32(7);
EEPROM eeprom(&at24c32);

// Symbols for data stored in AT24CXX EEPROM memory address space
namespace Persistant {
  int x[6] EEMEM;
  uint8_t y[300] EEMEM;
  float z EEMEM;
};

void init_eeprom()
{
  int x[membersof(Persistant::x)];
  for (uint8_t i = 0; i < membersof(x); i++) x[i] = i;
  TRACE(eeprom.write(Persistant::x, x, sizeof(x)));

  uint8_t y[sizeof(Persistant::y)];
  memset(y, 0, sizeof(y));
  TRACE(eeprom.write(Persistant::y, y, sizeof(y)));

  float z = 1.0;
  TRACE(eeprom.write(&Persistant::z, z));
}

void setup()
{
  // Start trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAT24CXX: started"));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Initiate Persistant variables
  init_eeprom();
}

void loop()
{
  // Buffer for update loop
  uint8_t buffer[sizeof(Persistant::y)];
  memset(buffer, 0, sizeof(buffer));
  ledPin.toggle();

  // Read x and print contents
  TRACE(eeprom.read(buffer, &Persistant::x, sizeof(Persistant::x)));
  trace << PSTR("int Persistant::x[6]:") << endl;
  trace.print((uint32_t) &Persistant::x, buffer, sizeof(Persistant::x), IOStream::hex);
  trace << endl;

  // Read y, print contents and update
  TRACE(eeprom.read(buffer, &Persistant::y, sizeof(Persistant::y)));
  trace << PSTR("uint8_t Persistant::y[300]:") << endl;
  trace.print((uint32_t) &Persistant::y, buffer, sizeof(Persistant::y), IOStream::hex);
  trace << endl;
  for (size_t i = 0; i < sizeof(buffer); i++)
    buffer[i]++;
  TRACE(eeprom.write(&Persistant::y, buffer, sizeof(buffer)));
  eeprom.write_await();
  TRACE(eeprom.is_ready());
  trace << endl;

  // Read z and update
  float z = 0.0;
  TRACE(eeprom.read(&z, &Persistant::z));
  trace << PSTR("float z:") << z << endl;
  trace << endl;
  z += 0.5;
  TRACE(eeprom.write(&Persistant::z, z));

  ledPin.toggle();
  sleep(2);
}
