/**
 * @file Cosa/UML/Thermometer.hh
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

#ifndef COSA_UML_THERMOMETER_HH
#define COSA_UML_THERMOMETER_HH

#include "Cosa/UML/TimedCapsule.hh"
#include "Cosa/UML/Connector.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"

namespace UML {

/**
 * Thermometer Capsule class. Provides a signal connector that is set
 * according to temperature reading. The thermometer is periodically
 * sampled and listeners are scheduled when the value changes.
 *
 * @section Diagram
 * @code
 *
 *  Thermometer
 *  +--------+
 *  |  temp  |
 *  |        |---[Temperature]--->
 *  |        |
 *  +--------+
 *    [OWI/ms]
 *
 * @endcode
 */
class Thermometer : public TimedCapsule, public DS18B20 {
public:
  /**
   * Default sample period for sensor (in ms).
   */
  static const uint16_t DEFAULT_TIMEOUT = 2048;

  /**
   * Type of temperature connector. Schedule listeners only on
   * change.
   */
  typedef Connector<float,true> Temperature;

  /**
   * Construct Thermometer monitoring given OWI bus and generating
   * signal. The pin is sampled with the given period (default 1024 ms).
   * @param[in] pin 1-wire bus.
   * @param[in] sample connector.
   * @param[in] ms period.
   */
  Thermometer(OWI* pin, Temperature& temp, uint16_t ms = DEFAULT_TIMEOUT) :
    TimedCapsule(ms / 2),
    DS18B20(pin),
    m_temp(temp),
    m_state(0)
  {}

  /**
   * @override UML::Capsule
   * Read digital pin and update signal on change.
   */
  virtual void behavior()
  {
    switch (m_state) {
    case 0:
      m_state = convert_request() ? 1 : 2;
      break;
    case 1:
      m_state = read_scratchpad() ? 0 : 2;
      m_temp = get_temperature() * 0.0625;
      break;
    case 2:
      break;
    }
  }

protected:
  Temperature& m_temp;
  uint8_t m_state;
};

};

#endif
