/**
 * @file Nucleo/Semaphore.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "Semaphore.hh"
#include "Thread.hh"

using namespace Nucleo;

void
Semaphore::wait(uint8_t count)
{
  uint8_t key = lock();
  while (count > m_count) {
    unlock(key);
    Thread::s_running->enqueue(&m_queue);
    key = lock();
  }
  m_count -= count;
  unlock(key);
}

void
Semaphore::signal(uint8_t count, bool flag)
{
  synchronized {
    m_count += count;
  }
  Thread::s_running->dequeue(&m_queue, flag);
}
