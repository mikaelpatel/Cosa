/**
 * @file CosaBitSet.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Demonstrate Cosa BitSet class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/BitSet.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

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
  
  TRACE(b[GREEN]);
  b += GREEN;
  TRACE(b[GREEN]);
  b -= GREEN;
  TRACE(b[GREEN]);
  b += RED;
  b += CYAN;
  trace << PSTR("B0:");
  b.print(trace);

  a += a.members() / 2;
  trace << PSTR("A0:");
  a.print(trace);
  a += b;
  trace << PSTR("A1:");
  a.print(trace);

  b += 67;
  b += 68;
  b += 100;
  trace << PSTR("B1:");
  b.print(trace);

  b -= a;
  trace << PSTR("B2:");
  b.print(trace);

  b.empty();
  for (uint16_t i = 4; i < b.members(); i += 5)
    b += i;
  trace << PSTR("B3:");
  b.print(trace);
}

void loop()
{
  ASSERT(true == false);
}
