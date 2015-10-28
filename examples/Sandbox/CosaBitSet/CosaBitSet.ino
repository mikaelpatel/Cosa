/**
 * @file CosaBitSet.ino
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
 * Demonstrate Cosa BitSet class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/BitSet.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

enum {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  MAGENTA,
  CYAN
};

void setup()
{
  BitSet<68> a, b;

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBitSet: started"));
  Watchdog::begin();

  TRACE(free_memory());
  TRACE(sizeof(a));

  // Check that the bitset is empty
  ASSERT(b.is_empty());
  ASSERT(a == b);

  // Add and remove elements
  ASSERT(!b[GREEN]);
  b += GREEN;
  ASSERT(b[GREEN]);
  b -= GREEN;
  ASSERT(!b[GREEN]);
  b += RED;
  b += CYAN;
  ASSERT(b[RED] && b[CYAN]);
  trace << PSTR("B0:") << b << endl;

  // Assign and check that they are equal
  a = b;
  ASSERT(a == b);
  trace << PSTR("A0:") << a << endl;

  // Add element and another bitset
  a += a.members() / 2;
  trace << PSTR("A1:") << a << endl;

  // Try adding element outside the bitset
  b += 67;
  b += 68;
  b += 100;
  trace << PSTR("B1:") << b << endl;
  ASSERT(b[RED] && b[CYAN] && b[67]);
  ASSERT(!b[68] && !b[100]);

  // Remove bitset and the last element
  b -= a;
  b -= 67;
  trace << PSTR("B2:") << b << endl;
  ASSERT(b.is_empty());
  for (uint16_t i = 4; i < b.members(); i += 5)
    b += i;
  trace << PSTR("B3:") << b << endl;
}

void loop()
{
  ASSERT(true == false);
}
