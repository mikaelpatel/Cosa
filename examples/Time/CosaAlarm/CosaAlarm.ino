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
 * @section Description
 * Demonstrate of Cosa Alarm handling.
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
  TraceAlarm(uint8_t id, uint16_t period = 0L);
  virtual void run();
private:
  uint8_t m_id;
  uint16_t m_tick;
};

TraceAlarm::TraceAlarm(uint8_t id, uint16_t period) : 
  Alarm(period), 
  m_id(id), 
  m_tick(0) 
{}

Alarm::Scheduler scheduler;

TraceAlarm every_3rd_second(1, 3);
TraceAlarm every_5th_second(2, 5);
TraceAlarm every_15th_second(3, 15);
TraceAlarm every_30th_second(4, 30);

void 
TraceAlarm::run()
{
  trace << Watchdog::millis() << ':'
	<< RTC::millis() << ':'
	<< RTC::seconds() << ':'
	<< time() << ':'
	<< ++m_tick << ':'
	<< every_3rd_second.expires_in() << ':'
	<< every_5th_second.expires_in() << ':'
	<< every_15th_second.expires_in() << ':'
	<< every_30th_second.expires_in() << ':'
	<< PSTR("alarm:id=") << m_id 
	<< endl;

}

void setup()
{
  // Start serial device and use as trace iostream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAlarm: started"));

  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(Alarm::Scheduler));
  TRACE(sizeof(Alarm));
  TRACE(sizeof(TraceAlarm));

  // Start the watchdog, real-time clock and the alarm scheduler
  Watchdog::begin(16, Watchdog::push_timeout_events);
  RTC::begin();
  scheduler.begin();

  // Set time to just before wrap
  RTC::time(-15);

  // Match time in Alarm
  Alarm::set_time(RTC::time());

  // Set next alarms
  every_3rd_second.next_alarm(3);
  every_5th_second.next_alarm(5);
  every_15th_second.next_alarm(15);
  every_30th_second.next_alarm(30);

  // Enable the alarm handlers
  every_30th_second.enable();
  every_15th_second.enable();
  every_5th_second.enable();
  every_3rd_second.enable();

  // Format
  trace << PSTR("wtd-millis:rtc-millis:rtc-seconds:alarm:tick:3rd-in:5th-in:15th-in:30th-in") << endl;
}

void loop()
{
  // The standard event dispatcher
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
