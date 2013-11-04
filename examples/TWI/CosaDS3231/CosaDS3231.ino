/**
 * @file CosaDS3231.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Cosa demonstration of the DS3231, Extremely Accurate I2C-Integrated
 * RTC/TCXO/Crystal device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/DS3231.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// Define to set clock
// #define __RTC_SET_TIME__

// The real-time device
DS3231 rtc;

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS3231: started"));
  
  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(rtc));
  
  // Start the watchdog ticks counter
  Watchdog::begin();

  // Initiate time keeper
#ifdef __RTC_SET_TIME__
  time_t now;
  now.seconds = 0x00;
  now.minutes = 0x24;
  now.hours = 0x22;
  now.day = 0x01;
  now.date = 0x23;
  now.month = 0x6;
  now.year = 0x13;
  rtc.set_time(now);
#endif
  
  // Read back and print current setting
  DS3231::timekeeper_t keeper;
  rtc.read(&keeper, sizeof(keeper));
  trace << keeper << endl;
}

void loop()
{
  // Wait a second
  SLEEP(1);
  ledPin.toggle();

  // Read the time from the rtc device and print
  time_t now;
  rtc.get_time(now);
  trace << now << ' ';
  int16_t temp = rtc.get_temperature();
  trace << (temp >> 2) << '.' << (25 * (temp & 0x3)) << PSTR(" C") << endl;

  // Heartbeat
  ledPin.toggle();
}
