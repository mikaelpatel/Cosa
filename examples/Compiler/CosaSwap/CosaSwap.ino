/**
 * @file CosaSwap.ino
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
 * Compare Cosa/Types swap in assembler with gcc built-in swap.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSwap: started"));
  RTT::begin();
}

void loop()
{
  uint16_t x;
  uint32_t y;

  MEASURE("uint16_t __builtin_bswap16(1):", 1) {
    x = __builtin_bswap16(0xaabbU);
  }
  trace << hex << x << endl;

  MEASURE("uint32_t __builtin_bswap32(1):", 1) {
    y = __builtin_bswap32(0xaabbccddUL);
  }
  trace << hex << y << endl;

  MEASURE("uint16_t __builtin_bswap16(1+100):", 1) {
    x = __builtin_bswap16(0xaabbU);
    for (int i = 0; i < 100; i++) {
      x = __builtin_bswap16(x);
    }
  }
  trace << hex << x << endl;

  MEASURE("uint32_t __builtin_bswap32(1+100):", 1) {
    y = __builtin_bswap32(0xaabbccddUL);
    for (int i = 0; i < 100; i++) {
      y = __builtin_bswap32(y);
    }
  }
  trace << hex << y << endl;

  MEASURE("uint16_t swap(1):", 1) {
    x = swap(0xaabbU);
  }
  trace << hex << x << endl;

  MEASURE("uint32_t swap(1):", 1) {
    y = swap(0xaabbccddUL);
  }
  trace << hex << y << endl;

  MEASURE("uint16_t swap(1+100):", 1) {
    x = swap(0xaabbU);
    for (int i = 0; i < 100; i++) {
      x = swap(x);
    }
  }
  trace << hex << x << endl;

  MEASURE("uint32_t swap(1+100):", 1) {
    y = swap(0xaabbccddUL);
    for (int i = 0; i < 100; i++) {
      y = swap(y);
    }
  }
  trace << hex << y << endl;

  ASSERT(true == false);
}
