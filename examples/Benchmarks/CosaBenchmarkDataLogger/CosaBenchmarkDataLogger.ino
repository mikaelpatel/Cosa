/**
 * @file CosaBenchmarkDataLogger.ino
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
 * Benchmark Analog data-logging over IOStream/UART.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

AnalogPin probe(Board::A0);

static uint32_t idle = 0L;

void iowait()
{
  uint32_t start = RTC::micros();
  Power::sleep();
  uint32_t stop = RTC::micros();
  idle = (start > stop) ? 0L : idle + (stop - start);
}

void setup()
{
  // Start serial output with given baud-rate
  uart.begin(500000);
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
  trace.begin(&uart, PSTR("CosaBenchmarkDataLogger: started"));

  // Install capture idle time
  ::yield = iowait;

  // Start timers
  Watchdog::begin();
  RTC::begin();
}

void loop()
{
  uint16_t SAMPLES = 1000;
  float Ksps;

  // 1000 samples with 1 ms period; simple sample and print
  idle = 0L;
  MEASURE("performance:", 1) {
    for (uint16_t i = 0; i < SAMPLES;) {
      periodic(timer, 1) {
	trace << probe << endl;
	i++;
      }
      yield();
    }
    trace.flush();
  }
  INFO("per sample:%ul us", trace.measure / SAMPLES);
  Ksps = ((float) SAMPLES * 1000.0) / trace.measure;
  trace << PSTR("effective sample rate (") << SAMPLES << PSTR(" samples):")
	<< Ksps << PSTR(" Ksps")
	<< endl;
  trace << PSTR("idle:") << (idle * 100.0) / RTC::micros() << '%'
	<< endl;
  sleep(5);

  // 1000 samples with 125 us period; pipe-line sample request and print
  idle = 0L;
  MEASURE("performance:", 1) {
    for (uint16_t i = 0; i < SAMPLES;) {
      probe.sample_request();
      PERIODIC(timer, 500) {
	uint16_t sample = probe.sample_await();
	probe.sample_request();
	trace << sample << endl;
	i++;
      }
    }
    trace.flush();
  }
  INFO("per sample:%ul us", trace.measure / SAMPLES);
  Ksps = ((float) SAMPLES * 1000.0) / trace.measure;
  trace << PSTR("effective sample rate (") << SAMPLES << PSTR(" samples):")
	<< Ksps << PSTR(" Ksps")
	<< endl;
  trace << PSTR("idle:") << (idle * 100.0) / RTC::micros() << '%'
	<< endl;

  ASSERT(true == false);
}
