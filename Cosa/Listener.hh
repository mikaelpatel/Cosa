/**
 * @file Cosa/Listener.hh
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

#ifndef __COSE_LISTENER_HH__
#define __COSE_LISTENER_HH__

#include "Cosa/Linkage.hh"

/**
 * Template class for event listener. Allows dispatch of events onto a
 * keyed set of listeners. The key data type must have assignment and 
 * at least operation== defined.
 * @param[in] T key type.
 */
template <typename T>
class Listener : public Link {
private:
  /** Listener key */
  T m_key;

public:
  /**
   * Construct Listener.
   * @param[in] key default value for key.
   */
  Listener(T key) : Link(), m_key(key) {}

  /**
   * Return Listener key value.
   * @return key.
   */
  T get_key() 
  { 
    return (m_key); 
  }

  /**
   * Set Listener key value.
   * @param[in] key new value.
   */
  void set_key(T key) 
  { 
    m_key = key; 
  }

  /**
   * Match Listener key with given value. Return true(1) if equal
   * else false(0).
   * @return bool.
   */
  bool match(T key) 
  { 
    return (m_key == key); 
  }

  /**
   * Dispatch given event type/value to all listeners in queue 
   * which match the given key.
   * @param[in] head of queue.
   * @param[in] key to match.
   * @param[in] type of event.
   * @param[in] value for event.
   */
  static void dispatch(Head* head, T key, uint8_t type, uint16_t value);
};

template <typename T>
void
Listener<T>::dispatch(Head* head, T key, uint8_t type, uint16_t value)
{
  Linkage* link = head->get_succ(); 
  while (link != head) {
    Listener<T>* listener = (Listener<T>*) link;
    if (listener->match(key)) listener->on_event(type, value);
    link = link->get_succ();
  }
}
#endif
