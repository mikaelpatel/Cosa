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
#include "Cosa/Power.hh"

bool
Queue::enqueue(void* data)
{
  if (m_length == NMEMB) return (false);
  synchronized {
    memcpy(&m_buffer[m_put * MSIZE], data, MSIZE);
    m_length += 1;
    m_put += 1;
    if (m_put == NMEMB) m_put = 0;
  }
  return (true);
}

bool
Queue::enqueue_P(const void* data)
{
  if (m_length == NMEMB) return (false);
  synchronized {
    memcpy(&m_buffer[m_put * MSIZE], data, MSIZE);
    m_length += 1;
    m_put += 1;
    if (m_put == NMEMB) m_put = 0;
  }
  return (true);
}

bool
Queue::dequeue(void* data)
{
  if (m_length == 0) return (false);
  synchronized {
    memcpy(data, &m_buffer[m_get * MSIZE], MSIZE);
    m_length -= 1;
    m_get += 1;
    if (m_get == NMEMB) m_get = 0;
  }
  return (true);
}

void
Queue::await(void* data, uint8_t mode)
{
  while (!dequeue(data)) Power::sleep(mode);
}

