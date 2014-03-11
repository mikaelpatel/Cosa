/**
 * @file Cosa/Button.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#ifndef __COSA_BUTTON_HH__
#define __COSA_BUTTON_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

/**
 * Debounded Button; Sampled input pin (with internal pullup
 * resistor). Uses a watchdog timeout event (64 ms) for sampling and
 * on change calls an event action. Subclass Button and implement the
 * virtual on_change() method. Use the subclass for any state needed
 * for the action function. Connect button/switch from pin to
 * ground. Internal pull-up resistor is activated. 
 *
 * @section Limitations
 * Button toggle faster than sample period may be missed. This is the
 * case when connecting to a Rotary Encoder.
 * 
 * @section See Also
 * The Button event handler requires the usage of an event dispatch. 
 * See Event.hh. 
 */
class Button : public InputPin, private Link {
public:
  /**
   * Button change detection modes; falling (high to low), rising (low
   * to high) and change (falling or rising). 
   */
  enum Mode {
    ON_FALLING_MODE = 0,
    ON_RISING_MODE = 1,
    ON_CHANGE_MODE = 2
  } __attribute__((packed));

  /**
   * Construct a button connected to the given pin and with 
   * the given change detection mode. 
   * @param[in] pin number.
   * @param[in] mode change detection mode.
   */
  Button(Board::DigitalPin pin, Mode mode = ON_CHANGE_MODE) :
    InputPin(pin, InputPin::PULLUP_MODE),
    Link(),
    MODE(mode),
    m_state(is_set())
  {
  }

  /**
   * Start the button handler.
   */
  void begin()
  {
    Watchdog::attach(this, SAMPLE_MS);
  }

  /**
   * Stop the button handler.
   */
  void end()
  {
    detach();
  }

  /**
   * @override Button
   * The button change event handler. Called when a change
   * corresponding to the mode has been detected. Event types are;
   * Event::FALLING_TYPE, Event::RISING_TYPE, and Event::CHANGE_TYPE. 
   * Sub-class must override this method.
   * @param[in] type event type.
   */
  virtual void on_change(uint8_t type) = 0;

protected:
  /**
   * Sample period, current state and change detection mode.
   */
  static const uint16_t SAMPLE_MS = 64;
  const Mode MODE;
  uint8_t m_state;

  /**
   * @override Event::Handler
   * Button event handler. Called by event dispatch. Samples the
   * attached pin and calls the pin change handler, on_change(). 
   * @param[in] type the type of event (timeout).
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

#endif
