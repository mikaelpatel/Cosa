/**
 * @file ProtoThread.cpp
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

#include "ProtoThread.hh"
#include "Cosa/Watchdog.hh"

Head ProtoThread::runq;

void
ProtoThread::on_event(uint8_t type, uint16_t value)
{
  if (UNLIKELY(m_state == WAITING)) detach();
  m_state = (type == Event::TIMEOUT_TYPE) ? TIMEOUT : RUNNING;
  on_run(type, value);
  if (m_state == RUNNING) {
    m_state = READY;
  }
  else if (m_state == TIMEOUT) {
    schedule(this);
  }
}

uint16_t
ProtoThread::dispatch(bool flag)
{
  uint16_t count = 0;
  // Check if events should be processed and the run queue is empty
  if (flag && runq.is_empty()) {
    Event event;
    Event::queue.await(&event);
    event.dispatch();
    count += 1;
  }
  // Iterate once through the run queue and call all threads run method
  Linkage* link = runq.succ();
  while (link != &runq) {
    Linkage* succ = link->succ();
    ProtoThread* thread = (ProtoThread*) link;
    thread->m_state = RUNNING;
    thread->on_run(Event::RUN_TYPE, 0);
    if (thread->m_state == RUNNING) {
      thread->m_state = READY;
    }
    link = succ;
    count += 1;
    // Check if events should be processed
    if (flag) {
      while (Event::queue.available()) {
	Event event;
	Event::queue.dequeue(&event);
	event.dispatch();
	count += 1;
      }
    }
  }
  // Return total number of function dispatch
  return (count);
}

void
ProtoThread::schedule(ProtoThread* thread)
{
  if (UNLIKELY(thread->m_state == TERMINATED)) thread->m_ip = 0;
  thread->m_state = READY;
  runq.attach(thread);
}
