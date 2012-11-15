/**
 * @file Cosa/Event.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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

#include "Types.h"
#include "Queue.h"

class Event {

public:
  static const uint8_t QUEUE_MAX = 16;

  /**
   * Event types are added here for new event sources. Typical mapping
   * from interrupts to events.
   */
  enum Type {
    NULL_TYPE = 0,
    INTERRUPT_PIN_CHANGE_TYPE = 1,
    ANALOG_PIN_SAMPLE_TYPE = 2,
    ANALOG_PIN_SET_SAMPLE_TYPE = 3,
    WATCHDOG_TIMEOUT_TYPE = 4,
    SPI_RECEIVE_DATA_TYPE = 5,
    NRF_RECEIVE_DATA_TYPE = 6,
    ADXL_CHANGE_TYPE = 7
  };

private:
  Type _type;
  void* _source;
  uint16_t _value;

public:
  /**
   * Construct event with given type, source and value.
   * @param[in] type event identity.
   * @param[in] source event generator.
   * @param[in] value event value.
   */
  Event(Type type = NULL_TYPE, void* source = 0, uint16_t value = 0) :
    _type(type),
    _source(source),
    _value(value)
  {}

  /**
   * Return event type.
   * @return type.
   */
  Type get_type() 
  { 
    return (_type); 
  }

  /**
   * Return event source.
   * @return pointer.
   */
  void* get_source() 
  { 
    return (_source); 
  } 

  /**
   * Return event value.
   * @return value.
   */
  uint16_t get_value() { return (_value); }

  /**
   * Push an event with given type, source and value into the event queue, .
   * (eventq). Return true(1) if successful otherwise false(0).
   * @param[in] type event identity.
   * @param[in] source event generator.
   * @param[in] value event value.
   * @return boolean, true(1) if successful otherwise false(0).
   */
  static uint8_t push(Type type, void* source = 0, uint16_t value = 0);

  /**
   * In debug mode, print event to serial stream.
   */
  void print();

  /**
   * In debug mode, print event to serial stream with new-line.
   */
  void println();

  /**
   * Event queue of size QUEUE_MAX.
   */
  static Queue queue;
};

#endif

