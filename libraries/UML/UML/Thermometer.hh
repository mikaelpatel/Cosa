/**
 * @file UML/Thermometer.hh
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

#include "TimedCapsule.hh"
#include "Connector.hh"

namespace UML {

/**
 * Thermometer Capsule class. Provides a signal connector that is set
 * according to temperature reading. The thermometer is periodically
 * sampled and listeners are scheduled when the temperature changes.
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
 *
 * @section Requires
 * DS18B20.h
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
   * Construct Thermometer monitoring given OWI bus and temperature
   * connnector. The temperature is sampled with the given period
   * (default 2048 ms).
   * @param[in] pin 1-wire bus.
   * @param[in] temp connector.
   * @param[in] ms period.
   */
  Thermometer(OWI* pin, Temperature& temp, uint16_t ms = DEFAULT_TIMEOUT) :
    TimedCapsule(ms / 2),
    DS18B20(pin),
    m_temp(temp),
    m_state(CONVERT)
  {}

  /**
   * @override UML::Capsule
   * Request temperature conversion and read value.
   */
  virtual void behavior()
  {
    switch (m_state) {
    case CONVERT:
      // Request a temperature conversion
      m_state = convert_request() ? READ : ERROR;
      break;
    case READ:
      // Read temperature and scale to a float
      m_state = read_scratchpad() ? CONVERT : ERROR;
      m_temp = get_temperature() * 0.0625;
      break;
    case ERROR:
      // Error state; set error value and retry
      m_temp = 85.0;
      m_state = CONVERT;
      break;
    }
  }

protected:
  Temperature& m_temp;		//!< Temperature reading.
  enum {
    CONVERT,			//!< Convert request.
    READ,			//!< Read temperature.
    ERROR			//!< Error state.
  } m_state;		    	//!< Current state.
};

};

#endif
