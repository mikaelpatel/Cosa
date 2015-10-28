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
 * Cosa demonstration of the DS1307 I2C/Two-Wire Real-Time Clock
 * device; read and write RAM, square wave signal generation,
 * and time keeping.
 *
 * @section Circuit
 * @code
 *                       TinyRTC(DS1307)
 *                       +------------+
 * (D2)----------------1-|SQ          |
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

#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Set the extern RTC
// #define SET_TIME

// Synchronized with external RTC square-ware
// #define SQUARE_WAVE_CLOCK

// The real-time device, latest start and sample time in ram
DS1307 rtc;

// Data structure stored in device ram; last set and run time
struct latest_t {
  time_t set;
  time_t run;
};

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// Clock pin
#if defined(SQUARE_WAVE_CLOCK)
#if defined(ARDUINO_PRO_MICRO)
InputPin clkPin(Board::D0);
#else
InputPin clkPin(Board::D2);
#endif
#endif

Watchdog::Clock clock;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaDS1307: started"));

  // Start the watchdog for low power yield
  Watchdog::begin();

  // Read the latest set and run time
  latest_t latest;
  int count = rtc.read(&latest, sizeof(latest), DS1307::RAM_START);
  ASSERT(count == sizeof(latest));
  latest.set.to_binary();
  latest.run.to_binary();

  // Print the latest set and run time
  trace << PSTR("set: ") << latest.set << endl;
  trace << PSTR("run: ") << latest.run << endl;

  // Set the time. Adjust below to your current time (BCD)
  time_t now;
#if defined(SET_TIME)
  now.year = 0x15;
  now.month = 0x10;
  now.date = 0x20;
  now.day = 0x02;
  now.hours = 0x19;
  now.minutes = 0x37;
  now.seconds = 0x30;
  rtc.set_time(now);
  latest.set = now;
#else
  rtc.get_time(now);
#endif

  // Update the run time with the current time and update ram
  latest.run = now;
  count = rtc.write(&latest, sizeof(latest), DS1307::RAM_START);
  ASSERT(count == sizeof(latest));

  // Enable square wave generation
  ASSERT(rtc.enable());

  // Print the control register
  DS1307::control_t control;
  uint8_t pos = offsetof(DS1307::timekeeper_t, control);
  count = rtc.read(&control, sizeof(control), pos);
  ASSERT(count == sizeof(control));
  trace << PSTR("control: ") << bin << control << endl;
  trace.flush();
}

void loop()
{
  // Wait for rising edge on clock pin
  uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);
#if defined(SQUARE_WAVE_CLOCK)
  while (clkPin.is_clear()) yield();
#else
  clock.await();
#endif
  Power::set(mode);
  ledPin.set();

  // Read the time from the rtc device and print
  time_t now;
  ASSERT(rtc.get_time(now));
  now.to_binary();
  trace << clock.time() << ':' << now << endl;
  trace.flush();

  // Wait for falling edge on clock pin
#if defined(SQUARE_WAVE_CLOCK)
  mode = Power::set(SLEEP_MODE_PWR_DOWN);
  while (clkPin.is_set()) yield();
  Power::set(mode);
#endif
  ledPin.clear();
}
