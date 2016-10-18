/**
 * @file Cosa/LED.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2016, Mikael Patel
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
#include "Cosa/Periodic.hh"

/**
 * Blinking LED; Use built-in LED or other digital pin for pulse.
 * Support simple application status indicator.
 */
class LED : private Periodic {
public:
  /**
   * Construct LED connected to the given pin.
   * @param[in] pin symbol (default built-in LED).
   */
  LED(Job::Scheduler* scheduler, Board::DigitalPin pin = Board::LED) :
    Periodic(scheduler, 512),
    m_pin(pin)
  {}

  /**
   * Turn LED on.
   */
  void on()
    __attribute__((always_inline))
  {
    stop();
    m_pin.on();
  }

  /**
   * Turn LED off.
   */
  void off()
    __attribute__((always_inline))
  {
    stop();
    m_pin.off();
  }

  /**
   * Blink LED with normal period.
   */
  void normal_mode()
    __attribute__((always_inline))
  {
    stop();
    period(512);
    start();
  }

  /**
   * Blink LED with alert period.
   */
  void alert_mode()
    __attribute__((always_inline))
  {
    stop();
    period(128);
    start();
  }

private:
  /** Output pin for LED. */
  OutputPin m_pin;

  /**
   * @override{Job}
   * The LED run virtual member function; Toggle LED on timeout event.
   */
  virtual void run()
  {
    m_pin.toggle();
  }
};

#endif
