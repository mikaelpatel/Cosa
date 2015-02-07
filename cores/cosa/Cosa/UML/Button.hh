/**
 * @file Cosa/UML/Button.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_UML_BUTTON_HH
#define COSA_UML_BUTTON_HH

#include "Cosa/UML/TimedCapsule.hh"
#include "Cosa/UML/Connector.hh"
#include "Cosa/InputPin.hh"

namespace UML {

/**
 * Button Capsule class. Provides a boolean signal connector that is
 * set according to the button (digital pin). The pin is periodically
 * sampled and listeners are scheduled when the state changes. 
 *
 * @section Diagram
 *
 *    Button
 *  +--------+
 *  |   bn   |
 *  |        |---[Signal]--->
 *  |        |
 *  +--------+
 *        [Dn]
 */
class Button : public TimedCapsule, private InputPin {
public:
  /**
   * Default sample period for debounce of button (in ms).
   */
  static const uint16_t DEFAULT_TIMEOUT = 64;

  /**
   * Construct Button monitoring given digital pin and generating
   * signal. The pin is sampled with the given period (default 64
   * ms). 
   * @param[in] pin digital pin for button.
   * @param[in] signal connector.
   * @param[in] ms period.
   */
  Button(Board::DigitalPin pin, Signal& signal, uint16_t ms = DEFAULT_TIMEOUT) : 
    TimedCapsule(ms), 
    InputPin(pin, InputPin::PULLUP_MODE),
    m_signal(signal)
  {}

  /**
   * @override Capsule
   * Read digital pin and update signal on change.
   */
  virtual void behavior() 
  {
    m_signal = read();
  }

protected:
  Signal& m_signal;
};

};

#endif
