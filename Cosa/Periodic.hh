/**
 * @file Cosa/Periodic.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Periodic function handler. Syntactic sugar for watchdog timeout
 * event handlers. Subclass and implement the virtual method run()
 * as the function to be executed periodically.
 *
 * @section Limitations
 * Avoid setting period to the same value in the run method as this
 * will force the function to be executed twice in the same time frame. 
 *
 * @section See Also
 * For details on time period handling see Watchdog.hh. This execution
 * pattern is also available in the FSM (Finite State Machine) class.
 * See FSM.hh.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_PERIODIC_HH__
#define __COSA_PERIODIC_HH__

#include "Cosa/Types.h"
#include "Cosa/Thing.hh"
#include "Cosa/Watchdog.hh"

class Periodic : public Thing {

private:
  /**
   * @override
   * Periodic event handler; dispatch the run() function on
   * timeout events.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    if (type != Event::TIMEOUT_TYPE) return;
    run();
  }

public:
  /**
   * Construct a periodic function handler. 
   * @param[in] ms period of timeout.
   */
  Periodic(uint16_t ms) : 
    Thing()
  {
    set_period(ms);
  }

  /**
   * Set timeout period.
   * @param[in] ms period of timeout.
   */
  void set_period(uint16_t ms)
  {
    Watchdog::attach(this, ms);
  }

  /**
   * The default null function. 
   */
  virtual void run()
  {
  }
};

#endif
