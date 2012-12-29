/**
 * @file Cosa/Button.hh
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
 * Debounded Button; Sampled input pin with pullup resistor. Uses a
 * watchdog timeout event (64 ms) for sampling and on change calls an
 * event action. Subclass Button and implement the virtual on_change()
 * method. Use the subclass for any state needed for the action
 * function.  
 *
 * @section Circuit
 * Connect button/switch from pin to ground. 
 *
 * @section Limitations
 * Button toggle faster than sample period may be missed.
 * 
 * @section See Also
 * The Button event handler requires the usage of an event
 * dispatch. See Event.hh. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BUTTON_HH__
#define __COSA_BUTTON_HH__

#include "Cosa/Types.h"
#include "Cosa/Thing.hh"
#include "Cosa/Pins.hh"

class Button : private Thing {

public:
  /**
   * Button change detection modes; falling (set->clear), 
   * rising (clear-> set) and change (falling or rising).
   */
  enum Mode {
    ON_FALLING_MODE = 0,
    ON_RISING_MODE = 1,
    ON_CHANGE_MODE = 2
  };

protected:
  // Sample period, current state and change detection mode
  static const uint16_t SAMPLE_MS = 64;
  InputPin m_pin;
  uint8_t m_state;
  uint8_t m_mode;

  /**
   * @override
   * Button event handler. Called by event dispatch. Samples the
   * attached pin and calls pin change handler, on_change(). 
   * @param[in] type the type of event (timeout).
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

public:
  /**
   * Construct a button connected to the given pin and with 
   * the given change detection mode. 
   * @param[in] pin number.
   * @param[in] mode change detection mode.
   */
  Button(uint8_t pin, Mode mode = ON_CHANGE_MODE);

  /**
   * The button change event handler. Called when a change
   * corresponding to the mode has been detected. Event types are;
   * Event::FALLING_TYPE, Event::RISING_TYPE, and Event::CHANGE_TYPE. 
   * @param[in] type event type.
   */
  virtual void on_change(uint8_t type);
};

#endif
