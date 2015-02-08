/**
 * @file Cosa/Linkage.cpp
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

#include "Cosa/Linkage.hh"

#if defined(NON_INLINED)
void
Linkage::attach(Linkage* pred)
{
  synchronized {
    // Check if it needs to be detached first
    if (pred->m_succ != pred) {
      pred->m_succ->m_pred = pred->m_pred;
      pred->m_pred->m_succ = pred->m_succ;
    }
    // Attach pred as the new predecessor
    pred->m_succ = this;
    pred->m_pred = this->m_pred;
    this->m_pred->m_succ = pred;
    this->m_pred = pred;
  }
}

void
Linkage::detach()
{
  synchronized {
    // Check that the detach is necessary
    if (m_succ != this) {
      // Unlink and initiate to self reference
      m_succ->m_pred = m_pred;
      m_pred->m_succ = m_succ;
      m_succ = this;
      m_pred = this;
    }
  }
}
#endif

void
Head::on_event(uint8_t type, uint16_t value)
{
  // Iterate through the list and dispatch the event
  Linkage* link = m_succ;
  while (link != this) {
    // Get the successor as the current event call may detach itself
    Linkage* succ = link->get_succ();
    link->on_event(type, value);
    link = succ;
  }
}

int
Head::available()
{
  int res = 0;
  // Iterate through the list and count the length of the queue
  for (Linkage* link = m_succ; link != this; link = link->get_succ()) res++;
  return (res);
}



