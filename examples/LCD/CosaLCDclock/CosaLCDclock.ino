/**
 * @file CosaLCDclock.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Cosa LCD demo with RTC. For Arduino Mega this sketch will require
 * an 2004 LCD, an ADXL345 Accelerometer and a HMC5883L Digital Compass.
 *
 * @section Circuit
 * See HD44780.hh for description of LCD adapter circuits.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/TWI/Driver/DS3231.hh"

// Configurations
#define USE_LARGE_LCD
// #define USE_SENSORS
// #define SET_RTC_TIME

// Select the access port for the LCD
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780::ERM1602_5 port;

#if defined(USE_SENSORS)
#include "Cosa/TWI/Driver/ADXL345.hh"
#include "Cosa/TWI/Driver/HMC5883L.hh"

// Digital acceleratometer with alternative address
ADXL345 accelerometer(1);

// The 3-Axis Digital Compass
HMC5883L compass;
#endif

#if defined(USE_LARGE_LCD)
HD44780 lcd(&port, 20, 4);
#else
HD44780 lcd(&port);
#endif

// Use Real-time clock with temperature sensor
DS3231 rtc;

// Bind the lcd to an io-stream
IOStream cout(&lcd);

void setup()
{
  Watchdog::begin();
  lcd.begin();
  cout << PSTR("CosaLCDclock: started");

#if defined(SET_RTC_TIME)
  time_t now;
  now.seconds = 0x00;
  now.minutes = 0x08;
  now.hours = 0x00;
  now.day = 0x01;
  now.date = 0x24;
  now.month = 0x03;
  now.year = 0x14;
  rtc.set_time(now);
#endif

#if defined(USE_SENSORS)
  // Start the accelerometer with the default settings
  accelerometer.begin();

  // Set continous measurement mode, 3 Hz output, avg 8 samples, +-4.0 Gauss
  compass.set_output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.set_samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.set_range(HMC5883L::RANGE_4_0_GA);
  compass.set_mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);

  // And start the compass
  compass.begin();
#endif

  // Give the sensors some time for startup
  sleep(1);
  cout << clear;
}

void loop()
{
  // Read clock and temperature
  time_t now;
  rtc.get_time(now);
  int16_t temp = rtc.get_temperature();

  // First line with date and temperature. Use BCD format output
  lcd.set_cursor(0, 0);
  cout << PSTR("20") << bcd << now.year << '-'
       << bcd << now.month << '-'
       << bcd << now.date << ' '
       << (temp >> 2)
#if defined(USE_LARGE_LCD)
       << '.' << (25 * (temp & 0x3))
#endif
       << PSTR(" C");
  
  // Second line with time and battery status
  lcd.set_cursor(0, 1);
  cout << bcd << now.hours << ':'
       << bcd << now.minutes << ':'
       << bcd << now.seconds << ' '
       << AnalogPin::bandgap(1100) << PSTR(" mV");

#if defined(USE_SENSORS)
  // Read the heading, scale to milli gauss and print the data
  compass.read_heading();
  compass.to_milli_gauss();
  HMC5883L::data_t dir;
  compass.get_heading(dir);
  lcd.set_cursor(0, 2);
  cout << dir.x << PSTR(", ") << dir.y << PSTR(", ") << dir.z << PSTR(" mG");

  // Read the accelerometer and print the data
  ADXL345::sample_t acc;
  accelerometer.sample(acc);
  lcd.set_cursor(0, 3);
  cout << acc.x << PSTR(", ") << acc.y << PSTR(", ") << acc.z << PSTR(" mg");
#endif

  // Delay until the next tick
  clock_t start = now;
  do {
    delay(200);
    rtc.get_time(now);
  } while (start == now);
}
