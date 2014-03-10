/**
 * @file Cosa/Alarm.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Alarm.hh"
#include "Cosa/RTC.hh"

uint32_t Alarm::s_ticks;
Head Alarm::s_queue;

void
Alarm::tick()
{
  s_ticks += 1;
  Alarm* alarm = (Alarm*) s_queue.get_succ(); 
  while (alarm != (Alarm*) &s_queue) {
    if (alarm->m_when > s_ticks) return;
    Alarm* succ = (Alarm*) alarm->get_succ();
    alarm->detach();
    alarm->run();
    if (alarm->m_period != 0) {
      alarm->m_when += alarm->m_period;
      alarm->enable();
    }
    alarm = succ;
  }
}

void
Alarm::enable()
{
  Alarm* alarm = (Alarm*) s_queue.get_succ(); 
  while (alarm != (Alarm*) &s_queue) {
    if (m_when < alarm->m_when) break;
    alarm = (Alarm*) alarm->get_succ();
  }
  alarm->attach(this);
}

void
Alarm::Scheduler::run()
{
  uint32_t now = RTC::seconds();
  if (now == Alarm::s_ticks) return;
  Alarm::tick();
}
