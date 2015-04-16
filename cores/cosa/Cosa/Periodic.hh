/**
 * @file Cosa/Periodic.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_PERIODIC_HH
#define COSA_PERIODIC_HH

#include "Cosa/Types.h"
#include "Cosa/Linkage.hh"
#include "Cosa/Watchdog.hh"

/**
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
 */
class Periodic : protected Link {
public:
  /**
   * Construct a periodic function handler.
   * @param[in] ms period of timeout.
   */
  Periodic(uint16_t ms) : Link(), m_period(ms) {}

  /**
   * Set timeout period.
   * @param[in] ms period of timeout.
   */
  void set_period(uint16_t ms)
  {
    m_period = ms;
  }

  /**
   * Start the periodic function.
   */
  void begin()
    __attribute__((always_inline))
  {
    Watchdog::attach(this, m_period);
  }

  /**
   * Stop the periodic function.
   */
  void end()
    __attribute__((always_inline))
  {
    detach();
  }

  /**
   * @override Periodic
   * The default null function.
   */
  virtual void run() {}

private:
  /**
   * @override Event::Handler
   * Periodic event handler; dispatch the run() function on
   * timeout events.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    UNUSED(value);
    if (type != Event::TIMEOUT_TYPE) return;
    run();
  }

  uint16_t m_period;
};

/**
 * Syntactic sugar for periodic block. Used in the form:
 * @code
 * PERIODIC(ms) {
 *   ...
 * }
 * @endcode
 */
#define PERIODIC(ms)							\
  for (uint32_t start = RTC::millis(), i = 1;				\
       i != 0;								\
       i--, delay(ms - RTC::since(start)))

#endif
