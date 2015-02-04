/**
 * @file UML/LED.hh
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

#ifndef COSA_UML_LED_HH
#define COSA_UML_LED_HH

#include "UML/Capsule.hh"
#include "UML/Button.hh"
#include "Cosa/OutputPin.hh"

namespace UML {

/**
 * The LED Capsule. The behavior will toggle the LED state.
 */
class LED : public Capsule, private OutputPin {
public:
  /**
   * Construct LED Capsule for given digital pin.
   * @param[in] ledpin.
   */
  LED(Button::Signal& signal, Board::DigitalPin ledpin = Board::LED) : 
    Capsule(), 
    OutputPin(ledpin, 0),
    m_signal(signal)
  {}

  /**
   * @override Capsule
   * Blink the LED if the signal is true.
   */
  virtual void behavior()
  {
    if (m_signal) {
      toggle();
    }
    else {
      clear();
    }
  }

protected:
  Button::Signal& m_signal;
};

};
#endif
