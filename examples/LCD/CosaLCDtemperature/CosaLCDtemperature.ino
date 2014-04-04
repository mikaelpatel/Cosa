/**
 * @file CosaLCDtermperature.ino
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
 * Cosa LCD demo with 1-wire temperature
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/LCD/Driver/PCD8544.hh"

PCD8544 lcd;

// #include "Cosa/LCD/Driver/HD44780.hh"
// HD44780::Port4b port;
// HD44780 lcd(&port);

IOStream console(&lcd);
#if defined(__ARDUINO_TINY__)
OWI owi(Board::D4);
#else
OWI owi(Board::D11);
#endif
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
