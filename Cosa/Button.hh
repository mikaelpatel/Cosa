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
 * Debounded Button abstraction. Sampled input pin using the
 * Watchdog timeout event(64 ms).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BUTTON_HH__
#define __COSA_BUTTON_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"

class Button : public InputPin {

public:
  enum Mode {
    ON_FALLING_MODE = 0,
    ON_RISING_MODE = 1,
    ON_CHANGE_MODE = 2
  };

private:
  static const uint16_t SAMPLE_MS = 64;
  uint8_t m_state;
  uint8_t m_mode;

public:
  /**
   * Construct a button with the given mode. 
   * @param[in] pin number.
   * @param[in] mode change event mode.
   */
  Button(uint8_t pin, Mode mode = ON_CHANGE_MODE);

  /**
   * Button event handler. Called by watchdog on timeout. Samples the
   * attached pin and calls pin event handler on change.
   * @param[in] it the target object (button).
   * @param[in] type the type of event (timeout).
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);

  /**
   * The event handler; button 
   * @param[in] type event type.
   */
  virtual void on_change(uint8_t type) {}
};

#endif
