/**
 * @file UML/TimedCapsule.hh
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

#ifndef COSA_UML_TIMED_CAPSULE_HH
#define COSA_UML_TIMED_CAPSULE_HH

#include "Cosa/Periodic.hh"
#include "Capsule.hh"
#include "Controller.hh"

namespace UML {

/**
 * Timed Capsule class. Run the behavior periodically.
 *
 * @section Diagram
 * @code
 *
 *   TimedCapsule
 *   +-----------+
 *   |  capsule  |
 *   |           |---[Connector]--->
 *   |           |
 *   +-----------+
 *            [ms]
 * @endcode
 */
class TimedCapsule : public Capsule, public Periodic {
public:
  /**
   * Construct Timed Capsule with given period in milli-seconds.
   * @param[in] ms period.
   */
  TimedCapsule(uint16_t ms) :
    Capsule(),
    Periodic(ms)
  {}

protected:
  /**
   * @override Periodic
   * Schedule this capsule on timeout. Could also call behaviour
   * directly.
   */
  virtual void run()
  {
    controller.schedule(this);
  }
};

};
#endif
