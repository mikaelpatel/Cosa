/**
 * @file CosaBenchmarkUART.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Benchmarking IOStream and UART functions; measure time to print
 * characters, strings and numbers through the IOStream interface and
 * IOBuffer to the UART.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IOStream/Driver/UART.hh"

static IOBuffer<128> ibuf;
static IOBuffer<1024> obuf;

// Create UART1 and bind to the cout IOStream
UART uart(0, &ibuf, &obuf);

void setup()
{
  uart.begin(115200);
  trace.begin(&uart, PSTR("CosaBenchmarkUART: started"));
  TRACE(free_memory());
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  uint32_t start, stop;

  // Measure time to print character, string and number
  start = RTC::micros();
  trace << '1' << endl;
  stop = RTC::micros();
  trace << PSTR("one character:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << PSTR("1") << endl;
  stop = RTC::micros();
  trace << PSTR("one character string:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << 1 << endl;
  stop = RTC::micros();
  trace << PSTR("integer:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << 1L << endl;
  stop = RTC::micros();
  trace << PSTR("long integer:");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  start = RTC::micros();
  trace << '1' << '0' << endl;
  stop = RTC::micros();
  trace << PSTR("two characters:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << PSTR("10") << endl;
  stop = RTC::micros();
  trace << PSTR("two character string:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << 10 << endl;
  stop = RTC::micros();
  trace << PSTR("integer:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << 10L << endl;
  stop = RTC::micros();
  trace << PSTR("long integer:");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  start = RTC::micros();
  trace << '1' << '0' << '0' << endl;
  stop = RTC::micros();
  trace << PSTR("three characters:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << PSTR("100") << endl;
  stop = RTC::micros();
  trace << PSTR("three character string:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << 100 << endl;
  stop = RTC::micros();
  trace << PSTR("integer:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << 100L << endl;
  stop = RTC::micros();
  trace << PSTR("long integer:");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  // Measure time to print max integer; 8, 16 and 32 bit.
  start = RTC::micros();
  trace << (int8_t) 0x7f << endl;
  stop = RTC::micros();
  trace << PSTR("max int8_t:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << (int16_t) 0x7fff << endl;
  stop = RTC::micros();
  trace << PSTR("max int16_t:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << (int32_t) 0x7fffffffL << endl;
  stop = RTC::micros();
  trace << PSTR("max int32_t:");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  // Measure time to print max unsigned integer; 8, 16 and 32 bit.
  start = RTC::micros();
  trace << (uint8_t) 0xffU << endl;
  stop = RTC::micros();
  trace << PSTR("max uint8_t:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << (uint16_t) 0xffffU << endl;
  stop = RTC::micros();
  trace << PSTR("max uint16_t:");
  trace << stop - start << PSTR(" us") << endl;
  sleep(1);

  start = RTC::micros();
  trace << (uint32_t) 0xffffffffUL << endl;
  stop = RTC::micros();
  trace << PSTR("max uint32_t:");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  // Measure time to print unsigned integer; 16 and 32 bit.
  start = RTC::micros();
  trace.print(100U, 6, IOStream::dec);
  trace.println();
  stop = RTC::micros();
  trace << PSTR("uint16_t(digits=6):");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  start = RTC::micros();
  trace.print(100UL, 6, IOStream::dec);
  trace.println();
  stop = RTC::micros();
  trace << PSTR("uint32_t(digits=6):");
  trace << stop - start << PSTR(" us") << endl;
  trace << endl;
  sleep(1);

  start = RTC::micros();
  for (uint8_t i = 0; i < 64; i++) {
    for (char c = ' '; c < ' '+64; c++)
      trace << c;
    trace << endl;
  }
  uart.flush();
  stop = RTC::micros();
  trace << PSTR("output(") << 64 * 64 << PSTR(" characters):");
  trace << stop - start << PSTR(" us") << endl;
  ASSERT(true == false);
}
