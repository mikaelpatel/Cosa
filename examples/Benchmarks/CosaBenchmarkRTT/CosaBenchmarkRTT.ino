/**
 * @file CosaBenchmarkRTT.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa RTT (Real-Time Timer) Benchmark. Measurement and validate RTT
 * operations; micro- and milli-second. The benchmark shows the number
 * of micro-seconds required for access of the RTT values, how
 * accurate the DELAY macro and Watchdog delay is.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Clock.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

RTT::Clock clock;

void setup()
{
  uint32_t start, stop, err;

  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkRTT: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Print CPU clock and instructions per 1MHZ
  TRACE(F_CPU);
  TRACE(I_CPU);

  // Start the timers
  Watchdog::begin();
  RTT::begin();

  // Check timer parameters
  TRACE(Watchdog::ms_per_tick());
  TRACE(RTT::us_per_tick());
  TRACE(RTT::us_per_timer_cycle());
  TRACE(RTT::micros());
  TRACE(RTT::millis());
  TRACE(clock.time());

  // Measure baseline
  MEASURE("RTT::micros(): ", 1) RTT::micros();
  MEASURE("RTT::millis(): ", 1) RTT::millis();
  MEASURE("clock.time(): ", 1) clock.time();
  MEASURE("RTT::await(): ", 1) RTT::await();
  MEASURE("RTT::delay(1): ", 1)  RTT::delay(1);
  MEASURE("RTT::delay(10): ", 1) RTT::delay(10);
  MEASURE("Watchdog::await(): ", 1) Watchdog::await();
  MEASURE("Watchdog::delay(10): ", 1)  Watchdog::delay(10);
  MEASURE("Watchdog::delay(100): ", 1) Watchdog::delay(100);
  MEASURE("DELAY(10): ", 1) DELAY(10);
  MEASURE("DELAY(100): ", 1) DELAY(100);
  MEASURE("delay(10): ", 1) delay(10);
  MEASURE("delay(100): ", 1) delay(100);
  MEASURE("sleep(1): ", 1) sleep(1);
  MEASURE("yield(): ", 1) yield();

  // Start the measurement
  TRACE(RTT::micros());
  TRACE(RTT::millis());
  TRACE(clock.time());
  for (uint8_t i = 0; i < 5; i++) {
    Watchdog::delay(1000);
    TRACE(RTT::micros());
    TRACE(RTT::millis());
    TRACE(clock.time());
  }
  trace.flush();

  // Measure and validate micro-second level (RTT)
  err = 0;
  for (uint32_t i = 0; i < 100000; i++) {
    start = RTT::micros();
    DELAY(100);
    stop = RTT::micros();
    uint32_t diff = stop - start;
    if (diff > 136) {
      trace.printf(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"),
		   i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  TRACE(clock.time());
  INFO("DELAY(100): 100000 measurement/validation (err = %ul)", err);
  trace.flush();

  // Measure and validate milli-second level (RTT)
  err = 0;
  for (uint32_t i = 0; i < 100; i++) {
    start = RTT::millis();
    RTT::delay(100);
    stop = RTT::millis();
    uint32_t diff = stop - start;
    if (diff > 105) {
      trace.printf(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"),
		   i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  TRACE(clock.time());
  INFO("RTT::delay(100): 100 measurement/validation (err = %ul)", err);
  trace.flush();

  // Measure and validate milli-second level (Watchdog)
  err = 0;
  for (uint32_t i = 0; i < 100; i++) {
    start = RTT::millis();
    Watchdog::delay(100);
    stop = RTT::millis();
    uint32_t diff = stop - start;
    if (diff > 128) {
      trace.printf(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"),
		   i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  TRACE(clock.time());
  INFO("Watchdog::delay(100): 100 measurement/validation (err = %ul)", err);
}

void loop()
{
  ASSERT(true == false);
}
