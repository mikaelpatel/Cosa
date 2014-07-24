/**
 * @file CosaMAX72XXMATRIX.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Math.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/LCD/Driver/MAX72XXMATRIX.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Canvas/OffScreen.hh"

// Select MAX72XX IO Adapter; Serial Output Pins or SPI
//LCD::Serial3W port;
LCD::SPI3W port;
MAX72XXMATRIX lcd(&port,2);

//// Create an iostream and bind the LCD
IOStream cout(&lcd);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart,PSTR("MAX72XXMATRIX"));
  // Initiate watchdog and lcd
  Watchdog::begin();
  lcd.begin();
  OffScreen<16, 8> offscreen;
  offscreen.begin();
  offscreen.draw_rect(6, 0, 2, 2);
  lcd.set_cursor(0, 0);
  lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
  sleep(3);
}

void loop()
{
  cout << clear << 'C' << 12;
  static uint8_t channel = 0;

  // Sample analog channel and display voltage
  float volt = (5.0 * AnalogPin::sample((Board::AnalogPin) channel)) / 1023;
  //cout << clear << 'A' << channel << ' ' << volt;
  cout << clear << volt;
  sleep(1);

  // Step to next channel
  channel += 1;
  if (channel == 6) channel = 0;

  // Draw a random number (1..100)
  int8_t saved = cout.precision(7);
  cout << clear << 1.0 / (rand(100U) + 1);
  cout.precision(saved);
  sleep(2);
}
