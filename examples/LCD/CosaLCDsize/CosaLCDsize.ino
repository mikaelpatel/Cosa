/**
 * @file CosaLCDsize.ino
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
 * Demonstration of the LCD device drivers build size for "Hello world"
 * type of sketch.
 * 
 * @section Circuit
 * No circuit as this is a build size demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"

// Binary sketch size:  MEGA/STD*/TINY bytes
// =========================================
// Baseline:		 784/ 660/ 562 bytes
// HD44780::Port4b:	3552/3386/3244 bytes
// HD44780::SR3W:	3890/3724/3576 bytes
// HD44780::SR3WSPI:	4422/4254/4018 bytes
// HD44780::SR4W:	3888/3710/3554 bytes
// HD44780::MJKDZ:	5234/5152/5656 bytes
// HD44780::GYIICLCD:	5234/5152/5656 bytes
// HD44780::DFRobot:	5204/5122/5626 bytes
// HD44780::ERM1602_5:	4182/4014/3844 bytes
// PCD8544: 		4378/4246/3924 bytes
// ST7565:  		4656/4540/4220 bytes
// VLCD:		4066/4010/4400 bytes
// =========================================
// The baseline is the Watchdog and Arduino
// init code. Move comment prefix to compile 
// different LCD drivers and compare size.
// =========================================

#include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780::Port4p port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::MJKDZ port;
HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780::ERM1602_5 port;
HD44780 lcd(&port);

// #include "Cosa/LCD/Driver/PCD8544.hh"
// PCD8544 lcd;

// #include "Cosa/LCD/Driver/ST7565.hh"
// ST7565 lcd;

// #include "Cosa/LCD/Driver/VLCD.hh"
// VLCD lcd;

void setup()
{
  Watchdog::begin();
  lcd.begin();
  lcd.puts_P(PSTR("Hello World!"));
}

void loop()
{
}
