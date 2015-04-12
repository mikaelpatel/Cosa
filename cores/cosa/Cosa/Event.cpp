/**
 * @file Cosa/Event.cpp
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

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"

Queue<Event, Event::QUEUE_MAX> Event::queue;

bool
Event::service(uint32_t ms)
{
  uint32_t start = Watchdog::millis();
  Event event;
  while (!queue.dequeue(&event)) {
    if ((ms == 0L) || (Watchdog::since(start) < ms))
      yield();
    else
      return (false);
  }
  event.dispatch();
  return (true);
}

bool
Event::operator==(const Event &t)
{
  return (m_type == t.m_type &&
          m_target == t.m_target &&
          m_value == t.m_value);
}
