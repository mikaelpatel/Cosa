/**
 * @file CosaMatrix.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

typedef int CUBE[3][3][3];

CUBE cube EEMEM;

uint16_t get_cube_addr(uint8_t x, uint8_t y, uint8_t z)
{
   return (uint16_t) &cube[x][y][z];
}

template<class M>
uint16_t get_addr(uint8_t x, uint8_t y, uint8_t z, M m = NULL)
{
  return (uint16_t) &m[x][y][z];
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMatrix: started"));
  Watchdog::begin();
  trace << PSTR("get_addr<CUBE>(0,1,2)) = ")
	<< hex << get_addr<CUBE>(0,1,2)
	<< endl;
  trace << PSTR("get_cube_addr(0,1,2) = ")
	<< hex << get_cube_addr(0,1,2)
	<< endl;
}

void loop()
{
  ASSERT(true == false);
}

