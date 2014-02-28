/**
 * @file Cosa/Nucleo/Thread.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/Nucleo/Thread.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"

namespace Nucleo {

Thread Thread::s_main;
Thread* Thread::s_running = &s_main;
size_t Thread::s_top = MAIN_STACK_MAX;
bool Thread::s_go_idle;
uint8_t Thread::s_mode = SLEEP_MODE_IDLE;

void
Thread::init()
{
  m_state = 1;
  s_main.attach(this);
  if (setjmp(m_context)) while (1) run();
}

void 
Thread::begin(Thread* t, size_t size)
{
  if (t == NULL) s_main.run();
  s_top += size;
  uint8_t buf[s_top];
  t->init();
}

void 
Thread::run() 
{ 
  while (1) { 
    s_go_idle = true;
    yield(); 
    if (s_go_idle) Power::sleep(s_mode);
  }
}

void
Thread::resume(Thread* t)
{
  if (setjmp(m_context)) return;
  s_running = t;
  if (t->m_state) s_go_idle = false;
  longjmp(t->m_context, 1);
}

void
Thread::enqueue(Head* queue)
{
  Thread* t = (Thread*) get_succ();
  queue->attach(this);
  resume(t);
}

void
Thread::dequeue(Head* queue, bool flag)
{
  if (queue->is_empty()) return;
  Thread* t = (Thread*) queue->get_succ();
  if (flag) {
    attach(t);
    resume(t);
  }
  else {
    get_succ()->attach(t);
  }
}

void 
Thread::delay(uint32_t ms)
{
  m_state = 0;
  uint32_t start = Watchdog::millis();
  while (Watchdog::since(start) < ms) yield();
  m_state = 1;
}

};
