/**
 * @file Cosa/UML/Clock.hh
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

#ifndef COSA_UML_CLOCK_HH
#define COSA_UML_CLOCK_HH

#include "Cosa/UML/TimedCapsule.hh"
#include "Cosa/UML/Connector.hh"
#include "Cosa/Time.hh"

namespace UML {

/**
 * Clock Capsule class. The clock signal is defined as a Connector
 * type; Tick. The behavior of this class is simple incrementing the
 * tick value and thereby scheduling the capsules that are listening
 * to the tick.
 *
 * @section Diagram
 * 
 *     Clock
 *   +--------+
 *   |   c1   |
 *   |        |---[Tick]--->
 *   |        |
 *   +--------+
 *         [ms]
 */
class Clock : public TimedCapsule {
public:
  /**
   * Type of clock tick connector.
   */
  typedef Connector<clock_t> Tick;

  /**
   * Construct Clock with given tick connector and period in
   * milli-seconds. 
   * @param[in] tick connector.
   * @param[in] ms period.
   */
  Clock(Tick& tick, uint16_t ms) : 
    TimedCapsule(ms), 
    m_tick(tick) 
  {}

  /**
   * @override Capsule
   * Increment clock tick and schedule all capsules that listen for
   * clock update.
   */
  virtual void behavior() 
  {
    m_tick = m_tick + 1;
  }

protected:
  Tick& m_tick;
};

};
#endif
