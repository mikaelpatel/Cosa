/**
 * @file Cosa/FSM.hh
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

#ifndef COSA_FSM_HH
#define COSA_FSM_HH

#include "Cosa/Job.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

/**
 * Finite State Machine support class. States are represented as an
 * Event handler. Supports timeout events and period timed state
 * machines.
 *
 * @section Acknowledgements
 * The design of FSM is inspired by UML-2 State Machines, QP by Miro
 * Samek, and ObjecTime by Brian Selic.
 */
class FSM : protected Job {
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

  /**
   * Construct state machine with given initial state.
   * @param[in] init initial state handler.
   * @param[in] scheduler for timeout handling (default no timout handling).
   * @param[in] period timeout in all states (default no timeout).
   */
  FSM(StateHandler init, Job::Scheduler* scheduler = NULL, uint16_t period = 0) :
    Job(scheduler),
    m_state(init),
    m_period(period),
    m_param(0)
  {}

  /**
   * Set new state handler for next event.
   * @param[in] fn state handler.
   * @pre fn != NULL
   */
  void state(StateHandler fn)
    __attribute__((always_inline))
  {
    if (UNLIKELY(fn == NULL)) return;
    m_state = fn;
  }

  /**
   * Get state handler for next event.
   */
  StateHandler state() const
    __attribute__((always_inline))
  {
    return (m_state);
  }

  /**
   * Set timeout period for all states.
   * @param[in] ms timeout.
   */
  void period(uint8_t ms)
  {
    m_period = ms;
  }

  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get(uint16_t& param) const
  {
    param = m_param;
  }

  /**
   * Get event parameter.
   * @param[out] param event parameter
   */
  void get(void*& param) const
  {
    param = (void*) m_param;
  }

  /**
   * Send an event to the state machine.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  void send(uint8_t type, uint16_t value = 0)
    __attribute__((always_inline))
  {
    Event::push(type, this, value);
  }

  /**
   * Send an event to the state machine.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  void send(uint8_t type, void* value)
    __attribute__((always_inline))
  {
    Event::push(type, this, value);
  }

  /**
   * Start the state machine with an Event::BEGIN_TYPE.
   */
  bool begin()
  {
    if ((m_period != 0) && (m_period != TIMEOUT_REQUEST)) {
      expire_after(m_period);
      start();
    }
    send(Event::BEGIN_TYPE);
    return (true);
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
    __attribute__((always_inline))
  {
    m_period = TIMEOUT_REQUEST;
    expire_after(ms);
    start();
  }

  /**
   * Cancel a timer request. This is performed by default on a
   * timeout event.
   */
  void cancel_timer()
    __attribute__((always_inline))
  {
    if (UNLIKELY(m_period == 0)) return;
    stop();
    m_period = 0;
  }

private:
  static const uint16_t TIMEOUT_REQUEST = 0xffff;
  StateHandler m_state;
  uint16_t m_period;
  uint16_t m_param;

  /**
   * @override{Event::Handler}
   * The first level event handler. Filters timeout events and
   * adapt to state handler function prototype.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    m_param = value;
    m_state(this, type);
    if (type == Event::TIMEOUT_TYPE) {
      if ((m_period != 0) && (m_period != TIMEOUT_REQUEST)) {
	expire_after(m_period);
	start();
      }
    }
  }
};

#endif

