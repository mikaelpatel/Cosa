/**
 * @file Cosa/Thread.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Cosa implementation of protothreads; A protothread is a
 * low-overhead mechanism for concurrent programming.  Protothreads
 * function as stackless, lightweight threads providing a blocking
 * context cheaply using minimal memory per protothread (on the order
 * of single bytes). Cosa/Thread supports event to thread mapping and
 * timers.
 *
 * @section Limitations
 * The thread macro set should only be used within the Thread::run()
 * function. 
 *
 * @section Acknowledgements
 * Inspired by research and prototype by Adam Dunkels, Oliver Schmidt,
 * Thiermo, Muneeb Ali, and the protothread library by Larry Ruane.
 *
 * @section References
 * [1] Adam Dunkels et al, Protothreads: Simplifying Event-Driven
 * Programming of Memory-Constrained Embedded Systems, SenSys'06,
 * November 1-3, 2006, Boulder, Colorado, USA.
 * [2] Larry Ruane, protothread: An extremely lightweight thread
 * library for GCC, http://code.google.com/p/protothread/
 * [3] http://en.wikipedia.org/wiki/Protothreads
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Thread.hh"
#include "Cosa/Watchdog.hh"

Head Thread::runq;

void 
Thread::on_event(uint8_t type, uint16_t value)
{
  if (type == Event::TIMEOUT_TYPE) {
    m_state = TIMEOUT;
    detach();
  } 
  else {
    m_state = RUNNING;
  }
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
Thread::dispatch(uint8_t flag)
{
  Linkage* link = runq.get_succ(); 
  uint16_t count = 0;
  while (link != &runq) {
    Linkage* succ = link->get_succ();
    Thread* thread = (Thread*) link;
    thread->run(Event::RUN_TYPE, 0);
    link = succ;
    count += 1;
    if (flag) {
      while (Event::queue.available()) {
	Event event;
	Event::queue.dequeue(&event);
	event.dispatch();
	count += 1;
      }
    }
  }
  return (count);
}

