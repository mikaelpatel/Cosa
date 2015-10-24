/**
 * @file Cosa/RTT_Scheduler.cpp
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

bool
RTT::Scheduler::start(Job* job)
{
  // Check that the job is not already started
  if (job->is_started()) return (false);

  // Check if the job should be run directly
  uint32_t now = RTT::micros();
  int32_t diff = job->expire_at() - now;
  if (diff < US_DIRECT_EXPIRE) {
    job->on_expired();
    return (true);
  }

  // Check if the job should use the timer match register
  if (diff < US_TIMER_EXPIRE) {
    synchronized {
      if ((s_job == NULL)
	  || ((int32_t) (job->expire_at() - s_job->expire_at()) < 0)) {
	uint16_t cnt = TCNTn + (diff / US_PER_TIMER_CYCLE);
	if (cnt > TIMER_MAX) cnt -= TIMER_MAX;
	OCRnB = cnt;
	TIMSKn |= _BV(OCIE0B);
	TIFRn |= _BV(OCF0B);
	s_job = job;
	m_queue.succ()->attach(job);
	return (true);
      }
    }
  }

  // Insert into the job scheduler queue
  synchronized {
    Linkage* succ = &m_queue;
    Linkage* curr;
    while ((curr = succ->pred()) != &m_queue) {
      int32_t diff = ((Job*) curr)->expire_at() - job->expire_at();
      if (diff < 0) break;
      succ = curr;
    }
    succ->attach(job);
  }
  return (true);
}

void
RTT::Scheduler::dispatch()
{
  // Check if there are no jobs
  if (m_queue.is_empty()) return;

  // Run all jobs that have expired
  Job* job = (Job*) m_queue.succ();
  while ((Linkage*) job != &m_queue) {
    // Check if the job should be run
    uint32_t now = RTT::micros();
    int32_t diff = job->expire_at() - now;
    if (diff < US_DIRECT_EXPIRE) {
      Job* succ = (Job*) job->succ();
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
	uint16_t cnt = TCNTn + (diff / US_PER_TIMER_CYCLE);
	if (cnt > TIMER_MAX) cnt -= TIMER_MAX;
	OCRnB = cnt;
	TIMSKn |= _BV(OCIE0B);
	TIFRn |= _BV(OCF0B);
	s_job = job;
      }
    }

    // No more jobs to run
    return;
  }
}

uint32_t
RTT::Scheduler::time()
{
  return (RTT::micros());
}
