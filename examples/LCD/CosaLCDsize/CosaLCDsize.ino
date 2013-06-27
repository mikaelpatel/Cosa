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
 * No circuit as this is a build demonstration.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"

// Binary sketch size:  MEGA/STD*/TINY bytes
// =========================================
// Baseline:		1118/916/700   bytes
// HD44780:		3312/3088/2868 bytes
// HD44780::Port:	3544/3318/3100 bytes
// HD44780::MJKDZ:	4484/4342/NA   bytes
// HD44780::DFRobot:	4474/4332/NA   bytes
// PCD8544: 		4482/4260/3916 bytes
// ST7565:  		5008/4802/4460 bytes
// =========================================
// The baseline is the Watchdog and Arduino
// init code. Move comment prefix to compile 
// different LCD drivers and compare size.
// =========================================

#include "Cosa/IOStream/Driver/HD44780.hh"

HD44780::Port port;
// HD44780::MJKDZ port;
// HD44780::DFRobot port;
HD44780 lcd(&port);

// #include "Cosa/IOStream/Driver/PCD8544.hh"
// PCD8544 lcd;

// #include "Cosa/IOStream/Driver/ST7565.hh"
// ST7565 lcd;

void setup()
{
  Watchdog::begin();
  lcd.begin();
  lcd.puts_P(PSTR("Hello World!"));
}

void loop()
{
}
