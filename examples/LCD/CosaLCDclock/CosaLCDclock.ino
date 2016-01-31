/**
 * @file CosaLCDclock.ino
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
 * Cosa LCD demo with RTC. For Arduino Mega this sketch will require
 * an 2004 LCD, an ADXL345 Accelerometer and a HMC5883L Digital Compass.
 *
 * @section Circuit
 * See HD44780.hh for description of LCD adapter circuits.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DS1307.h>

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"

// Configurations
#define USE_LARGE_LCD
// #define USE_SENSORS
// #define SET_RTC_TIME

// Select port type to use with the LCD device driver.
// LCD and communication port
#include <HD44780.h>

// HD44780 driver built-in adapters
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;

// I2C expander io port based adapters
#include <PCF8574.h>
#include <MJKDZ_LCD_Module.h>
// MJKDZ_LCD_Module port;
MJKDZ_LCD_Module port(0);
// #include <GY_IICLCD.h>
// GY_IICLCD port;
// #include <DFRobot_IIC_LCD_Module.h>
// DFRobot_IIC_LCD_Module port;
// #include <SainSmart_LCD2004.h>
// SainSmart_LCD2004 port;
// #include <MCP23008.h>
// #include <Adafruit_I2C_LCD_Backpack.h>
// Adafruit_I2C_LCD_Backpack port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

#if defined(USE_SENSORS)
#include <ADXL345.h>
#include <HMC5883L.h>

// Digital acceleratometer with alternative address
ADXL345 accelerometer(1);

// The 3-Axis Digital Compass
HMC5883L compass;
#endif

#if defined(USE_LARGE_LCD)
// HD44780 lcd(&port, 20, 4);
HD44780 lcd(&port, 16, 4);
#else
HD44780 lcd(&port);
#endif

// Use real-time clock with temperature sensor
DS1307 rtc;

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
  now.minutes = 0x52;
  now.hours = 0x13;
  now.day = 0x05;
  now.date = 0x15;
  now.month = 0x05;
  now.year = 0x15;
  rtc.set_time(now);
#endif

#if defined(USE_SENSORS)
  // Start the accelerometer with the default settings
  accelerometer.begin();

  // Set continous measurement mode, 3 Hz output, avg 8 samples, +-4.0 Gauss
  compass.output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.range(HMC5883L::RANGE_4_0_GA);
  compass.mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);

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

  // First line with date and time.
  lcd.set_cursor(0, 0);
  cout << 20 << bcd << now.year << '-'
       << bcd << now.month << '-'
       << bcd << now.date << ' '
       << bcd << now.hours << ':'
       << bcd << now.minutes
       << endl;

  // Second line with battery status
  cout << AnalogPin::bandgap(1100) << PSTR(" mV");
  lcd.set_cursor(lcd.WIDTH - 2, 1);
  cout << bcd << now.seconds;

#if defined(USE_SENSORS)
  // Read the heading, scale to milli gauss and print the data
  compass.read_heading();
  compass.to_milli_gauss();
  HMC5883L::data_t dir;
  compass.heading(dir);
  lcd.set_cursor(0, 2);
  cout << dir.x << PSTR(",") << dir.y << PSTR(",") << dir.z << PSTR(" mG");

  // Read the accelerometer and print the data
  ADXL345::sample_t acc;
  accelerometer.sample(acc);
  lcd.set_cursor(0, 3);
  cout << acc.x << PSTR(",") << acc.y << PSTR(",") << acc.z << PSTR(" mg");
#endif

  // Delay until the next tick
  now.to_binary();
  clock_t start = now;
  do {
    delay(200);
    rtc.get_time(now);
    now.to_binary();
  } while (start == now);
}
