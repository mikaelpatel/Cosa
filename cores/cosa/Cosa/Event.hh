/**
 * @file Cosa/Event.hh
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

#ifndef COSA_EVENT_HH
#define COSA_EVENT_HH

#include "Cosa/Types.h"
#include "Cosa/Queue.hh"

// Default event queue size
#ifndef COSA_EVENT_QUEUE_MAX
# if defined(BOARD_ATTINY)
#   define COSA_EVENT_QUEUE_MAX 8
# else
#   define COSA_EVENT_QUEUE_MAX 16
# endif
#endif

/**
 * Event data structure with type, source and value.
 */
class Event {
public:
  /**
   * Size of event queue. Adjust depending on application. Must be
   * Power(2). Class Queue will statically check this.
   */
  static const uint8_t QUEUE_MAX = COSA_EVENT_QUEUE_MAX;

  /**
   * Event types are added here. Typical mapping from interrupts to
   * events. Note that the event is not a global numbering
   * scheme. Instead depends on the receiving/sending party, the
   * protocol.
   */
  enum {
    NULL_TYPE = 0,

    FALLING_TYPE,		// Digital Pins
    RISING_TYPE,
    CHANGE_TYPE,

    SAMPLE_REQUEST_TYPE,	// Analog Pins
    SAMPLE_COMPLETED_TYPE,

    WATCHDOG_TYPE,		// Watchdog and timers
    TIMEOUT_TYPE,

    BEGIN_TYPE,			// Finite State Machines
    END_TYPE,

    RUN_TYPE,			// Thread

    CONNECT_TYPE,		// Device drivers and protocol stacks
    DISCONNECT_TYPE,
    RECEIVE_REQUEST_TYPE,
    RECEIVE_COMPLETED_TYPE,
    SEND_REQUEST_TYPE,
    SEND_COMPLETED_TYPE,

    OPEN_TYPE,			// Device drivers and storage
    CLOSE_TYPE,
    READ_REQUEST_TYPE,
    READ_COMPLETED_TYPE,
    WRITE_REQUEST_TYPE,
    WRITE_COMPLETED_TYPE,
    COMMAND_REQUEST_TYPE,
    COMMAND_COMPLETED_TYPE,

    SERVICE_REQUEST_TYPE,	// Servers
    SERVICE_RESPONSE_TYPE,

    USER_TYPE = 64,		// User defined events/messages, 64-254

    ERROR_TYPE = 255		// Error event
  } __attribute__((packed));

  /**
   * Event handler root class.
   */
  class Handler {
  public:
    /**
     * @override{Event::Handler}
     * Default null event handler. Should be redefined by sub-classes.
     * Called by Event::dispatch().
     * @param[in] type the event type.
     * @param[in] value the event value.
     */
    virtual void on_event(uint8_t type, uint16_t value)
    {
      UNUSED(type);
      UNUSED(value);
    }
  };

public:
  /**
   * Construct event with given type, target and value.
   * @param[in] type event identity (default NULL_TYPE(0)).
   * @param[in] target event receiver (default NULL(0)).
   * @param[in] value event value (default zero(0)).
   */
  Event(int8_t type = NULL_TYPE, Handler* target = NULL, uint16_t value = 0) :
    m_type(type),
    m_target(target),
    m_value(value)
  {}

  /**
   * Return event type.
   * @return type.
   */
  uint8_t type() const
  {
    return (m_type);
  }

  /**
   * Return event target.
   * @return pointer.
   */
  Handler* target() const
  {
    return (m_target);
  }

  /**
   * Return event value.
   * @return value.
   */
  uint16_t value() const
  {
    return (m_value);
  }

  /**
   * Return event environment pointer.
   * @return pointer.
   */
  void* env() const
  {
    return ((void*) m_value);
  }

  /**
   * Dispatch event handler for target object.
   */
  void dispatch()
    __attribute__((always_inline))
  {
    if (m_target != NULL) m_target->on_event(m_type, m_value);
  }

  /**
   * Push an event with given type, source and value into the event queue.
   * Return true(1) if successful otherwise false(0).
   * @param[in] type event identity.
   * @param[in] target event target.
   * @param[in] value event value.
   * @return bool.
   */
  static bool push(uint8_t type, Handler* target, uint16_t value = 0)
    __attribute__((always_inline))
  {
    Event event(type, target, value);
    return (queue.enqueue(&event));
  }

  /**
   * Push an event with given type, source and value into the event queue.
   * Return true(1) if successful otherwise false(0).
   * @param[in] type event identity.
   * @param[in] target event target.
   * @param[in] env event environment pointer.
   * @return bool.
   */
  static bool push(uint8_t type, Handler* target, void* env)
    __attribute__((always_inline))
  {
    return (push(type, target, (uint16_t) env));
  }

  /**
   * Event queue of size QUEUE_MAX.
   */
  static Queue<Event, QUEUE_MAX> queue;

  /**
   * Service events and wait at most given number of milliseconds. The
   * value zero(0) indicates that call should block until an event.
   * @param[in] ms maximum wait time (Default blocking).
   * @return true(1) if an event was dispatched otherwise false(0).
   */
  static bool service(uint32_t ms = 0L);

private:
  uint8_t m_type;		//!< Event type.
  Handler* m_target;		//!< Event target object (receiver).
  uint16_t m_value;		//!< Event parameter and/or value.
};

#endif

