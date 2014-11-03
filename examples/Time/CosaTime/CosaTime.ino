/**
 * @file CosaTime.ino
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
 * Cosa Time demo with internal RTC. Show time zone handling.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Time.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Bind the uart to an io-stream
IOStream cout(&uart);

// Start time (set to 30 seconds before New Years 2014/Stockholm)
clock_t epoch;

void setup()
{
  RTC::begin();
  Watchdog::begin();
  uart.begin(9600);
  cout << PSTR("CosaTime: started");

  // Set start time
  time_t now;
  now.seconds = 0x30;
  now.minutes = 0x59;
  now.hours = 0x22;
  now.date = 0x31;
  now.month = 0x12;
  now.year = 0x13;
  now.to_binary();
  epoch = now;
  RTC::time(epoch);

  // Sleep ten seconds 
  for (uint8_t i = 0; i < 10; i++) {
    cout << '.';
    sleep(1);
  }
  cout << endl;
}

void loop()
{
  // Read internal RTC time and create time for time zones
  clock_t clock = RTC::time();
  time_t se(clock, 2);
  time_t utc(clock);
  time_t us(clock, -4);

  // Print seconds since epoch and time zones
  cout << clock - epoch << ':'
       << PSTR("se=") << se << ',' 
       << PSTR("utc=") << utc << ','
       << PSTR("us=") << us << ',';

  // Take a nap until seconds update. Count number of yields
  uint8_t cnt = 0;
  while (clock == RTC::time()) {
    cnt += 1;
    yield();
  }

  // Print number of yields
  cout << PSTR("cnt=") << cnt << endl;
}
