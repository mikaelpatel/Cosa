/**
 * @file CosaLCDtermperature.ino
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
 * Cosa LCD demo with 1-wire temperature
 * 
 * @section Circuit
 * See LCD driver header files for circuit descriptions.
 *
 *                       DS18B20/sensor
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (D5)------+---------2-|DQ          |
 *           |       +-3-|VDD         |
 *          4K7      |   +------------+
 *           |       | 
 * (VCC)-----+       +---(VCC/GND)
 *
 * Connect Arduino to DS18B20 in D5 and GND. May use parasite 
 * powering (connect DS18B20 VCC to GND) otherwise to VCC.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"

// Select the LCD device for the sketch
#include "Cosa/LCD/Driver/PCD8544.hh"
PCD8544 lcd;
// #include "Cosa/LCD/Driver/ST7565.hh"
// ST7565 lcd;
// #include "Cosa/LCD/Driver/VLCD.hh"
// VLCD lcd;
// #include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::ERM1602_5 port;
// HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780 lcd(&port);

IOStream console(&lcd);
OWI owi(Board::D5);
DS18B20 sensor(&owi);

void setup()
{
  Watchdog::begin();
  lcd.begin();
  sensor.connect(0);
}

void loop()
{
  sensor.convert_request();
  sensor.read_scratchpad();
  console << clear << sensor << PSTR(" C") << endl;
  console << AnalogPin::bandgap() << PSTR(" mV");
  SLEEP(2);
}
