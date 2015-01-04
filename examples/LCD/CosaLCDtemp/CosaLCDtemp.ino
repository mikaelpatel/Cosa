/**
 * @file CosaLCDtemp.ino
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
 * Cosa LCD demo with 1-wire digital thermometer (DS18B20).
 * 
 * @section Circuit
 * See LCD driver header files for circuit descriptions.
 *
 *                       DS18B20/sensor
 * (VCC)--[4K7]--+       +------------+
 * (GND)---------)-----1-|GND         |\
 * (D4)----------+-----2-|DQ          | |
 * (VCC/GND)-----------3-|VDD         |/
 *                       +------------+
 *
 * Connect Arduino to DS18B20 in D4 and GND. May use parasite 
 * powering (connect DS18B20 VDD to GND) otherwise to VCC.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"

// Select the LCD device for the sketch
#include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780::SR3W port;
HD44780::SR3WSPI port;
// HD44780::SR4W port;
// HD44780::ERM1602_5 port;
// HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780::SainSmart port;
HD44780 lcd(&port);

// #include "Cosa/LCD/Driver/PCD8544.hh"
// LCD::Serial3W port;
// LCD::SPI3W port;
// PCD8544 lcd(&port);

// #include "Cosa/LCD/Driver/ST7565.hh"
// ST7565 lcd;

// #include "Cosa/LCD/Driver/VLCD.hh"
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
