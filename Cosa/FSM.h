/**
 * @file Cosa/FSM.h
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
 * Finite State Machine support class. States are represented as an
 * Event handler.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_FSM_H__
#define __COSA_FSM_H__

#include "Cosa/Thing.h"
#include "Cosa/Event.h"
#include "Cosa/Watchdog.h"

class FSM : public Thing {

public:
  /**
   * State handler function prototype. Event type and value.
   * @param[in] fsm finite state machine.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  typedef void (*StateHandler)(FSM* fsm, uint8_t type, uint16_t value);
  
private:
  StateHandler _state;

  /**
   * The first level event handler. Filters timeout events.
   * @param[in] it the target object.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  static void do_event(Thing* it, uint8_t type, uint16_t value)
  {
    FSM* fsm = (FSM*) it;
    if (type == Event::TIMEOUT_TYPE) fsm->cancel_timer();
    fsm->_state(fsm, type, value);
  }

public:
  FSM() : Thing(do_event), _state(0) { }
  
  /**
   * Start the state machine with the given state function.
   * @param[in] fn state handler.
   */
  void begin(StateHandler fn) 
  {
    set_state(fn);
    Event::push(Event::FSM_INIT_TYPE, this);
  }
  
  /**
   * Set new state handler for next event.
   * @param[in] fn state handler.
   */
  void set_state(StateHandler fn) 
  {
    _state = fn;
  }
  
  /**
   * Set timer for time out events and possible state transitions.
   * @param[in] timeout period in ms.
   */
  void set_timer(uint16_t timeout)
  {
    Watchdog::attach(this, timeout);
  }

  /**
   * Cancel a timer request. 
   */
  void cancel_timer()
  {
    detach();
  }
};

#endif

