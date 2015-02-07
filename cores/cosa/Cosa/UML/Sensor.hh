/**
 * @file Cosa/UML/Sensor.hh
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

#ifndef COSA_UML_SENSOR_HH
#define COSA_UML_SENSOR_HH

#include "Cosa/UML/TimedCapsule.hh"
#include "Cosa/UML/Connector.hh"
#include "Cosa/AnalogPin.hh"

namespace UML {

/**
 * Sensor Capsule class. Provides a signal connector that is set
 * according to the sensor (analog pin). The pin is periodically 
 * sampled and listeners are scheduled when the value changes. 
 * By default the sample is scaled to voltage.
 *
 * @section Diagram
 *
 *  +--------+
 *  | Sensor |---[Sample]--->
 *  +--------+
 *        [ms]
 */
class Sensor : public TimedCapsule, private AnalogPin {
public:
  /**
   * Default sample period for sensor (in ms).
   */
  static const uint16_t DEFAULT_TIMEOUT = 1024;

  /**
   * Type of sensor sample connector. Schedule listeners only on
   * change.
   */
  typedef Connector<float,true> Sample;

  /**
   * Construct Sensor monitoring given analog pin and generating
   * signal. The pin is sampled with the given period (default 1024 ms).
   * @param[in] pin analog pin for sensor.
   * @param[in] sample connector.
   * @param[in] ms period.
   */
  Sensor(Board::AnalogPin pin, Sample& sample, uint16_t ms = DEFAULT_TIMEOUT) : 
    TimedCapsule(ms), 
    AnalogPin(pin),
    m_sample(sample)
  {}

  /**
   * @override Capsule
   * Read digital pin and update signal on change.
   */
  virtual void behavior() 
  {
    m_sample = scale(sample());
  }

  /**
   * @override Sensor
   * Default sample scaling; range [0..1023] is scaled to [0.0..5.0]. 
   * @param[in] sample value.
   * @return scaled value.
   */
  virtual float scale(uint16_t value)
  {
    return (value * 5.0 / 1023);
  }

protected:
  Sample& m_sample;
};

};

#endif
