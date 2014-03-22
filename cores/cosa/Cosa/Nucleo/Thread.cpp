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


static void thread_delay(uint32_t ms) 
{
  Nucleo::Thread::get_running()->delay(ms);
}

static void thread_yield() 
{
  Nucleo::Thread::get_running()->yield();
}

static void thread_sleep(uint16_t s)
{
  Nucleo::Thread::get_running()->delay(s * 1000L);
}

namespace Nucleo {

Head Thread::s_delayed;
Thread Thread::s_main;
Thread* Thread::s_running = &s_main;
size_t Thread::s_top = MAIN_STACK_MAX;
uint8_t Thread::s_mode = SLEEP_MODE_IDLE;

void
Thread::init()
{
  s_main.attach(this);
  if (setjmp(m_context)) while (1) run();
}

void 
Thread::begin(Thread* thread, size_t size)
{
  if (thread != NULL) {
    uint8_t buf[s_top];
    UNUSED(buf);
    s_top += size;
    thread->init();
  }
  else {
    ::delay = thread_delay;
    ::sleep = thread_sleep;
    ::yield = thread_yield;
    s_main.run();
  }
}

void 
Thread::run() 
{ 
  while (1) { 
    if (!s_delayed.is_empty()) {
      uint32_t now = Watchdog::millis();
      Thread* thread = (Thread*) s_delayed.get_succ();
      while (thread != (Thread*) &s_delayed) {
	if (thread->m_expires > now) break;
	Thread* succ = (Thread*) thread->get_succ();
	this->attach(thread);
	thread = succ;
      }
    }
    Thread* thread = (Thread*) get_succ();
    if (thread != this) 
      resume(thread);
    else
      Power::sleep(s_mode);
  }
}

void
Thread::resume(Thread* thread)
{
  if (setjmp(m_context)) return;
  s_running = thread;
  longjmp(thread->m_context, 1);
}

void
Thread::enqueue(Head* queue)
{
  Thread* thread = (Thread*) get_succ();
  queue->attach(this);
  resume(thread);
}

void
Thread::dequeue(Head* queue, bool flag)
{
  if (queue->is_empty()) return;
  Thread* thread = (Thread*) queue->get_succ();
  if (flag) {
    attach(thread);
    resume(thread);
  }
  else {
    get_succ()->attach(thread);
  }
}

void 
Thread::delay(uint32_t ms)
{
  m_expires = Watchdog::millis() + ms;
  Thread* thread = (Thread*) s_delayed.get_succ();
  while (thread != (Thread*) &s_delayed) {
    if (thread->m_expires > m_expires) break;
    thread = (Thread*) thread->get_succ();
  }
  enqueue((Head*) thread);
}

void 
Thread::await(volatile uint8_t* ptr, uint8_t bit)
{
  while ((*ptr & _BV(bit)) == 0) yield();
}

};


