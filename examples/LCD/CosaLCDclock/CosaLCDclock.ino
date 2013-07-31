/**
 * @file CosaLCDclock.ino
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
 * @section Description
 * Cosa LCD demo with RTC.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/TWI/Driver/DS3231.hh"

// Remove comment to set real-time clock (update below)
// #define RTC_SET_TIME
DS3231 rtc;

// Select the access port for the LCD
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
HD44780::ERM1602_5 port;
// HD44780::MJKDZ port;
// HD44780::DFRobot port;
HD44780 lcd(&port);
IOStream cout(&lcd);

void setup()
{
  Watchdog::begin();
  lcd.begin();
  cout << PSTR("CosaLCDclock: started");

#ifdef RTC_SET_TIME
  time_t now;
  now.seconds = 0x00;
  now.minutes = 0x50;
  now.hours = 0x20;
  now.day = 0x01;
  now.date = 0x21;
  now.month = 0x07;
  now.year = 0x13;
  rtc.set_time(now);
#endif
  
  SLEEP(2);
}

void loop()
{
  // Read clock and temperature
  time_t now;
  rtc.get_time(now);
  int16_t temp = rtc.get_temperature();

  // Update the LCD with the reading
  cout << clear;

  // First line with date and temperature
  cout << PSTR("20") << bcd << now.year << '-'
       << bcd << now.month << '-'
       << bcd << now.date << PSTR("  ")
       << (temp >> 2) << PSTR(" C");

  // Second line with time and battery status
  lcd.set_cursor(0, 1);
  cout << bcd << now.hours << ':'
       << bcd << now.minutes << ':'
       << bcd << now.seconds << ' '
       << AnalogPin::bandgap(1100) << PSTR(" mV");

  // Take a nap
  SLEEP(1);
}
