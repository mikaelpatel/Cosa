/**
 * @file Cosa/Thing.h
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
 * The Cosa class hiearchy root object; Thing. Basic event
 * handler.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_THING_H__
#define __COSA_THING_H__

#include "Cosa/Types.h"

class Thing {

public:
  /**
   * Event handler function prototype.
   * @param[in] it the target object.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  typedef void (*EventHandler)(Thing* it, uint8_t type, uint16_t value);

protected:
  EventHandler _callback;
  Thing* _succ;
  Thing* _pred;

public:
  Thing(EventHandler callback = 0) : 
    _callback(callback),
    _succ(this),
    _pred(this)
  {}
  
  /**
   * Set the event handler for this thing.
   * @param[in] fn event handler.
   */
  void set_event_handler(EventHandler fn) 
  { 
    _callback = fn; 
  }

  /**
   * Get the event handler for this thing.
   * @return event handler.
   */
  EventHandler get_event_handler() 
  { 
    return (_callback);
  }

  /**
   * Return successor in sequence.
   * @return next thing.
   */
  Thing* get_succ() 
  {
    return (_succ);
  }

  /**
   * Return predecessor in sequence.
   * @return previous thing.
   */
  Thing* get_pred() 
  {
    return (_pred);
  }

  /**
   * Attach given thing as predecessor.
   * @param[in] it thing to add.
   */
  void attach(Thing* it)
  {
    synchronized {
      it->_succ = this;
      it->_pred = this->_pred;
      this->_pred->_succ = it;
      this->_pred = it;
    }
  }

  /**
   * Detach this thing from any things.
   */
  void detach()
  {
    synchronized {
      _succ->_pred = _pred;
      _pred->_succ = _succ;
      _succ = this;
      _pred = this;
    }
  }

  /**
   * Trampoline function for event dispatch.
   * @param[in] type the event type.
   * @param[in] value the event value.
   */
  void on_event(uint8_t type, uint16_t value)
  {
    if (_callback != 0) _callback(this, type, value);
  }
  void on_event(uint8_t type, void* value)
  {
    if (_callback != 0) _callback(this, type, (uint16_t) value);
  }
};

#endif
