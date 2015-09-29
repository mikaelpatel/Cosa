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
#include "Cosa/RTC_Config.hh"

// Initiated state
bool RTC::s_initiated = false;

// Micro-seconds counter (fraction in timer register)
uint32_t RTC::s_micros = 0UL;

// Milli-seconds counter
uint32_t RTC::s_millis = 0UL;

// Job scheduler
RTC::Scheduler* RTC::s_scheduler = NULL;

// RTC alarm clock
RTC::Clock* RTC::s_clock = NULL;

// Timer job
Job* RTC::s_job = NULL;

bool
RTC::begin()
{
  // Should not be already initiated
  if (UNLIKELY(s_initiated)) return (false);

  synchronized {
    // Power up the timers
    Power::timer0_enable();

    // Set prescaling to 64
    TCCR0B = (_BV(CS01) | _BV(CS00));

    // Clear Timer on Compare Match with given Count. Enable interrupt
    TCCR0A = _BV(WGM01);
    OCR0A = TIMER_MAX;
    TIMSK0 = _BV(OCIE0A);

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

  synchronized {
    // Disable the timer interrupts
    TIMSK0 = 0;

    // Power up the timers
    Power::timer0_disable();
  }

  // Mark as not initiated
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

  // Read micro-seconds and hardware counter. Adjust if pending interrupt
  synchronized {
    res = s_micros;
    cnt = TCNT0;
    if ((TIFR0 & _BV(OCF0A)) && (cnt < TIMER_MAX)) res += US_PER_TICK;
  }

  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

uint32_t
RTC::millis()
{
  uint32_t res;
  uint8_t cnt;

  // Read milli-seconds. Adjust if pending interrupt
  synchronized {
    res = s_millis;
    cnt = TCNT0;
    if ((TIFR0 & _BV(OCF0A)) && (cnt < TIMER_MAX)) res += MS_PER_TICK;
  }
  return (res);
}

void
RTC::delay(uint32_t ms)
{
  uint32_t start = RTC::millis();
  ms += 1;
  while (RTC::since(start) < ms) yield();
}

ISR(TIMER0_COMPA_vect)
{
  // Increment micro-seconds counter (fraction in timer)
  RTC::s_micros += US_PER_TICK;

  // Increment milli-seconds counter
  RTC::s_millis += MS_PER_TICK;

  // Dispatch expired jobs
  if ((RTC::s_scheduler != NULL) && (RTC::s_job == NULL))
    RTC::s_scheduler->dispatch();

  // Clock tick and dispatch expired jobs
  if (RTC::s_clock != NULL)
    RTC::s_clock->tick(MS_PER_TICK);
}

ISR(TIMER0_COMPB_vect)
{
  // Disable the timer match
  TIMSK0 &= ~_BV(OCIE0B);

  // Dispatch expired jobs
  RTC::s_job = NULL;
  if (RTC::s_scheduler != NULL)
    RTC::s_scheduler->dispatch();
}

