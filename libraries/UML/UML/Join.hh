/**
 * @file UML/Join.hh
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

#ifndef COSA_UML_JOIN_HH
#define COSA_UML_JOIN_HH

#include "Controller.hh"

namespace UML {

/**
 * Force a set of capsules to be completed before triggering
 * dependencies. Each capsule should signal on the join. The
 * capsules in the depencency set are scheduled when the
 * required number of signals has been performed. A join does
 * not have an associated value. It is a pure control flow
 * mechanism.
 *
 * @section Diagram
 * @code
 *
 *     Capsule                        Capsule
 *   +---------+                    +---------+
 *   |   c1    |-+                  |   c2    |-+
 *   |         | |                  |         | |
 *   |      [Port]---[Join(N)]--->[Port]      | |
 *   |         | |                  |         | |
 *   +---------+ |                  +---------+ |
 *     +---------+                    +---------+
 *             (N)                            (M)
 *
 * @endcode
 */
class Join {
public:
  /**
   * Construct Join with given null terminated vector of listeners
   * (Capsules), and number of signals required before dispatch.
   * @param[in] listeners null terminated vector of capsule references.
   * @param[in] count number of signals required.
   */
  Join(Capsule* const* listeners, uint8_t count) :
    m_listeners(listeners),
    m_count(count),
    m_current(count)
  {}

  /**
   * Signal join point. Schedules listeners when all joining parties
   * have signaled. Returns zero(0) if dispatched otherwise number of
   * signals before dispatch.
   */
  int signal()
  {
    synchronized {
      m_current -= 1;
      if (m_current != 0) synchronized_return (m_current);
      m_current = m_count;
    }
    controller.schedule(m_listeners);
    return (0);
  }

protected:
  Capsule* const* m_listeners;	//!< Null terminated vector of capsules.
  uint8_t m_count;		//!< Number of signals required.
  uint8_t m_current;		//!< Number of signals before dispatch.
};

};
#endif
