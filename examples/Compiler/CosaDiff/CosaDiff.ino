/**
 * @file CosaDiff.ino
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
 * Testing code generation of arithmetric and comparison of unsigned
 * numbers. Force wrap-around uint32_t and show how this works.
 * Validate RTC/Watchdog::since(start) and 32->16 bit truncation
 * of counters.
 *
 * The conclusion is that difference should be signed and comparison
 * operators should be avoided. Instead compare with difference to
 * handle wrap-around and negative interval correctly. Or simply use
 * unsigned arithmetrics.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDiff: started"));
  Watchdog::begin();
}

void iter(uint32_t t0, uint32_t t1, uint8_t i0, uint8_t i1)
{
  for (int i = 0; i < 32; i++) {
    uint16_t s0 = t0;
    uint16_t s1 = t1;
    int16_t diff16 = s1 - s0;
    uint16_t udiff16 = s1 - s0;
    int32_t diff32 = t1 - t0;
    uint32_t udiff32 = t1 - t0;
    bool lessthan16 = s1 < s0;
    bool lessthan32 = t1 < t0;
    bool diffzeroless16 = diff16 < 0;
    bool diffzeroless32 = diff32 < 0;
    trace << i
	  << PSTR(":v0=") << t0 << ',' << s0
	  << PSTR(",v1=") << t1 << ',' << s1
	  << PSTR(",diff=") << diff32 << ',' << diff16
	  << PSTR(",udiff=") << udiff32 << ',' << udiff16;
    if ((lessthan16 != diffzeroless16) || (lessthan32 != diffzeroless32)) {
      trace << PSTR(",(v1 < v0)=")
	    << lessthan32 << ',' << lessthan16
	    << PSTR(",((v1-v0) < 0)=")
	    << diffzeroless32
	    << ',' << diffzeroless16
	    << PSTR(":error");
    }
    trace << endl;
    t0 += i0;
    t1 += i1;
  }
  trace << endl;
}

void loop()
{
  iter(0xfffffff0UL, 0xfffffff4UL, 1, 1);
  iter(0xfffffff0UL, 0xfffffff0UL, 0, 1);
  iter(0xfffffff0UL, 0xfffffff4UL, 1, 0);
  ASSERT(true == false);
}

