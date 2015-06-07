/**
 * @file CosaLCDsize.ino
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
 * Demonstration of the LCD device drivers build size for "Hello world"
 * type of sketch.
 *
 * @section Circuit
 * No circuit as this is a build size demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"

// AVR Tools: 3.4.3, GCC: 4.3.1
// Binary sketch size:  MEGA/STD*/TINY bytes
// =========================================
// Baseline:		 954/ 840/ 994 bytes
// HD44780::Port4b:	3780/3638/3726 bytes
// HD44780::SR3W:	3692/3544/3628 bytes
// HD44780::SR3WSPI:	4240/4026/3964 bytes
// HD44780::SR4W:	3586/3442/3532 bytes
// HD44780::MJKDZ:	5058/5002/5652 bytes
// HD44780::GYIICLCD:	5058/5002/5652 bytes
// HD44780::DFRobot:	5032/4976/5626 bytes
// HD44780::ERM1602_5:	3866/3732/3812 bytes
// PCD8544::Serial3W:	4660/4442/4386 bytes
// PCD8544::SPI3W:	5198/4906/4744 bytes
// ST7565::Serial3W:	4834/4628/4562 bytes
// ST7565::SPI3W:	5374/5092/4920 bytes
// VLCD:		3958/3904/4520 bytes
// =========================================
// The baseline is the Watchdog and Arduino
// init code. Move comment prefix to compile
// different LCD drivers and compare size.
// =========================================

// Select port type to use with the LCD device driver.
// LCD and communication port
#include <HD44780.h>

// HD44780 driver built-in adapters
HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;

// I2C expander io port based adapters
// #include <PCF8574.h>
// #include <MJKDZ_LCD_Module.h>
// MJKDZ_LCD_Module port;
// MJKDZ_LCD_Module port(0);
// #include <GY_IICLCD.h>
// GY_IICLCD port;
// #include <DFRobot_IIC_LCD_Module.h>
// DFRobot_IIC_LCD_Module port;
// #include <SainSmart_LCD2004.h>
// SainSmart_LCD2004 port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);

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

void setup()
{
  Watchdog::begin();
  lcd.begin();
  lcd.puts(PSTR("Hello World!"));
}

void loop()
{
}
