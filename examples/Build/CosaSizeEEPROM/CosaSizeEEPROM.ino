/**
 * @file CosaSizeEEPROM.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Test the limits of EEPROM size against the build system and boards.txt.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#define EEPROM_SIZE  512
//#define EEPROM_SIZE  513
//#define EEPROM_SIZE 1024
//#define EEPROM_SIZE 1025
//#define EEPROM_SIZE 2048
//#define EEPROM_SIZE 2049

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/EEPROM.hh"

// EEPROM access object
EEPROM eeprom;

// Simple blob in EEPROM
uint8_t blob[EEPROM_SIZE] EEMEM = {0};

void setup()
{
  // Use serial as output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSizeEEPROM: started"));

  ASSERT(eeprom.write(&blob[0], (uint8_t) 0xff) == sizeof(uint8_t));
}

void loop()
{
  trace << PSTR("running") << endl;

  // Take a nap
  sleep(5);
}
