/**
 * @file CosaAlarm.ino
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
 * @section Description
 * Demonstrate of Cosa Alarm handling.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Time.hh"
#include "Cosa/Alarm.hh"
#include "Cosa/Event.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class TraceAlarm : public Alarm {
public:
  TraceAlarm(Job::Scheduler* scheduler, uint8_t id, uint16_t period = 0L);
  virtual void run();
private:
  uint8_t m_id;
  uint16_t m_tick;
};

TraceAlarm::TraceAlarm(Job::Scheduler* scheduler, uint8_t id, uint16_t period) :
  Alarm(scheduler, period),
  m_id(id),
  m_tick(0)
{}

RTC::Scheduler scheduler;
Alarm::Scheduler alarms(&scheduler);

TraceAlarm alarm1(&alarms, 1, 3);
TraceAlarm alarm2(&alarms, 2, 5);
TraceAlarm alarm3(&alarms, 3, 15);
TraceAlarm alarm4(&alarms, 4, 30);

void
TraceAlarm::run()
{
  uint32_t now = RTC::seconds();
  uint32_t expires = expire_at();
  int32_t diff = expires - now;
  trace << time_t(now) << ':' << expires
	<< (diff < 0 ? PSTR(":T") : PSTR(":T+")) << diff
    	<< PSTR(":alarm:id=") << m_id
	<< PSTR(",period=") << period()
	<< PSTR(",tick=") << ++m_tick
	<< endl;
}

void setup()
{
  // Start serial device and use as trace iostream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAlarm: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(Event::Handler));
  TRACE(sizeof(Link));
  TRACE(sizeof(Job));
  TRACE(sizeof(Periodic));
  TRACE(sizeof(Alarm));
  TRACE(sizeof(TraceAlarm));
  TRACE(sizeof(Job::Scheduler));
  TRACE(sizeof(Alarm::Scheduler));

  // Start the watchdog, real-time clock and the alarm scheduler
  Watchdog::begin();
  RTC::begin();
  RTC::job(&scheduler);
  alarms.begin();

  // Start the alarm handlers
  alarm1.begin();
  alarm2.begin();
  alarm3.begin();
  alarm4.begin();
}

void loop()
{
  // The standard event dispatcher
  Event::service();
}
