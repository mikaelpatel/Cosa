/**
 * @file UML/Clock.hh
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

#include "UML/Capsule.hh"
#include "UML/Connector.hh"
#include "UML/Controller.hh"
#include "Cosa/Periodic.hh"

namespace UML {

/**
 * Periodic Capsule class. The clock tick value is defined as a
 * Connector type. The behavior of this class is simple incrementing
 * the tick value. The periodic run function will map fromt the event
 * callback to the capsule scheduler (controller).
 */
class Clock : public Capsule, public Periodic {
public:
  /**
   * Type of clock tick connector.
   */
  typedef Connector<uint16_t> Tick;

  /**
   * Construct Clock with given tick connector and period in
   * milli-seconds. 
   * @param[in] tick connector.
   * @param[in] ms period.
   */
  Clock(Tick& tick, uint16_t ms) : 
    Capsule(), 
    Periodic(ms), 
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

  /**
   * @override Periodic
   * Schedule this capsule on timeout. Could also call behaviour
   * directly.
   */
  virtual void run()
  {
    controller.schedule(this);
  }

protected:
  Tick& m_tick;
};

};
#endif
