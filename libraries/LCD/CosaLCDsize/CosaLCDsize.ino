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

// GCC: 4.8.1
// Binary sketch size:  MEGA/STD*/TINY bytes
// =========================================
// Baseline:		 954/ 596/ 514 bytes
// HD44780::Port4b:	3984/3780/3674 bytes
// HD44780::SR3W:	3858/3654/3540 bytes
// HD44780::SR3WSPI:	3890/3752/3716 bytes
// HD44780::SR4W:	3708/3504/3398 bytes
// -----------------------------------------
// HD44780::MJKDZ:	5358/5200/5542 bytes
// HD44780::GYIICLCD:	5380/5222/5564 bytes
// HD44780::DFRobot:	5328/5170/5512 bytes
// HD44780::Sainsmart:	5328/5170/5512 bytes
// HD44780::Adafruit:	5378/5220/5834 bytes
// -----------------------------------------
// HD44780::ERM1602_5:	3966/3758/3652 bytes
// =========================================
// PCD8544::Serial3W:	5314/5110/4760 bytes
// PCD8544::SPI3W:	5334/5128/4964 bytes
// =========================================
// ST7565::Serial3W:	4834/4628/4906 bytes
// ST7565::SPI3W:	5480/5274/5510 bytes
// =========================================
// VLCD:		4394/4236/4844 bytes
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

// I2C expander(PCF8574) io port based adapters
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

// I2C expander(MCP23008) io port based adapters
// #include <MCP23008.h>
// #include <Adafruit_I2C_LCD_Backpack.h>
// Adafruit_I2C_LCD_Backpack port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);

// #include <Canvas.h>
// #include <PCD8544.h>
// LCD::Serial3W port;
// LCD::SPI3W port;
// PCD8544 lcd(&port);

// #include <Canvas.h>
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
