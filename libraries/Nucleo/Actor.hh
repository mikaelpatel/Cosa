/**
 * @file Actor.hh
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

#ifndef COSA_NUCLEO_ACTOR_HH
#define COSA_NUCLEO_ACTOR_HH

#include "Thread.hh"

namespace Nucleo {

/**
 * The Cosa Nucleo Actor; message passing supported thread.
 */
class Actor : public Thread {
public:
  /**
   * Construct actor and initiate internals.
   */
  Actor() :
    Thread(),
    m_receiving(false),
    m_sending(),
    m_port(0),
    m_size(0),
    m_buf(NULL)
  {}

  /**
   * Send message in given buffer and with given size to actor. Given port
   * may be used as message identity. Returns size or negative error code.
   * Receiving actor is resumed.
   * @param[in] port or message identity.
   * @param[in] buf pointer to buffer (default NULL).
   * @param[in] size of message (default 0).
   * @return size or negative error code.
   */
  int send(uint8_t port, const void* buf = NULL, size_t size = 0);

  /**
   * Receive message to given buffer and with given max size to
   * actor. Returns sender, port and size or negative error code.
   * Sending actor is rescheduled.
   * @param[in,out] sender actor.
   * @param[in,out] port or message identity.
   * @param[in] buf pointer to buffer (default NULL).
   * @param[in] size of message (default 0)
   * @return size or negative error code.
   */
  int recv(Actor*& sender, uint8_t& port, void* buf = NULL, size_t size = 0);

protected:
  volatile bool m_receiving;
  Head m_sending;
  uint8_t m_port;
  size_t m_size;
  const void* m_buf;
};

};
#endif
