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
  Alarm* alarm;
  s_ticks += 1;
  while ((alarm = (Alarm*) s_queue.get_succ()) != (Alarm*) &s_queue) {
    int32_t diff = alarm->m_when - s_ticks;
    if (diff > 0) break;
    alarm->detach();
    alarm->run();
    if (alarm->m_period != 0) {
      alarm->m_when += alarm->m_period;
      alarm->enable();
    }
  }
}

void
Alarm::enable()
{
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
  int32_t diff = RTC::seconds() - Alarm::s_ticks;
  if (diff <= 0) return;
  Alarm::tick();
}
