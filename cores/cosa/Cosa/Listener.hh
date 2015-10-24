/**
 * @file Cosa/Listener.hh
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

#ifndef COSA_LISTENER_HH
#define COSA_LISTENER_HH

#include "Cosa/Linkage.hh"

/**
 * Template class for event listener. Allows dispatch of events onto a
 * keyed set of listeners. The key data type must have assignment and
 * at least operation== defined.
 * @param[in] T key type.
 */
template <typename T>
class Listener : public Link {
public:
  /**
   * Construct Listener.
   * @param[in] key default value for key.
   */
  Listener(T key) :
    Link(),
    m_key(key)
  {}

  /**
   * Return Listener key value.
   * @return key.
   */
  T key() const
  {
    return (m_key);
  }

  /**
   * Set Listener key value.
   * @param[in] key new value.
   */
  void key(T key)
  {
    m_key = key;
  }

  /**
   * Match Listener key with given value. Return true(1) if equal
   * else false(0).
   * @return bool.
   */
  bool match(T key) const
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

private:
  /** Listener key. */
  T m_key;
};

template <typename T>
void
Listener<T>::dispatch(Head* head, T key, uint8_t type, uint16_t value)
{
  Linkage* link = head->succ();
  while (link != head) {
    Listener<T>* listener = (Listener<T>*) link;
    if (listener->match(key)) listener->on_event(type, value);
    link = link->succ();
  }
}
#endif
