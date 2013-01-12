/**
 * @file Cosa/Linkage.hh
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
 * The Cosa class double linked circulic list.
 * 
 * @section Acknowlegments
 * These classes are inspired by the Simula-67 SIMSET Linkage classes. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_LINKAGE_HH__
#define __COSA_LINKAGE_HH__

#include "Cosa/Types.h"
#include "Cosa/Event.hh"

class Linkage : public Event::Handler {
protected:
  /**
   * Double linked list pointers. 
   */
  Linkage* m_succ;
  Linkage* m_pred;

  /**
   * Detach this linkage. Unlink from any list.
   */
  void detach();

public:
  /**
   * Construct this linkage and initiate to self reference.
   */
  Linkage() : 
    Event::Handler(),
    m_succ(this),
    m_pred(this)
  {}
  
  /**
   * Return successor in sequence.
   * @return successor linkage.
   */
  Linkage* get_succ() 
  {
    return (m_succ);
  }

  /**
   * Return predecessor in sequence.
   * @return predecessor linkage.
   */
  Linkage* get_pred() 
  {
    return (m_pred);
  }

  /**
   * Attach given linkage as predecessor. Will check and detach
   * if already attached.
   * @param[in] pred linkage to attach.
   */
  void attach(Linkage* pred);
};

class Link : public Linkage {
public:
  /**
   * Construct a link.
   */
  Link() : Linkage() {}

  /**
   * Detach this link. Unlink from any list.
   */
  void detach()
  {
    Linkage::detach();
  }
};

class Head : public Linkage {
public:
  /**
   * Construct a thing collection.
   */
  Head() : Linkage() {}
  
  /**
   * Return number of things.
   * @return length
   */
  uint8_t length();

  /**
   * Return true(1) if there are no things otherwise false(0).
   * @return bool.
   */
  bool is_empty()
  {
    return (m_succ == this);
  }

private:
  /**
   * @override
   * Event handler. Default event handler for collections. 
   * Will boardcase the event to the collection.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

#endif
