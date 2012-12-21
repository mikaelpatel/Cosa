/**
 * @file Cosa/Queue.cpp
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
 * Ring-buffer for queueing data elements; events, serial data, etc.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Queue.hh"

bool
Queue::enqueue(void* data)
{
  if (m_length == m_nmemb) return (0);
  synchronized {
    memcpy(&m_buffer[m_put * m_msize], data, m_msize);
    m_length += 1;
    m_put += 1;
    if (m_put == m_nmemb) m_put = 0;
  }
  return (1);
}

bool
Queue::enqueue_P(const void* data)
{
  if (m_length == m_nmemb) return (0);
  synchronized {
    memcpy(&m_buffer[m_put * m_msize], data, m_msize);
    m_length += 1;
    m_put += 1;
    if (m_put == m_nmemb) m_put = 0;
  }
  return (1);
}

bool
Queue::dequeue(void* data)
{
  if (m_length == 0) return (0);
  synchronized {
    memcpy(data, &m_buffer[m_get * m_msize], m_msize);
    m_length -= 1;
    m_get += 1;
    if (m_get == m_nmemb) m_get = 0;
  }
  return (1);
}

void
Queue::await(void* data, uint8_t mode)
{
  while (!dequeue(data)) {
    cli();
    set_sleep_mode(mode);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  }
}

