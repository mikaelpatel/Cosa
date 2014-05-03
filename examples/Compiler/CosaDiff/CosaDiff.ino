/**
 * @file CosaDiff.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * Testing code generation of arithmetric and comparison of unsigned 
 * numbers. Force wrap-around uint32_t and show how this workds.
 *
 * The conclusion is that difference should be signed and comparison
 * operators should be avoided. Instead compare with difference to 
 * handle wrap-around and negative interval correctly.
 *   uint32_t t0, t1;
 *   int32_t diff = t1 - t0;
 *   bool wrong1 = t1 < t0; 
 *   bool wrong2 = (t1 - t0) < 0; 
 *   bool correct = diff < 0;
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDiff: started"));
}

void loop()
{
  iter(0xfffffff0UL, 0xfffffff4UL, 1, 1);
  iter(0xfffffff0UL, 0xfffffff0UL, 0, 1);
  iter(0xfffffff0UL, 0xfffffff4UL, 1, 0);
  ASSERT(true == false);
}

void iter(uint32_t t0, uint32_t t1, uint8_t i0, uint8_t i1)
{
  for (int i = 0; i < 32; i++) {
    int32_t diff = t1 - t0;
    bool wrong1 = t1 < t0;
    bool wrong2 = (t1 - t0) < 0;
    bool correct = diff < 0;
    trace << i 
	  << PSTR(":t0=") << t0
	  << PSTR(",t1=") << t1
	  << PSTR(",diff=") << diff
	  << PSTR(",wrong1=") << wrong1
	  << PSTR(",wrong2=") << wrong2
	  << PSTR(",correct=") << correct
	  << endl;
    t0 += i0;
    t1 += i1;
  }
  trace << endl;
}
