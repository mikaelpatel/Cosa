/**
 * @file Cosa/FSM.hh
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
 * Event handler. Supports timeout events and period timed state 
 * machines.
 *
 * @section Acknowledgements
 * The design of FSM is inspired by UML-2 State Machines, QP by Miro
 * Samek, and ObjecTime by Brian Selic.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_FSM_HH__
#define __COSA_FSM_HH__

#include "Cosa/Event.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

class FSM : public Link {
public:
  /**
   * State handler function prototype. Should return true(1) if
   * the event was handled otherwise false(0). This allows common
   * and hierarchical state handlers.
   * @param[in] fsm finite state machine.
   * @param[in] type the type of event.
   * @return bool.
   */
  typedef bool (*StateHandler)(FSM* fsm, uint8_t type);
  
private:
  static const uint16_t TIMEOUT_REQUEST = 0xffff;
  StateHandler m_state;
  uint16_t m_period;
  uint16_t m_param;

  /**
   * @override
   * The first level event handler. Filters timeout events and
   * adapt to state handler function prototype.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    if (m_period == TIMEOUT_REQUEST) cancel_timer();
    m_param = value;
    m_state(this, type);
  }
  
public:
  /**
   * Construct state machine with given initial state.
   * @param[in] init initial state handler.
   * @param[in] period timeout in all states (default no timeout).
   */
  FSM(StateHandler init, uint16_t period = 0) :
    Link(), 
    m_state(init),
    m_period(period),
    m_param(0)
  {}
  
  /**
   * Set new state handler for next event.
   * @param[in] fn state handler.
   */
  void set_state(StateHandler fn) 
  {
    if (fn == 0) return;
    m_state = fn;
  }
  
  /**
   * Set timeout period for all states.
   * @param[in] ms timeout.
   */
  void set_period(uint8_t ms)
  {
    m_period = ms;
  }
  
  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get(uint16_t& param)
  {
    param = m_param;
  }
  
  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get(void*& param)
  {
    param = (void*) m_param;
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
   * Start the state machine with an Event::BEGIN_TYPE.
   */
  bool begin()
  {
    if ((m_period != 0) && (m_period != TIMEOUT_REQUEST)) 
      Watchdog::attach(this, m_period);
    send(Event::BEGIN_TYPE);
    return (1);
  }
  
  /**
   * End the state machine with an Event::END_TYPE.
   */
  void end()
  {
    cancel_timer();
    send(Event::END_TYPE);
  }
  
  /**
   * Set timer for time out events and possible state transitions.
   * @param[in] ms timeout period.
   */
  void set_timer(uint16_t ms)
  {
    m_period = TIMEOUT_REQUEST;
    Watchdog::attach(this, ms);
  }

  /**
   * Cancel a timer request. This is performed by default on a
   * timeout event.
   */
  void cancel_timer()
  {
    if (m_period != 0) {
      detach();
      m_period = 0;
    }
  }
};

#endif

