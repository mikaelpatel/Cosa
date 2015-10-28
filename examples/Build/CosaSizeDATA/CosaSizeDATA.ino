/**
 * @file CosaSizeDATA.ino
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
 * Test the limits of DATA size against the build system and boards.txt.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#define DATA_SIZE 256
//#define DATA_SIZE 1024
//#define DATA_SIZE 2048
//#define DATA_SIZE 2560

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/EEPROM.hh"

// Simple blob
uint8_t blob[DATA_SIZE] = {0};

void setup()
{
  // Use serial as output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSizeDATA: started"));

  blob[0] = 255;
}

void loop()
{
  trace << PSTR("running") << endl;

  // Take a nap
  sleep(5);
}
