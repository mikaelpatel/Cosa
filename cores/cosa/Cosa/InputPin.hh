/**
 * @file Cosa/InputPin.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#ifndef COSA_INPUT_PIN_HH
#define COSA_INPUT_PIN_HH

#include "Cosa/Pin.hh"

/**
 * Abstract input pin. Allows pullup mode.
 */
class InputPin : public Pin {
public:
  enum Mode {
    NORMAL_MODE = 0,
    PULLUP_MODE = 1
  } __attribute__((packed));

  /**
   * Construct abstract input pin given Arduino pin number.
   * @param[in] pin number (digital pin).
   * @param[in] mode pin mode (default NORMAL_MODE).
   */
  InputPin(Board::DigitalPin pin, Mode mode = NORMAL_MODE) :
    Pin((uint8_t) pin)
  {
    if (mode == PULLUP_MODE) {
      synchronized {
	*PORT() |= m_mask; 
      }
    }
  }

  /**
   * Set input pin to given mode.
   * @param[in] pin number.
   * @param[in] mode pin mode (default NORMAL_MODE).
   */
  static void set_mode(uint8_t pin, Mode mode = NORMAL_MODE) 
    __attribute__((always_inline))
  {
    if (mode == PULLUP_MODE) {
      synchronized { 
	*PORT(pin) |= MASK(pin); 
      }
    }
  }
};

#endif
