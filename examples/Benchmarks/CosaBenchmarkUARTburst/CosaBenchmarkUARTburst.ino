/**
 * @file CosaBenchmarkUARTburst.ino
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
 * Benchmarking IOStream and UART functions; measure time to print
 * approx. 1 Mbyte (10.000 lines with 104 characters per line, 98
 * character string and 5 digits line number). Default format: 11 bits,
 * 1 start bit, 8 data bits, and 2 stop bits.
 *
 * The print of the line number is an integer to string conversion
 * with 5 digits. Up to 500 Kbps the conversion is in parallel with
 * the print and full effective baudrate is achieved.
 *
 * The sketch will also capture idle time (mostly waiting for buffer
 * space). At 115.200 bps the idle time is almost 80%. Only 20% of the
 * processing power is needed to handle the transmission and number to
 * string conversion. At 250 Kbps the idle time drops to 62% and at
 * 500 Kbps it is down to 12%. Higher baudrates require all processing
 * power and there is no idle time.
 *
 * The measurements are for command line build with link-time
 * optimization enabled, run on an Arduino Pro-Mini with a SparkFun
 * FDTI Basic.
 *
 * @section Warning
 * At 2 Mbps the execution time is about 7 seconds, at 500 Kbps
 * about 22 seconds, at 115.200 bps over 95 seconds. Running at
 * 9.600 bps will take nearly 20 minutes!
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

static uint32_t idle = 0L;

void iowait()
{
  uint32_t start = RTT::micros();
  Power::sleep();
  uint32_t stop = RTT::micros();
  idle = (start > stop) ? 0L : idle + (stop - start);
}

void setup()
{
  // Start serial output with given baud-rate
  uart.begin(1000000);
  // uart.begin(2000000);
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
  trace.begin(&uart, PSTR("CosaBenchmarkUARTburst: started"));

  // Install capture idle time
  ::yield = iowait;

  // Start timers
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  str_P s = PSTR(" !\"#$%&'()*+,-./0123456789:;<=>?@"
		 "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
		 "abcdefghijklmnopqrstuvwxyz{|}~\r\n");
  const uint16_t LINES = 10000;
  const uint32_t CHARS = (uint32_t) LINES * (strlen_P(s) + 6);
  const uint32_t BITS = 11 * CHARS;
  MEASURE("performance:", 1) {
    for (uint16_t i = LINES; i < LINES * 2; i++) trace << i << ':' << s;
    trace.flush();
  }
  const float Mbps = ((float) BITS) / trace.measure;
  trace << PSTR("effective baudrate (") << CHARS << PSTR(" characters):")
	<< Mbps << PSTR(" Mbps")
	<< endl;
  trace << PSTR("idle:") << (idle * 100.0) / RTT::micros() << '%'
	<< endl;
  ASSERT(true == false);
}
