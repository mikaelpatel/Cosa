/**
 * @file Cosa/RTC.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"

// Real-Time Clock configuration
#define COUNT 255
#define PRESCALE 64
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK ((COUNT + 1) * US_PER_TIMER_CYCLE)
#define US_PER_MS 1000
#define MS_PER_SEC 1000
#define MS_PER_TICK (US_PER_TICK / 1000)
#define US_ERR_PER_TICK (US_PER_TICK % 1000)

// Initiated state
bool RTC::s_initiated = false;

// Timer ticks counter
volatile uint32_t RTC::s_uticks = 0UL;
volatile uint16_t RTC::s_usec = 0;
volatile uint32_t RTC::s_ms = 0UL;
volatile uint16_t RTC::s_msec = 0;
volatile clock_t RTC::s_sec = 0UL;

// Timer interrupt extension
RTC::InterruptHandler RTC::s_handler = NULL;
void* RTC::s_env = NULL;

bool
RTC::begin()
{
  if (UNLIKELY(s_initiated)) return (false);
  synchronized {
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));

    // And enable interrupt on overflow
    TIMSK0 = _BV(TOIE0);

    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }

  // Install delay function and mark as initiated
  ::delay = RTC::delay;
  s_initiated = true;

  return (true);
}

bool
RTC::end()
{
  // Check if initiated
  if (UNLIKELY(!s_initiated)) return (false);

  // Disable the timer interrupts and mark as not initiated
  synchronized TIMSK0 = 0;
  s_initiated = false;

  return (true);
}

uint16_t
RTC::us_per_tick()
{
  return (US_PER_TICK);
}

uint16_t
RTC::us_per_timer_cycle()
{
  return (US_PER_TIMER_CYCLE);
}

uint32_t
RTC::micros()
{
  uint32_t res;
  uint8_t cnt;

  // Read tick count and hardware counter. Adjust if pending interrupt
  synchronized {
    res = s_uticks;
    cnt = TCNT0;
    if ((TIFR0 & _BV(TOV0)) && cnt < COUNT) res += US_PER_TICK;
  }

  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

void
RTC::delay(uint32_t ms)
{
  uint32_t start = RTC::millis();
  while (RTC::since(start) < ms) yield();
}

int
RTC::await(volatile bool &condvar, uint32_t ms)
{
  int res = 0;
  if (ms != 0) {
    uint32_t start = millis();
    while (!condvar && since(start) < ms) yield();
    if (!condvar) res = ETIME;
  }
  else {
    while (!condvar) yield();
  }
  return (res);
}

ISR(TIMER0_OVF_vect)
{
  // Increment most significant part of micro seconds counter
  RTC::s_uticks += US_PER_TICK;

  // Increment milli-seconds counter
  RTC::s_ms += MS_PER_TICK;

  // Increment micro-seconds part of milli-seconds counter
  RTC::s_usec += US_ERR_PER_TICK;
  if (UNLIKELY(RTC::s_usec >= US_PER_MS)) {
    RTC::s_usec -= US_PER_MS;
    RTC::s_ms += 1;
  }

  // Increment milli-seconds part of seconds counter
  RTC::s_msec += MS_PER_TICK;

  // Check for increment of seconds counter
  if (UNLIKELY(RTC::s_msec >= MS_PER_SEC)) {
    RTC::s_msec -= MS_PER_SEC;
    RTC::s_sec += 1;
  }

  // Check for extension of the interrupt handler
  RTC::InterruptHandler fn = RTC::s_handler;
  if (UNLIKELY(fn == NULL)) return;
  void* env = RTC::s_env;
  fn(env);
}
