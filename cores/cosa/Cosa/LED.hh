/**
 * @file Cosa/LED.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_LED_HH__
#define __COSA_LED_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
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
  void on()
  {
    detach();
    m_pin.on();
  }

  /**
   * Turn LED off.
   */
  void off()
  {
    detach();
    m_pin.off();
  }

  /**
   * Blink LED with normal period.
   */
  void normal_mode()
  {
    Watchdog::attach(this, 512);
  }

  /**
   * Blink LED with alert period.
   */
  void alert_mode()
  {
    Watchdog::attach(this, 128);
  }

private:
  /** Output pin for LED */
  OutputPin m_pin;

  /**
   * @override Event::Handler
   * LED event handler; Toggle LED on timeout event.
   * @param[in] type the type of event (timeout).
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  { 
    m_pin.toggle(); 
  }
};

#endif
