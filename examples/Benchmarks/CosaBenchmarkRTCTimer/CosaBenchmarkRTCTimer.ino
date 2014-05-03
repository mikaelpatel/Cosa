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
    while (!test.is_dispatched());
    if (RTCMeasure::start_queued_cycles > 0)
      break;
    sum_immediate += RTCMeasure::start_immediate_cycles;
  }
  uint32_t avg_immediate = (sum_immediate * us_per_timer_cycle) / s;
  max_immediate_expiration = s;
  trace.printf_P(PSTR("Expirations < %l us will be immediately dispatched\n"
		      "  Avg immediate dispatch = %l us (%l instructions)\n"),
		 max_immediate_expiration, avg_immediate, avg_immediate*I_CPU );
  uart.flush();

  uint32_t sum_start = 0;
  uint32_t sum_setup = 0;
  uint32_t sum_dispatch = 0;
  
  for (; s < 1024; s++) {
    test.expire_after(s);
    test.start();
    while (!test.is_dispatched());
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
  uart.flush();
#endif

  // microsecond test
  for (expected = 0; expected <= 2048;) {
    Simple::flag = false;
    start = RTC::micros();
    one_shot.expire_at(start + expected);
    one_shot.start();
    while (!Simple::flag); 
    actual = OneShot::time_stamp - start;
    trace.printf_P(PSTR("expire_after(%ul us): actual %l us\n"), 
		   expected, actual);
    uart.flush();
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
  while (!Simple::flag);
  actual = OneShot::time_stamp - start;
  trace.printf_P(PSTR("expire_after(%ul us): actual %ul us\n"), 
		 expected, actual);
  uart.flush();

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
    while (Simple::flag == (i & 1));
    stop = RTC::micros();
  }
  actual = stop - start;
  trace.printf_P(PSTR("1000X expire_at(n * %ul us): expected %ul, actual %ul us\n"),
		 us_per_tick, expected, actual);
  uart.flush();

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
  uart.flush();
}

void loop()
{
  ASSERT(true == false);
}

/**
@section Output
CosaBenchmarkRTCTimer: started
free_memory() = 1081
sizeof(Timer) = 10
sizeof(Simple) = 10
sizeof(OneShot) = 10
F_CPU = 16000000
I_CPU = 16
us_per_tick = 1024
RTC::millis() = 40
RTC::seconds() = 0
Expirations < 1024 us will be immediately dispatched
  Avg immediate dispatch = 34 us (544 instructions)
For queued expirations,
  Avg start = -1 us (-16 instructions)
  Avg setup = -1 us (-16 instructions)
  Avg dispatch = -1 us (-16 instructions)
expire_after(0 us): actual 16 us
expire_after(7 us): actual 16 us
expire_after(14 us): actual 16 us
expire_after(21 us): actual 16 us
expire_after(28 us): actual 16 us
expire_after(35 us): actual 16 us
expire_after(42 us): actual 16 us
expire_after(49 us): actual 16 us
expire_after(56 us): actual 16 us
expire_after(63 us): actual 68 us
expire_after(70 us): actual 76 us
expire_after(77 us): actual 84 us
expire_after(84 us): actual 88 us
expire_after(91 us): actual 96 us
expire_after(98 us): actual 104 us
expire_after(105 us): actual 112 us
expire_after(118 us): actual 124 us
expire_after(131 us): actual 136 us
expire_after(144 us): actual 148 us
expire_after(157 us): actual 164 us
expire_after(170 us): actual 176 us
expire_after(183 us): actual 188 us
expire_after(196 us): actual 200 us
expire_after(209 us): actual 216 us
expire_after(222 us): actual 228 us
expire_after(235 us): actual 240 us
expire_after(248 us): actual 252 us
expire_after(261 us): actual 268 us
expire_after(302 us): actual 308 us
expire_after(343 us): actual 348 us
expire_after(384 us): actual 388 us
expire_after(425 us): actual 432 us
expire_after(466 us): actual 472 us
expire_after(507 us): actual 512 us
expire_after(548 us): actual 552 us
expire_after(589 us): actual 596 us
expire_after(630 us): actual 636 us
expire_after(671 us): actual 672 us
expire_after(712 us): actual 716 us
expire_after(753 us): actual 760 us
expire_after(794 us): actual 800 us
expire_after(835 us): actual 840 us
expire_after(876 us): actual 880 us
expire_after(917 us): actual 924 us
expire_after(958 us): actual 964 us
expire_after(999 us): actual 1004 us
expire_after(1040 us): actual 1044 us
expire_after(1081 us): actual 1088 us
expire_after(1122 us): actual 1124 us
expire_after(1163 us): actual 1168 us
expire_after(1204 us): actual 1208 us
expire_after(1245 us): actual 1252 us
expire_after(1286 us): actual 1292 us
expire_after(1327 us): actual 1332 us
expire_after(1368 us): actual 1372 us
expire_after(1409 us): actual 1416 us
expire_after(1450 us): actual 1456 us
expire_after(1491 us): actual 1496 us
expire_after(1532 us): actual 1536 us
expire_after(1573 us): actual 1576 us
expire_after(1614 us): actual 1620 us
expire_after(1655 us): actual 1660 us
expire_after(1696 us): actual 1700 us
expire_after(1737 us): actual 1744 us
expire_after(1778 us): actual 1784 us
expire_after(1819 us): actual 1824 us
expire_after(1860 us): actual 1864 us
expire_after(1901 us): actual 1908 us
expire_after(1942 us): actual 1948 us
expire_after(1983 us): actual 1988 us
expire_after(2024 us): actual 2028 us
expire_after(1024 us): actual 1028 us
1000X expire_at(n * 1024 us): expected 1024000, actual 1028036 us
50 timers started in 4196us
elapsed time (0.5 + 49*0.1s = 5.400s): actual 5400008us
247:void loop():assert:true == false
*/
