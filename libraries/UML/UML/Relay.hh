/**
 * @file UML/Relay.hh
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

#ifndef COSA_UML_RELAY_HH
#define COSA_UML_RELAY_HH

#include "Capsule.hh"
#include "Cosa/OutputPin.hh"

namespace UML {

/**
 * The Relay Capsule. The Relay will set given output pin when
 * signal is less than min value, and clear when the signal is
 * greater than max value.
 * @param[in] T connector type.
 *
 * @section Diagram
 * @code
 *                       Relay
 *                     +--------+
 *                     | relay  |
 * ---[T value]------->|        |
 * ---[T min_value]--->|        |
 * ---[T max_value]--->|        |
 *                     +--------+
 *                           [Dn]
 * @endcode
 */
template<typename T>
class Relay : public Capsule, private OutputPin {
public:
  /**
   * Construct Relay Capsule for given digital pin. Set pin when
   * value is less than min value, clear when value is greater than
   * max value.
   * @param[in] value.
   * @param[in] min_value.
   * @param[in] max_value.
   * @param[in] pin.
   */
  Relay(T& value, T& min_value, T& max_value, Board::DigitalPin pin) :
    Capsule(),
    OutputPin(pin, 0),
    m_value(value),
    m_min_value(min_value),
    m_max_value(max_value)
  {}

  /**
   * @override UML::Capsule
   * Turn on relay when current value is less than min value,
   * and turn off when greater than max value.
   */
  virtual void behavior()
  {
    if (is_on()) {
      if (m_value > m_max_value)
	off();
    }
    else {
      if (m_value < m_min_value)
	on();
    }
  }

protected:
  T& m_value;			//!< Relay control input value connector.
  T& m_min_value;		//!< Relay control min input value connector.
  T& m_max_value;		//!< Relay control max input value connector.
};

};
#endif
