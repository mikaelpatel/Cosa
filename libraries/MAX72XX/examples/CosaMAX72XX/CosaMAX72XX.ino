/**
 * @file CosaMAX72XX.ino
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
 * Demonstration of the MAX72XX device driver with mapping to
 * IOStream::Device.
 */

#include <MAX72XX.h>

#include "Cosa/Types.h"
#include "Cosa/Math.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"

// Select MAX72XX IO Adapter; Serial Output Pins or SPI
// LCD::Serial3W port;
LCD::SPI3W port;
MAX72XX lcd(&port);

// Create an iostream and bind the LCD
IOStream cout(&lcd);

void setup()
{
  // Initiate watchdog and lcd
  Watchdog::begin();
  lcd.begin();
  cout << PSTR("CHE COSA.");
  for (uint8_t i = 0; i < 16; i++) {
    lcd.display_contrast(i);
    lcd.display_off();
    delay(100);
    lcd.display_on();
    delay(100);
  }
  lcd.display_clear();
  lcd.display_contrast(7);

  // Display the font table
  for (char c = ' '; c < 0x7f; c++) {
    cout << c;
    delay(250);
  }
}

void loop()
{
  static uint8_t channel = 0;

  // Sample analog channel and display voltage
  float volt = (5.0 * AnalogPin::sample((Board::AnalogPin) channel)) / 1023;
  cout << clear << 'A' << channel << ' ' << volt;
  sleep(1);

  // Step to next channel
  channel += 1;
  if (channel == 6) channel = 0;

  // Draw a random number (1..100)
  int8_t saved = cout.precision(7);
  cout << clear << 1.0 / (rand(100U) + 1);
  cout.precision(saved);
  sleep(1);
}
