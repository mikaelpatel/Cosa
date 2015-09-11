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

// Configuration
#define COUNT 250
#define PRESCALE 64
#define TIMER_MAX (COUNT - 1)
#define US_PER_TIMER_CYCLE (PRESCALE / I_CPU)
#define US_PER_TICK (COUNT * US_PER_TIMER_CYCLE)
#define MS_PER_TICK (US_PER_TICK / 1000)
#define US_DIRECT_EXPIRE (800 / I_CPU)
#define US_TIMER_EXPIRE (US_PER_TICK - 1)

// Initiated state
bool RTC::s_initiated = false;

// Micro-seconds counter (fraction in timer register)
volatile uint32_t RTC::s_micros = 0UL;

// Milli-seconds counter
volatile uint32_t RTC::s_millis = 0UL;

// Job scheduler
RTC::Scheduler* RTC::s_scheduler = NULL;

// Timer job
Job* RTC::s_job = NULL;

// RTC alarm clock
RTC::Clock* RTC::s_clock = NULL;

bool
RTC::Scheduler::start(Job* job)
{
  // Check that the job is not already started
  if (job->is_started()) return (false);

  // Check if the job should be run directly
  uint32_t now = RTC::micros();
  int32_t diff = job->expire_at() - now;
  if (diff < US_DIRECT_EXPIRE) {
    job->on_expired();
    return (true);
  }

  // Check if the job should use the timer match register
  if (diff < US_TIMER_EXPIRE) {
    if ((s_job == NULL)
	|| ((int32_t) (job->expire_at() - s_job->expire_at()) < 0)) {
      synchronized {
	uint16_t cnt = TCNT0 + (diff / US_PER_TIMER_CYCLE);
	if (cnt > COUNT) cnt -= COUNT;
	OCR0B = cnt;
	TIMSK0 |= _BV(OCIE0B);
	TIFR0 |= _BV(OCF0B);
	s_job = job;
	m_queue.get_succ()->attach(job);
      }
      return (true);
    }
  }

  // Insert into the job scheduler queue
  synchronized {
    Linkage* succ = &m_queue;
    Linkage* curr;
    while ((curr = succ->get_pred()) != &m_queue) {
      int32_t diff = ((Job*) curr)->expire_at() - job->expire_at();
      if (diff < 0) break;
      succ = curr;
    }
    succ->attach(job);
  }
  return (true);
}

void
RTC::Scheduler::dispatch()
{
  // Check if there are no jobs
  if (m_queue.is_empty()) return;

  // Run all jobs that have expired
  uint32_t now = RTC::micros();
  Job* job = (Job*) m_queue.get_succ();
  while ((Linkage*) job != &m_queue) {
    // Check if the job should be run
    int32_t diff = job->expire_at() - now;
    if (diff < US_DIRECT_EXPIRE) {
      Job* succ = (Job*) job->get_succ();
      ((Link*) job)->detach();
      job->on_expired();
      job = succ;
      continue;
    }

    // Check if the job should use the timer
    if (diff < US_TIMER_EXPIRE) {
      // Check that the job will expire before current match
      if ((s_job == NULL)
	  || ((int32_t) (job->expire_at() - s_job->expire_at()) < 0)) {
	synchronized {
	  uint16_t cnt = TCNT0 + (diff / US_PER_TIMER_CYCLE);
	  if (cnt > COUNT) cnt -= COUNT;
	  OCR0B = cnt;
	  TIMSK0 |= _BV(OCIE0B);
	  TIFR0 |= _BV(OCF0B);
	  s_job = job;
	}
      }
   }

    // No more jobs to run
    return;
  }
}

uint32_t
RTC::Scheduler::time()
{
  return (RTC::micros());
}

bool
RTC::begin()
{
  // Should not be already initiated
  if (UNLIKELY(s_initiated)) return (false);

  synchronized {
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
  while (RTC::since(start) < ms) yield();
}

int
RTC::await(volatile bool &condvar, uint32_t ms)
{
  uint32_t start = RTC::millis();
  while (!condvar && ((ms == 0) || (RTC::since(start) < ms))) yield();
  return (!condvar ? ETIME : 0);
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

