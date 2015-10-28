/**
 * @file CosaMCP7940N.ino
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
 * Cosa demonstration of the MCP7940N, Real-Time Clock/Calendar with
 * SRAM and Battery Switchover, device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <MCP7940N.h>

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// The real-time device
MCP7940N rtc;

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaMCP7940N: started"));

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(rtc));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Initiate time keeper
  time_t now;
  now.seconds = 0x30;
  now.minutes = 0x24;
  now.hours = 0x22;
  now.day = 0x01;
  now.date = 0x23;
  now.month = 0x6;
  now.year = 0x14;
  TRACE(rtc.set_time(now));
  now.to_binary();
  trace << PSTR("time:     ") << now << endl;

  // Set alarm 0 when seconds match (every minute)
  time_t alarm(now);
  alarm.to_bcd();
  uint8_t when = MCP7940N::WHEN_SEC_MATCH;
  alarm.seconds = 0x45;
  rtc.set_alarm(0, alarm, when);

  // Set alarm 1 when time match; in a minute
  alarm.minutes += 1;
  when = MCP7940N::WHEN_TIME_MATCH;
  rtc.set_alarm(1, alarm, when);

  // Check the settings
  rtc.get_alarm(0, alarm, when);
  alarm.to_binary();
  trace << PSTR("alarm(0): ") << alarm << ' ' << when << endl;
  rtc.get_alarm(1, alarm, when);
  alarm.to_binary();
  trace << PSTR("alarm(1): ") << alarm << ' ' << when << endl;

  // Enable square wave output
  TRACE(rtc.square_wave(true));
}

void loop()
{
  // Run smoothly
  delay(200);

  // Read the time from the rtc device
  time_t now;
  rtc.get_time(now);
  now.to_binary();

  // Convert to seconds from epoch (will convert now to binary)
  static clock_t clock = 0;
  clock_t epoch(now);
  if (epoch == clock) return;
  clock = epoch;

  // Heart beat
  ledPin.toggle();

  // Print epoch and adjust back to bcd and print time
  trace << epoch << ':';
  trace << now << ' ';

  // Create time and adjust for zone(2). Use epoch for calculation
  now = time_t(epoch, 2);
  trace << now << ' ';

  // Check pending alarms; set alarm 0 in 5 seconds, alarm 1 in 10 seconds
  uint8_t pending = rtc.pending_alarm();
  if (pending & 0x01) {
    trace << PSTR("alarm(0) ");
    time_t alarm(epoch + 5);
    alarm.to_bcd();
    rtc.set_alarm(0, alarm, MCP7940N::WHEN_SEC_MATCH);
  }
  if (pending & 0x02) {
    trace << PSTR("alarm(1)");
    time_t alarm(epoch + 10);
    alarm.to_bcd();
    rtc.set_alarm(1, alarm, MCP7940N::WHEN_SEC_MATCH);
  }
  trace << endl;

  // Heartbeat
  ledPin.toggle();
}
