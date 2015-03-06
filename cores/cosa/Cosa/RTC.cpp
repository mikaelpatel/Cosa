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
#define TICKS_PER_SEC (1000000L / US_PER_TICK)
#define US_PER_SEC_ERROR (1000000L - (TICKS_PER_SEC * US_PER_TICK))

// Initiated state
bool RTC::s_initiated = false;

// Timer ticks counter
volatile uint32_t RTC::s_uticks = 0UL;
volatile uint16_t RTC::s_ticks = 0;
volatile clock_t RTC::s_sec = 0L;
volatile int16_t RTC::s_uerror = 0;

// Timer interrupt extension
RTC::InterruptHandler RTC::s_handler = NULL;
void* RTC::s_env = NULL;

bool
RTC::begin()
{
  if (s_initiated) return (false);
  synchronized {
    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));

    // And enable interrupt on overflow
    TIMSK0 = _BV(TOIE0);

    // Reset the counter and clear interrupts
    TCNT0 = 0;
    TIFR0 = 0;
  }
  s_initiated = true;
  ::delay = RTC::delay;
  return (true);
}

bool
RTC::end()
{
  // Disable the timer interrupts
  synchronized {
    TIMSK0 = 0;
  }
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
  if (ms == 0) {
    while (!condvar) yield();
    return (0);
  }

  uint32_t start = millis();
  while (!condvar && since(start) < ms) yield();
  return (condvar ? 0 : ETIME);
}

ISR(TIMER0_OVF_vect)
{
  // Increment most significant part of micro second counter
  RTC::s_uticks += US_PER_TICK;

  // Skip tick if accumulated error is greater than tick time
  if (RTC::s_uerror >= US_PER_TICK)
    RTC::s_uerror -= US_PER_TICK;
  else if (RTC::s_ticks == TICKS_PER_SEC-1) {
    RTC::s_ticks = 0;
    RTC::s_sec++;
    RTC::s_uerror += US_PER_SEC_ERROR;
  } else
    RTC::s_ticks++;

  // Check for extension of the interrupt handler
  RTC::InterruptHandler fn = RTC::s_handler;
  if (fn == NULL) return;
  void* env = RTC::s_env;
  fn(env);
}
