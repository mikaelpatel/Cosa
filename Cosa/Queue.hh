/**
 * @file Cosa/Queue.hh
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

#ifndef __COSE_QUEUE_HH__
#define __COSE_QUEUE_HH__

#include "Cosa/Types.h"
#include "Cosa/Power.hh"

/**
 * Template class for ring-buffer for queueing data elements.
 * Number of members, nmemb, must be power of 2.
 * See Event::queue for an example of usage.
 * @param[in] T element class.
 * @param[in] nmemb number of elements in queue.
 * @pre nmemb is powerof(2)
 */
template <class T, uint8_t nmemb>
class Queue {
public:
  /**
   * Maximum number elements in queue.
   */
  const uint8_t NMEMB;

private:
  volatile uint8_t m_put;
  volatile uint8_t m_get;
  T m_buffer[nmemb];

public:
  /**
   * Construct a ring-buffer queue with given number of members
   * member type.
   */
  Queue() :
    NMEMB(nmemb),
    m_put(0),
    m_get(0)
  {
  }

  /**
   * Return number of elements in queue.
   * @return available elements.
   */
  uint8_t available() 
  { 
    return ((m_put - m_get + (nmemb - 1)) % (nmemb - 1));
  }

  /**
   * Enqueue given member data if storage is available. Return true(1)
   * if successful otherwise false(0). Synchronised operation as
   * interrupt handler may push events.
   * @param[in] data pointer to member data buffer.
   * @return boolean.
   * @pre data != 0
   */
  bool enqueue(T* data);

  /**
   * Enqueue given member data in program memory if storage is available. 
   * Return true(1) if successful otherwise false(0). Synchronised operation as
   * interrupt handler may push events.
   * @param[in] data pointer to member data buffer in program memory.
   * @return boolean.
   * @pre data != 0
   */
  bool enqueue_P(const T* data);

  /**
   * Dequeue member data from queue to given buffer. Returns true(1)
   * if member was available and succcessful otherwise
   * false(0). Synchronised operation as interrupt handler may push
   * events. 
   * @param[in,out] data pointer to member data buffer.
   * @pre data != 0
   * @return boolean.
   */
  bool dequeue(T* data);

  /**
   * Await data to become available from queue. Will perform a system
   * sleep with the given sleep mode. 
   * @param[in,out] data pointer to member data buffer.
   * @param[in] mode sleep mode.
   * @pre data != 0
   */
  void await(T* data, uint8_t mode = SLEEP_MODE_IDLE);
};

template <class T, uint8_t nmemb>
bool
Queue<T,nmemb>::enqueue(T* data)
{
  synchronized {
    uint8_t next = (m_put + 1) & (nmemb - 1);
    if (next == m_get) synchronized_return (false);
    m_buffer[next] = *data;
    m_put = next;
  }
  return (true);
}

template <class T, uint8_t nmemb>
bool
Queue<T,nmemb>::enqueue_P(const T* data)
{
  synchronized {
    uint8_t next = (m_put + 1) & (nmemb - 1);
    if (next == m_get) synchronized_return (false);
    memcpy_P(&m_buffer[next], data, sizeof(T));
    m_put = next;
  }
  return (true);
}

template <class T, uint8_t nmemb>
bool
Queue<T,nmemb>::dequeue(T* data)
{
  if (m_get == m_put) return (false);
  synchronized {
    uint8_t next = (m_get + 1) & (nmemb - 1);
    m_get = next;
    *data = m_buffer[next];
  }
  return (true);
}

template <class T, uint8_t nmemb>
void
Queue<T,nmemb>::await(T* data, uint8_t mode)
{
  while (!dequeue(data)) Power::sleep(mode);
}

#endif
