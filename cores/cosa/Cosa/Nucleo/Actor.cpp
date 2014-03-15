/**
 * @file Cosa/Nucleo/Actor.cpp
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

#include "Cosa/Nucleo/Actor.hh"

namespace Nucleo {

int 
Actor::send(uint8_t port, const void* buf, size_t size)
{
  // Is not allowed to send to itself
  if (s_running == this) return (-1);
  if (m_sender != NULL) return (-2);

  // Store message in sender actor
  Actor* sender = (Actor*) s_running;
  synchronized {
    m_sender = sender;
    sender->m_port = port;
    sender->m_size = size;
    sender->m_buf = buf;
  }
  
  // Resume receiving actor if waiting
  if (m_receiving) {
    ((Linkage*) s_running)->attach(this);
    s_running->resume(this);
  }
  // Else wait for receiving actor
  else {
    Thread* thread = (Thread*) ((Linkage*) s_running)->get_succ();
    ((Link*) s_running)->detach();
    s_running->resume(thread);
  }
  return (size);
}

int 
Actor::recv(Actor*& sender, uint8_t& port, void* buf, size_t size)
{
  if (s_running != this) return (-1);

  // Check if receiver needs to wait for sending actor
  uint8_t key = lock();
  if (m_sender == NULL) {
    m_receiving = true;
    Thread* thread = (Thread*) get_succ();
    detach();
    unlock(key);
    resume(thread);
    key = lock();
  }

  // Copy message parameters
  int res;
  sender = (Actor*) m_sender;
  port = sender->m_port;
  if (size < sender->m_size) {
    res = -2;
  }
  else {
    memcpy(buf, sender->m_buf, sender->m_size);
    res = sender->m_size;
  }
  m_sender = NULL;
  sender->m_buf = NULL;
  sender->m_size = 0;
  m_receiving = false;

  // Reschedule the sender
  attach(sender);
  unlock(key);
  return (res);
}

};

