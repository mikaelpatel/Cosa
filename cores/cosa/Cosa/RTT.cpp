/**
 * @file Cosa/RTT.cpp
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

#include "Cosa/RTT.hh"
#include "Cosa/RTT_Config.hh"

// Initiated state
bool RTT::s_initiated = false;

// Micro-seconds counter (fraction in timer register)
uint32_t RTT::s_micros = 0UL;

// Milli-seconds counter
uint32_t RTT::s_millis = 0UL;

// Job scheduler
RTT::Scheduler* RTT::s_scheduler = NULL;

// RTT alarm clock
RTT::Clock* RTT::s_clock = NULL;

// Timer job
Job* RTT::s_job = NULL;

bool
RTT::begin()
{
  // Should not be already initiated
  if (UNLIKELY(s_initiated)) return (false);

  synchronized {
    // Power up the timers
    Power::timern_enable();

    // Set prescaling to 64
    TCCRnB = CSn;

    // Clear Timer on Compare Match with given Count. Enable interrupt
    TCCRnA = _BV(WGM01);
    OCRnA = TIMER_MAX;
    TIMSKn = _BV(OCIE0A);

    // Reset the counter and clear interrupts
    TCNTn = 0;
    TIFRn = 0;
  }

  // Install delay function and mark as initiated
  ::delay = RTT::delay;
  s_initiated = true;
  return (true);
}

bool
RTT::end()
{
  // Check if initiated
  if (UNLIKELY(!s_initiated)) return (false);

  synchronized {
    // Disable the timer interrupts
    TIMSKn = 0;

    // Power up the timers
    Power::timern_disable();
  }

  // Mark as not initiated
  s_initiated = false;
  return (true);
}

uint16_t
RTT::us_per_tick()
{
  return (US_PER_TICK);
}

uint16_t
RTT::us_per_timer_cycle()
{
  return (US_PER_TIMER_CYCLE);
}

uint32_t
RTT::micros()
{
  uint32_t res;
  uint8_t cnt;

  // Read micro-seconds and hardware counter. Adjust if pending interrupt
  synchronized {
    res = s_micros;
    cnt = TCNTn;
    if ((TIFRn & _BV(OCF0A)) && (cnt < TIMER_MAX)) res += US_PER_TICK;
  }

  // Convert ticks to micro-seconds
  res += ((uint32_t) cnt) * US_PER_TIMER_CYCLE;
  return (res);
}

uint32_t
RTT::millis()
{
  uint32_t res;
  uint8_t cnt;

  // Read milli-seconds. Adjust if pending interrupt
  synchronized {
    res = s_millis;
    cnt = TCNTn;
    if ((TIFRn & _BV(OCF0A)) && (cnt < TIMER_MAX)) res += MS_PER_TICK;
  }
  return (res);
}

void
RTT::delay(uint32_t ms)
{
  uint32_t start = RTT::millis();
  ms += 1;
  while (RTT::since(start) < ms) yield();
}

ISR(TIMERn_COMPA_vect)
{
  // Increment micro-seconds counter (fraction in timer)
  RTT::s_micros += US_PER_TICK;

  // Increment milli-seconds counter
  RTT::s_millis += MS_PER_TICK;

  // Dispatch expired jobs
  if ((RTT::s_scheduler != NULL) && (RTT::s_job == NULL))
    RTT::s_scheduler->dispatch();

  // Clock tick and dispatch expired jobs
  if (RTT::s_clock != NULL)
    RTT::s_clock->tick(MS_PER_TICK);
}

ISR(TIMERn_COMPB_vect)
{
  // Disable the timer match
  TIMSKn &= ~_BV(OCIE0B);

  // Dispatch expired jobs
  RTT::s_job = NULL;
  if (RTT::s_scheduler != NULL)
    RTT::s_scheduler->dispatch();
}

