/**
 * @file Cosa/Alarm.cpp
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

#include "Cosa/Alarm.hh"
#include "Cosa/RTC.hh"

uint32_t Alarm::s_ticks;
Head Alarm::s_queue;

void
Alarm::tick()
{
  Alarm* alarm;
  s_ticks += 1;

  // Check for alarms that should be run
  while ((alarm = (Alarm*) s_queue.get_succ()) != (Alarm*) &s_queue) {
    int32_t diff = alarm->m_when - s_ticks;
    if (diff > 0) break;
    alarm->detach();
    alarm->run();
    // Check if the alarm should be rescheduled (periodic)
    if (alarm->m_period != 0) {
      alarm->m_when += alarm->m_period;
      alarm->enable();
    }
  }
}

void
Alarm::enable()
{
  // Enqueue the alarm in the schedule queue
  Alarm* alarm = (Alarm*) s_queue.get_succ(); 
  while (alarm != (Alarm*) &s_queue) {
    int32_t diff = m_when - alarm->m_when;
    if (diff <= 0) break;
    alarm = (Alarm*) alarm->get_succ();
  }
  alarm->attach(this);
}

void
Alarm::Scheduler::run()
{
  // Check if it is time to run the alarm update
  int32_t diff = RTC::seconds() - Alarm::s_ticks;
  if (diff <= 0) return;
  Alarm::tick();
}
