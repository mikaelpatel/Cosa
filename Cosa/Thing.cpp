/**
 * @file Cosa/Thing.cpp
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
 * The Cosa class hierarchy root object; Thing. Supports double linked
 * circulic lists and basic event handler (virtual method). 
 * 
 * @section See Also
 * Things.hh for collection of Things, and Event.hh for details on
 * event types and parameter passing.  
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Thing.hh"

void 
Thing::attach(Thing* it)
{
  synchronized {
    // Check if it needs to be detached first
    if (it->m_succ != it) {
      it->m_succ->m_pred = it->m_pred;
      it->m_pred->m_succ = it->m_succ;
    }
    // Attach it as the new predecessor
    it->m_succ = this;
    it->m_pred = this->m_pred;
    this->m_pred->m_succ = it;
    this->m_pred = it;
  }
}

void
Thing::detach()
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

void 
Thing::on_event(uint8_t type, uint16_t value) 
{
}

