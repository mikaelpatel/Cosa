/**
 * @file Cosa/Button.h
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
 * Debounded Button abstraction. Sampled input pin using the
 * Watchdog timeout event(64 ms).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BUTTON_HH__
#define __COSA_BUTTON_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Thing.hh"

class Button : private Thing {

public:
  enum Mode {
    ON_FALLING_MODE = 0,
    ON_RISING_MODE = 1,
    ON_CHANGE_MODE = 2
  };
  
private:
  static const uint16_t SAMPLE_MS = 64;
  InputPin* _pin;
  uint8_t _state;
  Mode _mode;

public:
  /**
   * Construct a button with the given mode. Input pin to be 
   * attached.
   * @param[in] mode event mode.
   */
  Button(Mode mode = ON_CHANGE_MODE) : 
    Thing(), 
    _pin(0), 
    _state(0),
    _mode(mode)
  {}

  /**
   * Attach given input pin to the button. Pin will be sampled and 
   * on change receive events.
   * @param[in] pin input pin.
   */
  void attach(InputPin* pin);

private:
  /**
   * Button event handler. Called by watchdog on timeout. Samples the
   * attached pin and calls pin event handler on change.
   * @param[in] it the target object (button).
   * @param[in] type the type of event (timeout).
   * @param[in] value the event value.
   */
  static void on_timeout(Thing* it, uint8_t type, uint16_t value);
};

#endif
