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
   * State handler function prototype. Should return true(1) if
   * the event was handled otherwise false(0). This allows common
   * and hierarchical state handlers.
   * @param[in] fsm finite state machine.
   * @param[in] type the type of event.
   * @return bool
   */
  typedef bool (*StateHandler)(FSM* fsm, uint8_t type);
  
private:
  static const uint16_t TIMEOUT_REQUEST = 0xffff;
  StateHandler _state;
  uint16_t _period;
  uint16_t _param;

  /**
   * The first level event handler. Filters timeout events and
   * adapt to state handler function prototype.
   * @param[in] it the target object.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  static void do_event(Thing* it, uint8_t type, uint16_t value)
  {
    FSM* fsm = (FSM*) it;
    if (fsm->_period == TIMEOUT_REQUEST) fsm->cancel_timer();
    fsm->_param = value;
    fsm->_state(fsm, type);
  }

public:
  /**
   * Construct state machine with given initial state.
   * @param{in] init initial state handler.
   * @param{in] period timeout in all states.
   */
  FSM(StateHandler init, uint16_t period = 0) :
    Thing(do_event), 
    _state(init),
    _period(period),
    _param(0)
  {}
  
  /**
   * Set new state handler for next event.
   * @param[in] fn state handler.
   */
  void set_state(StateHandler fn) 
  {
    if (fn == 0) return;
    _state = fn;
  }
  
  /**
   * Set timeout period for all states.
   * @param[in] ms timeout.
   */
  void set_period(uint8_t ms)
  {
    _period = ms;
  }
  
  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get(uint16_t& param)
  {
    param = _param;
  }
  
  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get(void*& param)
  {
    param = (void*) _param;
  }
  
  /**
   * Send an event to the state machine.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  void send(uint8_t type, uint16_t value = 0)
  {
    Event::push(type, this, value);
  }
  
  /**
   * Send an event to the state machine.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  void send(uint8_t type, void* value)
  {
    Event::push(type, this, value);
  }
  
  /**
   * Start the state machine with a FSM_BEGIN_TYPE event.
   */
  bool begin()
  {
    if ((_period != 0) && (_period != TIMEOUT_REQUEST)) 
      Watchdog::attach(this, _period);
    send(Event::FSM_BEGIN_TYPE);
    return (1);
  }
  
  /**
   * End the state machine with a FSM_END_TYPE event.
   */
  void end()
  {
    cancel_timer();
    send(Event::FSM_END_TYPE);
  }
  
  /**
   * Set timer for time out events and possible state transitions.
   * @param[in] timeout period in ms.
   */
  void set_timer(uint16_t timeout)
  {
    _period = TIMEOUT_REQUEST;
    Watchdog::attach(this, timeout);
  }

  /**
   * Cancel a timer request. This is performed by default on a
   * timeout event.
   */
  void cancel_timer()
  {
    if (_period != 0) {
      detach();
      _period = 0;
    }
  }
};

#endif

