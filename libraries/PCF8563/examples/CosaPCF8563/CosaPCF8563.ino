/**
 * @file CosaPCF8563.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * Cosa demonstration of the PCF8563, Real-Time Clock/Calendar.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <PCF8563.h>

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// The real-time device
PCF8563 rtc;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPCF8563: started"));

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(rtc));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Initiate time keeper
  time_t now;
  now.seconds = 0x45;
  now.minutes = 0x59;
  now.hours = 0x23;
  now.day = SUNDAY;
  now.date = 0x24;
  now.month = 0x06;
  now.year = 0x14;
  ASSERT(rtc.set_time(now));

  // Get initial alarm setting
  PCF8563::alarm_t alarm;
  ASSERT(rtc.get_alarm(alarm));
  trace << PSTR("alarm:") << alarm << endl;

  // Set alarm
  // alarm.day = MONDAY;
  // alarm.date = 0x25;
  alarm.minutes = 0x00;
  alarm.hours = 0x00;
  ASSERT(rtc.set_alarm(alarm));
}

void loop()
{
  PCF8563::alarm_t alarm;
  time_t now;

  ASSERT(rtc.get_alarm(alarm));
  trace << PSTR("alarm:") << alarm << endl;

  while (!rtc.pending_alarm()) {
    sleep(1);
    ASSERT(rtc.get_time(now));
    now.to_binary();
    trace << clock_t(now) << ':' << now << ' ' << now.day << endl;
  }

  alarm.minutes = to_bcd((now.minutes + 1) % 60);
  ASSERT(rtc.set_alarm(alarm));
}
