/**
 * @file Cosa/LED.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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

#ifndef COSA_LED_HH
#define COSA_LED_HH

#include "Cosa/Types.h"
#include "Cosa/OutputPin.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

/**
 * Blinking LED; Use built-in LED or other digital pin for pulse.
 * Support simple application status indicator.
 */
class LED : private Link {
public:
  /**
   * Construct LED connected to the given pin.
   * @param[in] pin symbol (default built-in LED).
   */
  LED(Board::DigitalPin pin = Board::LED) :
    Link(),
    m_pin(pin)
  {
  }

  /**
   * Turn LED on.
   */
  void on() __attribute__((always_inline))
  {
    detach();
    m_pin.on();
  }

  /**
   * Turn LED off.
   */
  void off() __attribute__((always_inline))
  {
    detach();
    m_pin.off();
  }

  /**
   * Blink LED with normal period.
   */
  void normal_mode() __attribute__((always_inline))
  {
    Watchdog::attach(this, 512);
  }

  /**
   * Blink LED with alert period.
   */
  void alert_mode() __attribute__((always_inline))
  {
    Watchdog::attach(this, 128);
  }

private:
  /** Output pin for LED. */
  OutputPin m_pin;

  /**
   * @override Event::Handler
   * LED event handler; Toggle LED on timeout event.
   * @param[in] type the type of event (timeout).
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  { 
    UNUSED(type);
    UNUSED(value);
    m_pin.toggle(); 
  }
};

#endif
