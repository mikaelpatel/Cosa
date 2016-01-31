/**
 * @file CosaLCD10DOF.ino
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
 * Cosa demonstration of 10 DOF module (GY-80) with ADXL345, BMP085,
 * HMC5883L, and L3G4200D; 3-axis acceleratometer, thermometer,
 * barometer, 3-axis compass and 3-axis gyroscope with output to LCD.
 *
 * @section Circuit
 * The GY-80 10DOF module with pull-up resistors (4K7) for TWI signals
 * and 3V3 internal voltage converter. For LCD wiring see HD44780.hh.
 * @code
 *                           GY-80
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 *                     2-|3V3         |
 * (GND)---------------3-|GND         |
 * (A5/SCL)------------4-|SCL         |
 * (A4/SDA)------------5-|SDA         |
 *                     6-|M-DRDY      |
 *                     7-|A-INT1      |
 *                     8-|T-INT1      |
 *                     9-|P-XCLR      |
 *                    10-|P-EOC       |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/AnalogPin.hh"

#include <HD44780.h>
#include <ADXL345.h>
#include <BMP085.h>
#include <HMC5883L.h>
#include <L3G4200D.h>
#include <DS1307.h>

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

// Select LCD width and height
#define WIDTH 16
// #define WIDTH 20
// #define HEIGHT 2
#define HEIGHT 4
HD44780 lcd(&port, WIDTH, HEIGHT);

// Digital acceleratometer with alternative address
ADXL345 acceleratometer(1);

// Digital temperature and pressure sensor
BMP085 bmp;

// The 3-Axis Digital Compass
HMC5883L compass;

// Digital Gyroscope using alternative address
L3G4200D gyroscope(1);

// Real-time clock
DS1307 rtc;

void setup()
{
  // Start the watchdog ticks
  Watchdog::begin();

  // Start trace output stream on the LCD
  lcd.begin();
  trace.begin(&lcd, PSTR("CosaLCD10DOF"));
  sleep(2);

  // Start the sensors
  acceleratometer.begin();
  bmp.begin(BMP085::ULTRA_LOW_POWER);
  compass.output_rate(HMC5883L::OUTPUT_RATE_3_HZ);
  compass.samples_avg(HMC5883L::SAMPLES_AVG_8);
  compass.range(HMC5883L::RANGE_4_0_GA);
  compass.mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE);
  compass.begin();
  gyroscope.begin();
}

void loop()
{
  // Read the time from the rtc device and print
  time_t now;
  rtc.get_time(now);
  now.to_binary();
  trace << clear;
  trace << PSTR("RTC; ") << now;

  // Read battery
#if (HEIGHT == 2)
  sleep(2);
  trace << clear;
#else
  trace << endl;
#endif
  trace << PSTR("VCC: ") << AnalogPin::bandgap() << PSTR(" mV") << endl;

  // Free memory
  trace << PSTR("MEM: ") << free_memory() << PSTR(" bytes");
  sleep(2);

  // Read barometer; pressure and temperature
  bmp.sample();
  trace << clear;
  trace << PSTR("Barometer:") << endl;
  trace << bmp.pressure() << PSTR(" Pa, ");
  trace << (bmp.temperature() + 5) / 10 << PSTR(" C");
#if (HEIGHT == 2)
  sleep(2);
  trace << clear;
#else
  trace << endl;
#endif

  // Read compass heading
  compass.read_heading();
  compass.to_milli_gauss();
  HMC5883L::data_t dir;
  compass.heading(dir);
  trace << PSTR("Compass:") << endl;
  trace << dir.x << PSTR(", ")
	<< dir.y << PSTR(", ")
	<< dir.z;
  sleep(2);

  // Read acceleration
  trace << clear;
  ADXL345::sample_t acc;
  acceleratometer.sample(acc);
  trace << PSTR("Accelerometer:") << endl;
  trace << acc.x << PSTR(", ")
	<< acc.y << PSTR(", ")
	<< acc.z;

  // Read gyroscope
#if (HEIGHT == 2)
  sleep(2);
  trace << clear;
#else
  trace << endl;
#endif
  L3G4200D::sample_t rate;
  gyroscope.sample(rate);
  trace << PSTR("Gyroscope:") << endl;
  trace << rate.x << PSTR(", ")
	<< rate.y << PSTR(", ")
	<< rate.z;
  sleep(2);
}
