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
 * Demonstrate of Cosa Alarm handling. May be configured to use
 * either the RTC Scheduler or External Interrupt based Alarm Handler.
 * The DS1307 square way output is used as interrupt source.
 *
 * @section Circuit
 * @code
 *                       TinyRTC(DS1307)
 *                       +------------+
 * (EXT0/D2)-----------1-|SQ          |
 *                     2-|DS        DS|-1
 * (A5/SCL)------------3-|SCL      SCL|-2
 * (A4/SDA)------------4-|SDA      SDA|-3
 * (VCC)---------------5-|VCC      VCC|-4
 * (GND)---------------6-|GND      GND|-5
 *                     7-|BAT         |
 *                       +------------+
 * @endcode
 *
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Time.hh"
#include "Cosa/Clock.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Alarm.hh"
#include "Cosa/Event.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"

// Configuration: RTT, Watchdog or External Interrupt Clock Source
// #define USE_RTT_CLOCK
// #define USE_WATCHDOG_CLOCK
#define USE_ALARM_CLOCK

#if defined(USE_RTT_CLOCK)
RTT::Clock alarms;
#endif

#if defined(USE_WATCHDOG_CLOCK)
Watchdog::Clock alarms;
#endif

#if defined(USE_ALARM_CLOCK)
#include <DS1307.h>
DS1307 rtc;
Alarm::Clock alarms(Board::EXT0);
#endif

// Trace the expired alarms. Note that the Clock should be the outer
// context and not the default i.e. Alarm::Clock
class TraceAlarm : public Alarm {
public:
  TraceAlarm(::Clock* clock, uint8_t id, uint16_t period) :
    Alarm(clock, period),
    m_id(id),
    m_tick(0)
  {
    expire_at(period);
  }

  virtual void run()
  {
    uint32_t now = time();
    uint32_t expires = expire_at();
    int32_t diff = expires - now;
    trace << time_t(now) << ':' << expires
	  << (diff < 0 ? PSTR(":T") : PSTR(":T+")) << diff
	  << PSTR(":alarm:id=") << m_id
	  << PSTR(",period=") << period()
	  << PSTR(",tick=") << ++m_tick
	  << endl;
  }

private:
  uint8_t m_id;
  uint16_t m_tick;
};

// The alarms with the given period in seconds
TraceAlarm alarm1(&alarms, 1, 3);
TraceAlarm alarm2(&alarms, 2, 5);
TraceAlarm alarm3(&alarms, 3, 15);
TraceAlarm alarm4(&alarms, 4, 30);

void setup()
{
  // Start serial device and use as trace iostream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAlarm: started"));

#if defined(USE_RTT_CLOCK)

  trace << PSTR("RTT clock") << endl;
  trace.flush();

  // Start the real-time timer
  RTT::begin();

#elif defined(USE_WATCHDOG_CLOCK)

  trace << PSTR("Watchdog clock") << endl;
  trace.flush();

  // Start the watchdog
  Watchdog::begin();

#elif defined(USE_ALARM_CLOCK)

  trace << PSTR("Alarm clock (DS1307/EXT0)") << endl;
  trace.flush();

  // Get current time from external real-time clock
  time_t now;
  rtc.get_time(now);
  now.to_binary();

  // Adjust the expire time for the alarms
  clock_t clock = now;
  alarm1.expire_after(clock);
  alarm2.expire_after(clock);
  alarm3.expire_after(clock);
  alarm4.expire_after(clock);
  alarms.time(clock);

  // Set the alarm scheduler time and enable ticks
  alarms.enable();
#endif

  // Start the alarm handlers
  alarm1.start();
  alarm2.start();
  alarm3.start();
  alarm4.start();
}

void loop()
{
  // The standard event dispatcher
  Event::service();
}
