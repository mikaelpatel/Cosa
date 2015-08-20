/**
 * @file CosaLCDtemp.ino
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
 * Cosa LCD demo with 1-wire digital thermometer (DS18B20).
 *
 * @section Circuit
 * See LCD driver header files for circuit descriptions.
 * @code
 *                       DS18B20/sensor
 * (VCC)--[4K7]--+       +------------+
 * (GND)---------)-----1-|GND         |\
 * (D4)----------+-----2-|DQ          | |
 * (VCC/GND)-----------3-|VDD         |/
 *                       +------------+
 * @endcode
 *
 * Connect Arduino to DS18B20 in D4 and GND. May use parasite
 * powering (connect DS18B20 VDD to GND) otherwise to VCC.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/IOStream.hh"

// Digital Thermometer
#include <OWI.h>
#include <DS18B20.h>

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

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
HD44780 lcd(&port, 16, 4);
// HD44780 lcd(&port);

// #include <HD44780.h>
// #include <ST7920.h>
// HD44780::Port4b port;
// ST7920 lcd(&port);

// #include <PCD8544.h>
// LCD::Serial3W port;
// LCD::SPI3W port;
// PCD8544 lcd(&port);

// #include <ST7565.h>
// LCD::Serial3W port;
// LCD::SPI3W port;
// ST7565 lcd(&port);

// #include <VLCD.h>
// VLCD lcd;

// Use the LCD as an IOStream device for the console
IOStream console(&lcd);

// The 1-Wire bus and the connected digital thermometer
OWI owi(Board::D4);
DS18B20 sensor(&owi);

void setup()
{
  Watchdog::begin();
  lcd.begin();
  console << PSTR("CosaLCDtemp: started");
  sensor.connect(0);
  sleep(2);
}

void loop()
{
  // Request temperature conversion and read result
  sensor.convert_request();
  uint16_t vcc = AnalogPin::bandgap();
  sensor.read_scratchpad();

  // Display current temperature and battery status
  console << clear;
  console << PSTR("sensor:  ") << sensor << PSTR(" C") << endl;
  console << PSTR("battery: ") << vcc << PSTR(" mV");
  sleep(2);
}
