/**
 * @file CosaBenchmarkRTC.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * Cosa RTC (Real-Time Clock) Benchmark. Measurement and validate RTC
 * operations; micro- and milli-second. The benchmark shows the number
 * of micro-seconds required for access of the RTC values, how
 * accurate the DELAY macro and Watchdog delay is. 
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  uint32_t start, stop, err;

  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkRTC: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Print CPU clock and instructions per 1MHZ 
  TRACE(F_CPU);
  TRACE(I_CPU);

  // Start the timers
  Watchdog::begin();
  RTC::begin();

  // Check timer parameters
  TRACE(Watchdog::ms_per_tick());
  TRACE(Watchdog::ticks());
  TRACE(RTC::us_per_tick());
  TRACE(RTC::seconds());
  
  // Measure baseline
  start = RTC::micros();
  uint32_t us = RTC::micros();
  stop = RTC::micros();
  INFO("RTC::micros(): %ul us", stop - start);
  UNUSED(us);
  
  start = RTC::micros();
  uint32_t ms = RTC::millis();
  stop = RTC::micros();
  INFO("RTC::millis(): %ul us", stop - start);
  UNUSED(ms);

  start = RTC::micros();
  uint32_t sec = RTC::seconds();
  stop = RTC::micros();
  INFO("RTC::seconds(): %ul us", stop - start);
  UNUSED(sec);

  start = RTC::micros();
  RTC::delay(1);
  stop = RTC::micros();
  INFO("RTC::delay(1): %ul us", stop - start);

  start = RTC::micros();
  RTC::delay(10);
  stop = RTC::micros();
  INFO("RTC::delay(10): %ul us", stop - start);

  start = RTC::micros();
  DELAY(10);
  stop = RTC::micros();
  INFO("DELAY(10): %ul us", stop - start);

  start = RTC::micros();
  DELAY(100);
  stop = RTC::micros();
  INFO("DELAY(100): %ul us", stop - start);

  start = RTC::micros();
  SLEEP(1);
  stop = RTC::micros();
  INFO("SLEEP(1): %ul us", stop - start);

  start = RTC::micros();
  delay(10);
  stop = RTC::micros();
  INFO("delay(10): %ul us", stop - start);

  start = RTC::micros();
  delay(100);
  stop = RTC::micros();
  INFO("delay(100): %ul us", stop - start);

  start = RTC::micros();
  sleep(1);
  stop = RTC::micros();
  INFO("sleep(1): %ul us", stop - start);

  start = RTC::micros();
  yield();
  stop = RTC::micros();
  INFO("yield(): %ul us", stop - start);

  // Start the measurement
  TRACE(RTC::micros());
  TRACE(RTC::millis());
  TRACE(RTC::seconds());
  for (uint8_t i = 0; i < 5; i++) {
    Watchdog::delay(1000);
    TRACE(RTC::micros());
    TRACE(RTC::millis());
    TRACE(RTC::seconds());
  }

  // Measure and validate micro-second level (RTC)
  err = 0;
  for (uint32_t i = 0; i < 100000; i++) {
    start = RTC::micros();
    DELAY(100);
    stop = RTC::micros();
    uint32_t diff = stop - start;
    if (diff > 136) {
      trace.printf_P(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"), 
		     i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  TRACE(RTC::seconds());
  INFO("DELAY(100): 100000 measurement/validation (err = %ul)", err);

  // Measure and validate milli-second level (RTC)
  err = 0;
  for (uint32_t i = 0; i < 100; i++) {
    start = RTC::millis();
    RTC::delay(100);
    stop = RTC::millis();
    uint32_t diff = stop - start;
    if (diff > 115) {
      trace.printf_P(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"), 
		     i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  TRACE(RTC::seconds());
  INFO("RTC::delay(100): 100 measurement/validation (err = %ul)", err);

  // Measure and validate milli-second level (Watchdog)
  err = 0;
  for (uint32_t i = 0; i < 100; i++) {
    start = RTC::millis();
    Watchdog::delay(100);
    stop = RTC::millis();
    uint32_t diff = stop - start;
    if (diff > 115) {
      trace.printf_P(PSTR("%ul: start = %ul, stop = %ul, diff = %ul\n"), 
		     i, start, stop, diff);
      Watchdog::delay(128);
      err++;
    }
  }
  TRACE(RTC::seconds());
  INFO("Watchdog::delay(100): 100 measurement/validation (err = %ul)", err);
}

void loop()
{
  ASSERT(true == false);
}

/**
@section Output
CosaBenchmarkRTC: started
free_memory() = 1694
F_CPU = 16000000
I_CPU = 16
Watchdog::ms_per_tick() = 16
Watchdog::ticks() = 3
RTC::us_per_tick() = 1024
RTC::seconds() = 0
70:void setup():info:RTC::micros(): 8 us
76:void setup():info:RTC::millis(): 44 us
82:void setup():info:RTC::seconds(): 4 us
88:void setup():info:RTC::delay(1): 1184 us
93:void setup():info:RTC::delay(10): 10232 us
98:void setup():info:DELAY(10): 12 us
103:void setup():info:DELAY(100): 108 us
108:void setup():info:SLEEP(1): 1071672 us
113:void setup():info:delay(10): 16500 us
118:void setup():info:delay(100): 96388 us
123:void setup():info:sleep(1): 1073728 us
128:void setup():info:yield(): 600 us
RTC::micros() = 2748120
RTC::millis() = 2775
RTC::seconds() = 2
RTC::micros() = 3871028
RTC::millis() = 3871
RTC::seconds() = 3
RTC::micros() = 4945188
RTC::millis() = 4945
RTC::seconds() = 4
RTC::micros() = 6019328
RTC::millis() = 6019
RTC::seconds() = 6
RTC::micros() = 7093408
RTC::millis() = 7093
RTC::seconds() = 7
RTC::micros() = 8167504
RTC::millis() = 8167
RTC::seconds() = 8
RTC::seconds() = 19
156:void setup():info:DELAY(100): 100000 measurement/validation (err = 0)
RTC::seconds() = 29
173:void setup():info:RTC::delay(100): 100 measurement/validation (err = 0)
RTC::seconds() = 39
190:void setup():info:Watchdog::delay(100): 100 measurement/validation (err = 0)
195:void loop():assert:true == false
*/
