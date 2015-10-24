/**
 * @file Cosa/Job_Scheduler.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "Cosa/Job.hh"

bool
Job::Scheduler::start(Job* job)
{
  // Check that the job is not already started
  if (job->is_started()) return (false);

  // Insert the job into the scheduler job queue
  synchronized {
    Linkage* succ = &m_queue;
    Linkage* curr;
    while ((curr = succ->pred()) != &m_queue) {
      int32_t diff = ((Job*) curr)->m_expires - job->m_expires;
      if (diff < 0) break;
      succ = curr;
    }
    succ->attach(job);
  }
  return (true);
}

bool
Job::Scheduler::stop(Job* job)
{
  // Check that the job is started
  if (!job->is_started()) return (false);
  job->detach();
  return (true);
}

void
Job::Scheduler::dispatch()
{
  // Check if the queue is empty
  if (m_queue.is_empty()) return;

  // Run all jobs that have expired
  Job* job = (Job*) m_queue.succ();
  while ((Linkage*) job != &m_queue) {
    uint32_t now = time();
    int32_t diff = now - job->expire_at();
    if (diff < 0) return;
    Job* succ = (Job*) job->succ();
    ((Link*) job)->detach();
    job->on_expired();
    job = succ;
  }
}
