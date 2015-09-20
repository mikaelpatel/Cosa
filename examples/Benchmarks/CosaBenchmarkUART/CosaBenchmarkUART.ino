/**
 * @file CosaBenchmarkUART.ino
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
#include "Cosa/IOStream/Driver/UART.hh"

// Bits transmitted; 1 start bit, 8 data bits, 2 stop bits
static const uint32_t BITS = 11;

void setup()
{
  // Put baudrate setting as first line
  uart.begin(2000000);
  // uart.begin(1000000);
  // uart.begin(500000);
  // uart.begin(250000);
  // uart.begin(230400);
  // uart.begin(115200);
  // uart.begin(57600);
  // uart.begin(38400);
  // uart.begin(28800);
  // uart.begin(19200);
  // uart.begin(14400);
  // uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkUART: started"));
  TRACE(free_memory());
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  // Measure time to print character, string and number
  MEASURE("one character (new-line):", 1) trace << endl;
  MEASURE("one character:", 1) trace << '1' << endl;
  MEASURE("one character string:", 1) trace << PSTR("1") << endl;
  MEASURE("integer:", 1) trace << 1 << endl;
  MEASURE("long integer:", 1) trace << 1L << endl;
  MEASURE("two characters:", 1) trace << '1' << '0' << endl;
  MEASURE("two character string:", 1) trace << PSTR("10") << endl;
  MEASURE("integer:", 1) trace << 10 << endl;
  MEASURE("long integer:", 1) trace << 10L << endl;
  MEASURE("floating point:", 1) trace << 10.0 << endl;
  MEASURE("three characters:", 1) trace << '1' << '0' << '0' << endl;
  MEASURE("three character string:", 1) trace << PSTR("100") << endl;
  MEASURE("integer:", 1) trace << 100 << endl;
  MEASURE("long integer:", 1) trace << 100L << endl;
  MEASURE("floating point:", 1) trace << 100.0 << endl;

  // Measure time to print max integer << endl; 8, 16 and 32 bit.
  MEASURE("max int8_t:", 1) trace << (int8_t) 0x7f << endl;
  MEASURE("max int16_t:", 1) trace << (int16_t) 0x7fff << endl;
  MEASURE("max int32_t:", 1) trace << (int32_t) 0x7fffffffL << endl;

  // Measure time to print max unsigned integer << endl; 8, 16 and 32 bit.
  MEASURE("max uint8_t:", 1) trace << (uint8_t) 0xffU << endl;
  MEASURE("max uint16_t:", 1) trace << (uint16_t) 0xffffU << endl;
  MEASURE("max uint32_t:", 1) trace << (uint32_t) 0xffffffffUL << endl;

  // Measure time to print unsigned integer; 16 and 32 bit.
  MEASURE("uint16_t(digits=6):", 1) {
    trace.print(100U, 6, IOStream::dec);
    trace.println();
  }
  MEASURE("uint32_t(digits=6):", 1) {
    trace.print(100UL, 6, IOStream::dec);
    trace.println();
  }

  // Measure time to print some special characters
  MEASURE("newline character:", 1) trace << '\n';
  MEASURE("tab:", 1) trace << '\t' << endl;
  MEASURE("newline string(1):", 1) trace << (char*) "\n";
  MEASURE("newline string(2):", 1) trace << (char*) "\n\n";

  // Measure time to print all characters
  delay(10);
  uint16_t n = 0;
  MEASURE("performance:", 1) {
    for (uint8_t i = 0; i < 10; i++) {
      for (uint8_t c = ' '; c < ' '+95; c++, n++)
	trace << (char) c;
      trace << endl;
      n += 2;
    }
    trace.flush();
  }
  delay(10);
  uint32_t Kbps = (BITS * n * 1000UL) / trace.measure;
  trace << PSTR("effective baudrate (") << n << PSTR(" characters):")
	<< Kbps << PSTR(" Kbps")
	<< endl;

  ASSERT(true == false);
}
