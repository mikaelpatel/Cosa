/**
 * @file Cosa/RTC_Scheduler.cpp
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
