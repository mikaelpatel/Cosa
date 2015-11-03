/**
 * @file CosaDS3231.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Cosa demonstration of the DS3231, Extremely Accurate I2C-Integrated
 * RTC/TCXO/Crystal device driver.
 *
 * @section Circuit
 * The Mini RTC pro module with pull-up resistors (4K7) for TWI signals.
 * @code
 *                        Mini RTC pro
 *                       +------------+
 *                     1-|32KHz       |
 * (D3)----------------2-|SQW         |
 * (A5/SCL)------------3-|SCL         |
 * (A4/SDA)------------4-|SDA         |
 * (GND)---------------5-|GND         |
 * (VCC)---------------6-|VCC         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DS3231.h>

#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// Define to set clock
// #define RTC_SET_TIME

// The real-time device
DS3231 rtc;

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

// Square-wave
InputPin clkPin(Board::D3);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS3231: started"));
  trace.precision(2);
  trace.width(4);

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(rtc));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Initiate time keeper
#ifdef RTC_SET_TIME
  time_t now;
  now.seconds = 0x00;
  now.minutes = 0x13;
  now.hours = 0x00;
  now.day = 0x05;
  now.date = 0x24;
  now.month = 0x10;
  now.year = 0x15;
  ASSERT(rtc.set_time(now));
#endif

  // Read back and print current setting
  DS3231::timekeeper_t keeper;
  ASSERT(rtc.read(&keeper, sizeof(keeper)) == sizeof(keeper));
  trace << keeper << endl;

  // Set square-wave output
  TRACE(rtc.square_wave(true));
  trace.flush();
}

void loop()
{
  // Wait for rising clock signal
  uart.powerdown();
  uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);
  while (clkPin.is_clear()) yield();
  Power::set(mode);
  uart.powerup();
  ledPin.toggle();

  // Read the time from the rtc device and print
  time_t now;
  ASSERT(rtc.get_time(now));
  now.to_binary();
  float temp = rtc.temperature() * 0.25;
  trace << now << ' ' << temp << PSTR(" C") << endl;

  // Wait for falling clock signal
  ledPin.toggle();
  trace.flush();
  uart.powerdown();
  mode = Power::set(SLEEP_MODE_PWR_DOWN);
  while (clkPin.is_set()) yield();
  Power::set(mode);
  uart.powerup();
}
