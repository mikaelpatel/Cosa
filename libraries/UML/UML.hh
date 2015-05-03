/**
 * @file UML.hh
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

#ifndef COSA_UML_HH
#define COSA_UML_HH

#include "Cosa/RTC.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

#include "UML/Controller.hh"

namespace UML {
  /**
   * Start UML run-time; initiates necessary timers, etc.
   * @param[in] ms watchdog period (Default 16 ms).
   */
  void begin(uint16_t ms = 16)
  {
    Watchdog::begin(ms, Watchdog::push_timeout_events);
    RTC::begin();
  }

  /**
   * Service UML run-time; dispatch events and run capsules.
   */
  void service()
  {
    for (Event event; Event::queue.dequeue(&event);) event.dispatch();
    controller.run();
  }
};
#endif
