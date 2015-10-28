/**
 * @file CosaSPIclock.ino
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
 * Verify SPI clock and cycle time mapping to SPI clock prescale.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSPIclock: started"));
}

void loop()
{
  INFO("mapping from frequency to clock setting", 0);
  for (uint32_t freq = 100000L; freq <= 10000000L; freq += 100000L) {
    SPI::Clock rate = SPI::Driver::clock(freq);
    trace << 1000000000L / freq << PSTR(" ns, ");
    trace << freq / 1000000.0 << PSTR(" MHz, ");
    trace << rate << endl;
  }
  trace << endl;

  INFO("mapping from cycle time to clock setting", 0);
  for (uint16_t ns = 25L; ns <= 1000L; ns += 25L) {
    SPI::Clock rate = SPI::Driver::cycle(ns);
    trace << ns << PSTR(" ns, ");
    trace << 1000.0 / ns << PSTR(" MHz, ");
    trace << rate << endl;
  }
  trace << endl;

  ASSERT(true == false);
}
