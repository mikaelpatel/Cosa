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

namespace Nucleo {

Thread Thread::s_main;
size_t Thread::s_top = MAIN_STACK_MAX;

void
Thread::init()
{
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
  while (1) yield(); 
}

void
Thread::resume(Thread* t)
{
  if (setjmp(m_context)) return;
  longjmp(t->m_context, 1);
}

void 
Thread::delay(uint32_t ms)
{
  uint32_t start = Watchdog::millis();
  while (Watchdog::since(start) < ms) yield();
}

};
