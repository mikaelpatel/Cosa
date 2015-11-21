/**
 * @file CosaBitSetString.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Demonstrate Cosa BitSet class; copy characters once.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/BitSet.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

char* strcpy1(char* dst, const char* src)
{
  BitSet<256> cs;
  uint8_t c;
  char* res = dst;
  while ((c = *src++) != 0) {
    if (!cs[c]) {
      cs += c;
      *dst++ = c;
    }
  }
  *dst = 0;
  return (res);
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR(__FILE__ ": started"));
}

void loop()
{
  const char* src = __FILE__ ": Nisse badar.";
  char dst[64];
  trace << strcpy1(dst, src) << endl;
  ASSERT(true == false);
}
