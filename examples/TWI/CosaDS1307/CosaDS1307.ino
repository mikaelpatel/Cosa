/**
 * @file CosaDS1307.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Cosa demonstration of the DS1307 I2C/Two-Wire Realtime clock device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/DS1307.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Set the real-time clock
// #define __RTC_SET_TIME__

// The real-time device, latest start and sample time in ram
DS1307 rtc;

// Data structure stored in device ram; last set and run time
struct latest_t {
  time_t set;
  time_t run;
};

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

#if defined(__ARDUINO_TINYX5__)
#include "Cosa/Soft/UART.hh"
Soft::UART uart(Board::D4);
#endif

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
  rtc.read(&latest, sizeof(latest), DS1307::RAM_START);

  // Set the time. Adjust below to your current time
  time_t now;
#ifdef __RTC_SET_TIME__
  now.year = 0x13;
  now.month = 0x07;
  now.date = 0x15;
  now.day = 0x02;
  now.hours = 0x12;
  now.minutes = 0x45;
  now.seconds = 0x00;
  rtc.set_time(now);
  latest.set = now;
#endif

  // Print latest set time
  trace.print_P(PSTR("set on "));
  trace << latest.set << endl;

  // And the latest run time
  trace.print_P(PSTR("run on "));
  trace << latest.run << endl;

  // Update the run time with the current time and update ram
  rtc.get_time(now);
  latest.run = now;
  rtc.write(&latest, sizeof(latest), DS1307::RAM_START);
}

void loop()
{
  // Wait a second
  SLEEP(1);
  ledPin.toggle();

  // Read the time from the rtc device and print
  time_t now;
  rtc.get_time(now);
  trace << now << endl;

  // Heartbeat
  ledPin.toggle();
}
