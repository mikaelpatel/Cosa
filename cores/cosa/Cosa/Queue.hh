/**
 * @file Cosa/Queue.hh
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

#ifndef COSA_QUEUE_HH
#define COSA_QUEUE_HH

#include "Cosa/Types.h"
#include "Cosa/Power.hh"

/**
 * Template class for ring-buffer for queueing data elements.
 * See Event::queue for an example of usage.
 * @param[in] T element class.
 * @param[in] nmemb number of elements in queue.
 * @pre nmemb is powerof(2) and max 128.
 */
template <class T, uint8_t NMEMB>
class Queue {
  static_assert(NMEMB && !(NMEMB & (NMEMB - 1)), "NMEMB should be power of 2");
public:
  /**
   * Construct a ring-buffer queue with given number of members
   * member type.
   */
  Queue() :
    m_put(0),
    m_get(0)
  {}

  /**
   * Return number of elements in queue.
   * @return available elements.
   */
  uint8_t available() const
    __attribute__((always_inline))
  {
    return ((NMEMB + m_put - m_get) & MASK);
  }

  /**
   * Number of elements room in queue.
   * @return room for elements.
   */
  uint8_t room() const
    __attribute__((always_inline))
  {
    return ((NMEMB - m_put + m_get - 1) & MASK);
  }

  /**
   * Enqueue given member data if storage is available. Return true(1)
   * if successful otherwise false(0). Synchronised operation as
   * interrupt handler may push events.
   * @param[in] data pointer to member data buffer.
   * @return boolean.
   * @pre data != NULL
   * @note atomic
   */
  bool enqueue(T* data);

  /**
   * Enqueue given member data in program memory if storage is available.
   * Return true(1) if successful otherwise false(0). Synchronised operation as
   * interrupt handler may push events.
   * @param[in] data pointer to member data buffer in program memory.
   * @return boolean.
   * @pre data != NULL
   * @note atomic
   */
  bool enqueue_P(const T* data);

  /**
   * Dequeue member data from queue to given buffer. Returns true(1)
   * if member was available and succcessful otherwise
   * false(0). Synchronised operation as interrupt handler may push
   * events.
   * @param[in,out] data pointer to member data buffer.
   * @pre data != NULL
   * @return boolean.
   * @note atomic
   */
  bool dequeue(T* data);

  /**
   * Await data to become available from queue. Will perform a system
   * sleep with the given sleep mode.
   * @param[in,out] data pointer to member data buffer.
   * @pre data != NULL
   * @note atomic
   */
  void await(T* data);

private:
  static const uint8_t MASK = (NMEMB - 1);
  volatile uint8_t m_put;
  volatile uint8_t m_get;
  T m_buffer[NMEMB];
};

template <class T, uint8_t NMEMB>
bool
Queue<T,NMEMB>::enqueue(T* data)
{
  synchronized {
    uint8_t next = (m_put + 1) & MASK;
    if (UNLIKELY(next == m_get)) return (false);
    m_buffer[next] = *data;
    m_put = next;
  }
  return (true);
}

template <class T, uint8_t NMEMB>
bool
Queue<T,NMEMB>::enqueue_P(const T* data)
{
  synchronized {
    uint8_t next = (m_put + 1) & MASK;
    if (UNLIKELY(next == m_get)) return (false);
    memcpy_P(&m_buffer[next], data, sizeof(T));
    m_put = next;
  }
  return (true);
}

template <class T, uint8_t NMEMB>
bool
Queue<T,NMEMB>::dequeue(T* data)
{
  synchronized {
    if (UNLIKELY(m_get == m_put)) return (false);
    uint8_t next = (m_get + 1) & MASK;
    m_get = next;
    *data = m_buffer[next];
  }
  return (true);
}

template <class T, uint8_t NMEMB>
void
Queue<T,NMEMB>::await(T* data)
{
  while (!dequeue(data)) yield();
}

#endif
