/**
 * @file CosRTCBenchmark.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Cosa RTC (Real-Time Clock) Benchmark. Validate and measurements.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/UART.hh"

void setup()
{
  uint32_t start, stop, err;

  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinsBenchmark: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the timers
  Watchdog::begin();
  Watchdog::delay(1);
  RTC::begin();
  
  // Measure baseline
  start = RTC::micros();
  stop = RTC::micros();
  INFO("RTC::micros(): %ul", stop - start);

  start = RTC::micros();
  uint32_t ms = RTC::millis();
  stop = RTC::micros();
  INFO("RTC::millis(): %ul", stop - start);

  start = RTC::micros();
  DELAY(1);
  stop = RTC::micros();
  INFO("DELAY(1): %ul", stop - start);

  start = RTC::micros();
  Watchdog::delay(1);
  stop = RTC::micros();
  INFO("Watchdog::delay(1): %ul", stop - start);

  // Measure and validate micro-second level
  err = 0;
  for (uint32_t i = 0; i < 100000; i++) {
    start = RTC::micros();
    DELAY(100);
    stop = RTC::micros();
    uint32_t diff = stop - start;
    if (diff > 120) {
      trace.printf_P(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"), 
		     i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  INFO("DELAY(100): measurement/validation (err = %ul)", err);

  // Measure and validate milli-second level
  err = 0;
  for (uint32_t i = 0; i < 100; i++) {
    start = RTC::millis();
    Watchdog::delay(30);
    stop = RTC::millis();
    uint32_t diff = stop - start;
    if (diff > 36) {
      trace.printf_P(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"), 
		     i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  INFO("Watchdog::delay(30): measurement/validation (err = %ul)", err);
}

void loop()
{
}
