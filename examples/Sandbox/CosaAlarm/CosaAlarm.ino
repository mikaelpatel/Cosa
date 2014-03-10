/**
 * @file CosaAlarm.ino
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
 * @section Description
 * Cosa Alarm.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Alarm.hh"
#include "Cosa/Event.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class TraceAlarm : public Alarm {
public:
  TraceAlarm(uint8_t id, uint16_t period) : Alarm(period), m_id(id) {}
  virtual void run();
private:
  uint8_t m_id;
};

void 
TraceAlarm::run()
{
  time_t now(RTC::seconds());
  trace << now << ':' << time() << PSTR(":alarm:id=") << m_id << endl;
}

Alarm::Scheduler scheduler;
TraceAlarm every_third_second(1, 3);
TraceAlarm every_fifth_second(2, 5);
TraceAlarm every_half_minute(3, 30);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAlarm: started"));
  TRACE(free_memory());
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  RTC::begin();
  every_third_second.enable();
  every_fifth_second.enable();
  every_half_minute.enable();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
