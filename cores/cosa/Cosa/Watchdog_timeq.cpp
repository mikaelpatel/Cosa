/**
 * @file Cosa/Watchdog_timeq.cpp
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

#include "Cosa/Watchdog.hh"

Head Watchdog::s_timeq[Watchdog::TIMEQ_MAX];

void
Watchdog::attach(Link* target, uint16_t ms)
{
  // Map milli-seconds to watchdog time queue index
  uint8_t level = as_prescale(ms);

  // Attach the target to the selected time queue
  s_timeq[level].attach(target);
}

void
Watchdog::push_timeout_events(void* env)
{
  UNUSED(env);
  uint32_t changed = (s_ticks ^ (s_ticks + 1));
  for (uint8_t i = s_prescale; i < TIMEQ_MAX; i++, changed >>= 1)
    if ((changed & 1) && !s_timeq[i].is_empty())
      Event::push(Event::TIMEOUT_TYPE, &s_timeq[i], i);
}

