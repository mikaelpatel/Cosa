/**
 * @file CosaMath.ino
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
 * Testing Math class functions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Math.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMath: started"));
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  // Random testing
  MEASURE("Random, uint16_t [0..1000]:", 1000)
    rand(1000);
  MEASURE("Random, uint16_t [-1000..1000]:", 1000)
    rand(-1000,1000);
  MEASURE("Random, uint32_t [0..10000]:", 1000)
    random(100000L);
  MEASURE("Random, uint32_t [-10000..10000]:", 1000)
    random(-10000L, 10000L);

  MEASURE("Random, uint16_t [0..1000] until 1000:", 1)
    while (rand(1000) != 1000);
  MEASURE("Random, uint16_t [-1000..1000] until -1000:", 1)
    while (rand(-1000,1000) != -1000);
  MEASURE("Random, uint16_t [-1000..1000] until 0:", 1)
    while (rand(-1000,1000) != 0);
  MEASURE("Random, uint16_t [-1000..1000] until 1000:", 1)
    while (rand(-1000,1000) != 1000);

  // Check log2
  ASSERT(log2<uint8_t>(0x00) == 0);
  ASSERT(log2<uint8_t>(0x01) == 1);
  ASSERT(log2<uint8_t>(0x80) == 8);

  ASSERT(log2<uint16_t>(0x0000) == 0);
  ASSERT(log2<uint16_t>(0x0001) == 1);
  ASSERT(log2<uint16_t>(0x0008) == 4);
  ASSERT(log2<uint16_t>(0x0080) == 8);
  ASSERT(log2<uint16_t>(0x0800) == 12);
  ASSERT(log2<uint16_t>(0x8000) == 16);

  // Map the world
  for (int x = -128; x < 1025; x += 128)
    trace << x << ':' << map<long,0,1023,-2500,2500>(x) << endl;

  // And constrain it
  for (int x = -128; x < 1025; x += 128)
    trace << x << ':' << constrain<int,0,1023>(x) << endl;

  ASSERT(true == false);
}

