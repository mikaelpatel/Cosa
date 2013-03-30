/**
 * @file Cosa/Queue.hh
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

#ifndef __COSE_QUEUE_HH__
#define __COSE_QUEUE_HH__

#include "Cosa/Types.h"
#include <avr/sleep.h>

class Queue {
public:
  const uint8_t NMEMB;
  const uint8_t MSIZE;

private:
  uint8_t m_length;
  uint8_t m_put;
  uint8_t m_get;
  uint8_t* m_buffer;

public:
  /**
   * Construct a ring-buffer queue with given number of members, 
   * member size and possible buffer. A buffer area is allocated
   * if the given buffer pointer is null.
   * @param[in] nmemb number of members.
   * @param[in] msize size of member.
   * @param[in] buffer pointer to buffer.
   */
  Queue(uint8_t nmemb, uint8_t msize, void* buffer = 0) :
    NMEMB(nmemb),
    MSIZE(msize),
    m_length(0),
    m_put(0),
    m_get(0),
    m_buffer((uint8_t*) (buffer != 0 ? buffer : malloc(msize * nmemb)))
  {
  }

  /**
   * Return length of queue.
   * @return length of queue.
   */
  uint8_t length() 
  { 
    return (m_length); 
  }

  /**
   * Return true(1) if members are available in queue otherwise
   * false(0).
   * @return boolean.
   */
  bool available() 
  { 
    return (m_length > 0); 
  }

  /**
   * Enqueue given member data if storage is available. Return true(1)
   * if successful otherwise false(0). Atomic operation.
   * @param[in] data pointer to member data buffer.
   * @return boolean.
   */
  bool enqueue(void* data);

  /**
   * Enqueue given member data in program memory if storage is available. 
   * Return true(1) if successful otherwise false(0). Atomic operation.
   * @return boolean.
   */
  bool enqueue_P(const void* data);

  /**
   * Dequeue member data from queue to given buffer. Returns true(1)
   * if member was available and succcessful otherwise
   * false(0). Atomic operation.
   * @param[in,out] data pointer to member data buffer.
   * @return boolean.
   */
  bool dequeue(void* data);

  /**
   * Await data to become available from queue. Will perform a system
   * sleep with the given sleep mode. 
   * @param[in,out] data pointer to member data buffer.
   * @param[in] mode sleep mode.
   */
  void await(void* data, uint8_t mode = SLEEP_MODE_IDLE);
};

#endif
