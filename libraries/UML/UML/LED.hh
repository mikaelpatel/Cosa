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

#include "Cosa/OutputPin.hh"
#include "Capsule.hh"
#include "Connector.hh"

namespace UML {

/**
 * The LED Capsule. The behavior will toggle the LED state.
 *
 * @section Diagram
 * @code
 *                   LED
 *                +-------+
 *                |  led  |
 * ---[Signal]--->|       |
 *                |       |
 *                +-------+
 *                     [Dn]
 * @endcode
 */
class LED : public Capsule, private OutputPin {
public:
  /**
   * Construct LED Capsule for given digital pin.
   * @param[in] pin.
   */
  LED(Signal& signal, Board::DigitalPin pin = Board::LED) :
    Capsule(),
    OutputPin(pin, 0),
    m_signal(signal)
  {}

  /**
   * @override UML::Capsule
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
  Signal& m_signal;		//!< LED input signal connector.
};

};
#endif
