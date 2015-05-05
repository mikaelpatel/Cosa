/**
 * @file CosaDS1307.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa demonstration of the DS1307 I2C/Two-Wire Realtime clock device.
 *
 * @section Circuit
 * @code
 *                       TinyRTC(DS1307)
 *                       +------------+
 *                     1-|SQ          |
 *                     2-|DS        DS|-1
 * (A5/SCL)------------3-|SCL      SCL|-2
 * (A4/SDA)------------4-|SDA      SDA|-3
 * (VCC)---------------5-|VCC      VCC|-4
 * (GND)---------------6-|GND      GND|-5
 *                     7-|BAT         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DS1307.h>

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Set the real-time clock
// #define SET_TIME

// The real-time device, latest start and sample time in ram
DS1307 rtc;

// Data structure stored in device ram; last set and run time
struct latest_t {
  time_t set;
  time_t run;
};

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS1307: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Read the latest set and run time
  latest_t latest;
  int count = rtc.read(&latest, sizeof(latest), DS1307::RAM_START);
  TRACE(count);

  // Print latest set and the latest run time
  trace.print(PSTR("set on "));
  trace << latest.set << endl;
  trace.print(PSTR("run on "));
  trace << latest.run << endl;

  // Set the time. Adjust below to your current time
  time_t now;
#if defined(SET_TIME)
  now.year = 0x14;
  now.month = 0x05;
  now.date = 0x05;
  now.day = 0x00;
  now.hours = 0x00;
  now.minutes = 0x01;
  now.seconds = 0x00;
  rtc.set_time(now);
  latest.set = now;
#else
  rtc.get_time(now);
#endif

  // Update the run time with the current time and update ram
  latest.run = now;
  count = rtc.write(&latest, sizeof(latest), DS1307::RAM_START);
  TRACE(count);
}

void loop()
{
  // Wait a second
  sleep(1);
  ledPin.toggle();

  // Read the time from the rtc device and print
  time_t now;
  if (rtc.get_time(now)) {
    now.to_binary();
    trace << now << endl;
  }

  // Heartbeat
  ledPin.toggle();
}
