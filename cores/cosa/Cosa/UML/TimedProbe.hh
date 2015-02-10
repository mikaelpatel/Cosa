/**
 * @file Cosa/UML/TimedProbe.hh
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

#ifndef COSA_UML_TIMED_PROBE_HH
#define COSA_UML_TIMED_PROBE_HH

#include "Cosa/UML/TimedCapsule.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"

namespace UML {

/**
 * Abstract Connector Probe for given Connector type.
 * @param[in] T connector type to probe.
 *
 * @section Diagram
 * @code
 *
 *           TimedProbe<T>
 *           +-----------+
 *           |    p1     |
 * ---[T]--->|           |
 *           |           |
 *           +-----------+
 *                    [ms]
 * @endcode
 */
template<typename T>
class TimedProbe : public TimedCapsule {
public:
  /**
   * Construct Timed Probe for given connector and timeout period.
   * @param[in] name string in program memory.
   * @param[in] connector to probe.
   * @param[in] ms timeout period in ms (Default 2048).
   */
  TimedProbe(str_P name, T& connector, uint16_t ms = 2048) :
    TimedCapsule(ms),
    m_name(name),
    m_connector(connector)
  {}

  /**
   * @override UML::Capsule
   * Print timestamp, probe name and connector value to trace output stream.
   */
  virtual void behavior()
  {
    trace << Watchdog::millis() << ':' << m_name << '=' << m_connector << endl;
  }

protected:
  str_P m_name;
  T& m_connector;
};

};
#endif
