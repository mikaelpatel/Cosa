/**
 * @file Cosa/Event.h
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
 * Event data structure with type, source and value.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_EVENT_H__
#define __COSA_EVENT_H__

#include "Cosa/Types.h"
#include "Cosa/Queue.h"
#include "Cosa/IOStream.h"
#include "Cosa/Trace.h"
#include "Cosa/Thing.h"

class Event {

public:
  static const uint8_t QUEUE_MAX = 16;

  /**
   * Event types are added here for new event sources. Typical mapping
   * from interrupts to events.
   */
  enum {
    NULL_TYPE = 0,
    FALLING_TYPE = 1,
    RISING_TYPE = 2,
    CHANGE_TYPE = 3,
    TIMEOUT_TYPE = 4,
    CONNECT_TYPE = 5,
    DISCONNECT_TYPE = 6,
    RECEIVE_COMPLETED_TYPE = 7,
    SEND_COMPLETED_TYPE = 8,
    READ_COMPLETED_TYPE = 9,
    WRITE_COMPLETED_TYPE = 10,
    WATCHDOG_TYPE = 11,
    THING_TYPE = 63,
    ERROR_TYPE = 255
  };

private:
  uint8_t _type;
  Thing* _target;
  uint16_t _value;

public:
  /**
   * Construct event with given type, source and value.
   * @param[in] type event identity.
   * @param[in] target event receiver.
   * @param[in] value event value.
   * @param[in] env event environment.
   */
  Event(int8_t type = NULL_TYPE, Thing* target = 0, uint16_t value = 0) :
    _type(type),
    _target(target),
    _value(value)
  {}

  /**
   * Return event type.
   * @return type.
   */
  int get_type() 
  { 
    return (_type); 
  }

  /**
   * Return event target.
   * @return pointer.
   */
  Thing* get_target() 
  { 
    return (_target); 
  } 

  /**
   * Return event value.
   * @return value.
   */
  uint16_t get_value() 
  { 
    return (_value); 
  }

  /**
   * Return event environment pointer.
   * @return pointer
   */
  void* get_env() 
  { 
    return ((void*) _value); 
  }

  /**
   * Dispatch event handler for target object.
   */
  bool dispatch()
  {
    if (_target != 0) 
      _target->on_event(_type, _value);
  }

  /**
   * Push an event with given type, source and value into the event queue, .
   * (eventq). Return true(1) if successful otherwise false(0).
   * @param[in] type event identity.
   * @param[in] target event target.
   * @param[in] value event value.
   * @return boolean, true(1) if successful otherwise false(0).
   */
  static bool push(uint8_t type, Thing* target, uint16_t value = 0)
  {
    Event event(type, target, value);
    return (queue.enqueue(&event));
  }

  /**
   * Push an event with given type, source and value into the event queue, .
   * (eventq). Return true(1) if successful otherwise false(0).
   * @param[in] type event identity.
   * @param[in] target event target.
   * @param[in] value event value.
   * @return boolean, true(1) if successful otherwise false(0).
   */
  static bool push(uint8_t type, Thing* target, void* value)
  {
    push(type, target, (uint16_t) value);
  }

  /**
   * In debug mode, print event to given stream. Default is the
   * trace stream.
   */
  void print(IOStream& stream = trace);

  /**
   * In debug mode, print event to given stream with new-line.
   * Default is the trace stream.
   */
  void println(IOStream& stream = trace);

  /**
   * Event queue of size QUEUE_MAX.
   */
  static Queue queue;
};

#endif

