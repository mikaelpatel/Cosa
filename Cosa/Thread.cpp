/**
 * @file Cosa/Thread.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Thread.hh"
#include "Cosa/Watchdog.hh"

Head Thread::runq;

void 
Thread::on_event(uint8_t type, uint16_t value)
{
  if (m_state == WAITING) detach();
  m_state = (type == Event::TIMEOUT_TYPE) ? TIMEOUT : RUNNING; 
  run(type, value);
  if (m_state == RUNNING) {
    m_state = READY;
  }
  else if (m_state == TIMEOUT) {
    schedule(this);
  } 
}

void 
Thread::set_timer(uint16_t ms)
{
  m_state = WAITING;
  Watchdog::attach(this, ms);
}

uint16_t
Thread::dispatch(bool flag)
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
  Linkage* link = runq.get_succ(); 
  while (link != &runq) {
    Linkage* succ = link->get_succ();
    Thread* thread = (Thread*) link;
    thread->m_state = RUNNING; 
    thread->run();
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
Thread::schedule(Thread* thread)
{
  if (thread->m_state == TERMINATED) thread->m_ip = 0;
  thread->m_state = READY;
  runq.attach(thread);
}
