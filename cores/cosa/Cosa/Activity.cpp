/**
 * @file Cosa/Activity.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "Cosa/Activity.hh"

void 
Activity::set_time(clock_t time, uint16_t duration, uint16_t period)
{
  m_start_time = time;
  m_stop_time = time + duration;
  m_duration = duration;
  m_period = period;
  m_scheduler.set_alarm(time);
}

void 
Activity::schedule(clock_t now) 
{ 
  // Check if still in activity duration
  int32_t diff = now - m_stop_time;
  if (diff < 0) {
    run();
    if (m_cycles == 0) m_scheduler.set_period(m_run_period);
    m_cycles += 1;
  }
  
  // Check if activity should be rescheduled
  else if (m_period != 0) {
    m_start_time += m_period * 60L;
    m_stop_time = m_start_time + m_duration;
    m_scheduler.set_alarm(m_start_time);
    m_scheduler.set_period(0);
    m_scheduler.enable();
    m_cycles = 0;
  }
}

