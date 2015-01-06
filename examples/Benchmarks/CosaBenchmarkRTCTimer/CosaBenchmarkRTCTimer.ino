/**
 * @file CosaBenchmarkRTCTimer.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * Cosa RTC (Real-Time Clock) Benchmark. Validate and measurements.
 *
 * @section Circuit
 * This example requires no special circuit. Uses serial output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#define RTC_TIMER_MEASURE
#include "Cosa/RTC.hh"
#include "Cosa/Timer.hh"
#include "RTCMeasure.h"

class Simple : public Timer {
public:
  volatile static bool flag;
  virtual void on_expired() { flag = !flag; }
};
volatile bool Simple::flag = false;

class OneShot : public Simple {
public:
  volatile static uint32_t time_stamp;
  virtual void on_expired()
  {
    time_stamp = RTC::micros();
    flag = !flag;
  }
};
volatile uint32_t OneShot::time_stamp;

class TimerGroup : public Timer {
public:
  volatile static uint8_t started;
  virtual void start() { started++; Timer::start(); }
  virtual void on_expired() { started--; }
};
volatile uint8_t TimerGroup::started = 0;

static Simple timer;
static OneShot one_shot;

void setup()
{
  uint32_t start, stop, expected, actual;

  // Start the trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaBenchmarkRTCTimer: started"));

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(Timer));
  TRACE(sizeof(Simple));
  TRACE(sizeof(OneShot));
  
  // Print CPU clock and instructions per 1MHZ 
  TRACE(F_CPU);
  TRACE(I_CPU);

  RTC::begin();
  Timer::begin();

  // Check timer parameters
  uint32_t us_per_tick = RTC::us_per_tick();
  TRACE(us_per_tick);
  TRACE(RTC::millis());
  TRACE(RTC::seconds());

#if defined(RTC_TIMER_MEASURE)
  // Measure basic performance
  RTCMeasure test;
  
  uint32_t max_immediate_expiration = 0;
  uint32_t sum_immediate = 0;
  uint32_t s;
  uint32_t us_per_timer_cycle = RTC::us_per_timer_cycle();
  
  for (s = 0; s < 1024; s++) {
    test.expire_after( s );
    test.start();
    while (!test.is_dispatched())
      ;
    if (RTCMeasure::start_queued_cycles > 0)
      break;
    sum_immediate += RTCMeasure::start_immediate_cycles;
  }
  uint32_t avg_immediate = (sum_immediate * us_per_timer_cycle) / s;
  max_immediate_expiration = s;
  trace.printf_P(PSTR("Expirations < %l us will be immediately dispatched\n"
		      "  Avg immediate dispatch = %l us (%l instructions)\n"),
		 max_immediate_expiration, avg_immediate, avg_immediate*I_CPU );
  trace.flush();

  uint32_t sum_start = 0;
  uint32_t sum_setup = 0;
  uint32_t sum_dispatch = 0;
  
  for (; s < 1024; s++) {
    test.expire_after(s);
    test.start();
    while (!test.is_dispatched())
      ;
    sum_start += (uint32_t) RTCMeasure::start_queued_cycles;
    sum_setup += (uint32_t) RTCMeasure::setup_cycles;
    sum_dispatch += (uint32_t) RTCMeasure::dispatch_cycles;
  }
  uint32_t queued_samples = s - max_immediate_expiration;
  
  uint32_t avg_start = ((sum_start * us_per_timer_cycle) / queued_samples);
  uint32_t avg_setup = ((sum_setup * us_per_timer_cycle) / queued_samples);
  uint32_t avg_dispatch = ((sum_dispatch * us_per_timer_cycle) / queued_samples);
  
  trace.printf_P(PSTR("For queued expirations,\n"
		      "  Avg start = %l us (%l instructions)\n"
		      "  Avg setup = %l us (%l instructions)\n"
		      "  Avg dispatch = %l us (%l instructions)\n"),
		 avg_start, avg_start * I_CPU,
		 avg_setup, avg_setup * I_CPU,
		 avg_dispatch, avg_dispatch * I_CPU );
  trace.flush();
#endif

  // microsecond test
  for (expected = 0; expected <= 2048;) {
    Simple::flag = false;
    start = RTC::micros();
    one_shot.expire_at(start + expected);
    one_shot.start();
    while (!Simple::flag)
      ; 
    actual = OneShot::time_stamp - start;
    trace.printf_P(PSTR("expire_after(%ul us): actual %l us\n"), 
		   expected, actual);
    trace.flush();
    if (expected < 100)
      expected += 7;
    else if (expected < 256)
      expected += 13;
    else
      expected += 41;
  }
  
  // tick test
  Simple::flag = false;
  expected = us_per_tick;
  start = RTC::micros();
  one_shot.expire_at(start + expected);
  one_shot.start();
  while (!Simple::flag)
    ;
  actual = OneShot::time_stamp - start;
  trace.printf_P(PSTR("expire_after(%ul us): actual %ul us\n"), 
		 expected, actual);
  trace.flush();

  // 1000x tick tests
  Simple::flag = false;
  expected = 0;
  start = RTC::micros();
  timer.expire_at(start);
  for (uint16_t i = 0; i < 1000; i++) {
    expected += us_per_tick;
    // Make sure the expirations are evenly spaced, even if one
    // expires a little late or early. The next one will aim for the
    // correct expiration. 
    timer.expire_at(timer.expire_at() + us_per_tick);
    timer.start();
    while (Simple::flag == (i & 1))
      ;
    stop = RTC::micros();
  }
  actual = stop - start;
  trace.printf_P(PSTR("1000X expire_at(n * %ul us): expected %ul, actual %ul us\n"),
		 us_per_tick, expected, actual);
  trace.flush();

  TimerGroup bunch[50];
  const uint32_t base_offset = 500000;
  const uint32_t increment = 100000;
  expected = base_offset + (membersof(bunch) - 1) * increment;

  start = RTC::micros();
  for (uint32_t i = 0; i < membersof(bunch); i++)
    bunch[i].expire_at(start + base_offset + i * increment);
  uint32_t startStart = RTC::micros();
  for (int8_t i = membersof(bunch) - 1; i >= 0; i--) {
    bunch[i].start();
  }
  trace.printf_P(PSTR("%d timers started in %ulus\n"), 
		 membersof(bunch), RTC::micros() - startStart );

  uint32_t timeout = ((expected / 1000000) + 2) * 1000000;
  while (TimerGroup::started > 0) {
    stop = RTC::micros();
    if (stop - start > timeout) {
      trace.printf_P(PSTR("ERROR: %d timers still running\n"), 
		     TimerGroup::started );
      trace.printf_P(PSTR("  started at %ul, now=%ul, expiring at "), 
		     start, stop );
      for (uint8_t i=0; i < membersof(bunch); i++) {
        trace << bunch[i].expire_at() << PSTR(", ");
        bunch[i].stop();
      }
      trace << endl;
      break;
    }
  }
  stop = RTC::micros();
  actual = stop - start;
  
  uint32_t expected_ms = expected / 1000;
  trace.printf_P(PSTR("elapsed time (0.5 + %d*0.1s = %ul.%uls): actual %ulus\n"),
		 (membersof(bunch) - 1), 
		 (expected_ms / 1000), 
		 (expected_ms % 1000), 
		 actual);
  trace.flush();
  
  RTC::micros( 0xFFFFF000UL ); // 4095uS 'til rolloover
  expected = 5000;  // anywhere past the rolloever...
 
  Simple::flag = false;
  start = RTC::micros();
  one_shot.expire_at(start + expected);
  one_shot.start();
  while (!Simple::flag)
    ; 
  actual = OneShot::time_stamp - start;
  trace.printf_P(PSTR("Rollover test: start %ul, end %ul\n  expire_after(%ul us): actual %l us\n"), 
     start, OneShot::time_stamp, expected, actual);
  trace.flush();
}

void loop()
{
  ASSERT(true == false);
}
