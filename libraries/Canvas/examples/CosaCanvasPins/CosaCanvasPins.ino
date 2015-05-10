/**
 * @file CosaCanvasPins.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa demonstration of Canvas device driver for ST7735, 262K Color
 * Single-Chip TFT Controller, and monitoring of Arduino pins.
 * Shows binding to IOStream::Device onto the display (and not the UART).
 *
 * @section Circuit
 * Reads digital pins(D0..D7) and analog pins(A0..A7).
 * @code
 *
 *                           ST7735
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                      -|            |
 * (RST)---------------6-|RESET       |
 * (D9)----------------7-|A0          |
 * (MOSI/D11)----------8-|SDA         |
 * (SCK/D13)-----------9-|SCK         |
 * (SS/D10)-----------10-|CS          |
 *                      -|            |
 * (VCC)----[330]-----15-|LED+        |
 * (GND)--------------16-|LED-        |
 *                       +------------+
 *
 * (A0..A7)------------->
 * (A0..D7)------------->
 *
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <GDDRAM.h>
#include <ST7735.h>

#include <Canvas.h>
#include "Canvas/Element/Textbox.hh"

#include "Cosa/InputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"

ST7735 tft;
Textbox textbox(&tft);
IOStream cout(&textbox);

void setup()
{
  // Start the watchdogy for delay
  Watchdog::begin();

  // Initiate the display and clear the screen
  tft.begin();
  tft.set_pen_color(Canvas::WHITE);
  tft.fill_screen();
}

void loop()
{
  // Display digital and analog pin values (A/D0..7)
  for (uint8_t x = 0, y = 2; y < tft.HEIGHT; y += 20, x++) {
    tft.set_pen_color(tft.shade(Canvas::WHITE, 75));
    tft.fill_rect(10, y, tft.WIDTH - 20, 16);
    tft.set_pen_color(Canvas::BLACK);
    tft.draw_rect(10, y, tft.WIDTH - 20, 16);
    textbox.set_cursor(15, y + 5);
    cout.printf(PSTR("D%d"), x);
    tft.set_pen_color(InputPin::read((Board::DigitalPin) x) ?
		      Canvas::RED : Canvas::GREEN);
    tft.fill_circle(35, y + 8, 5);
    textbox.set_cursor(55, y + 5);
    cout.printf(PSTR("A%d %d mV"), x,
		(AnalogPin::sample((Board::AnalogPin) x) * 500L) / 1024);
  }
  sleep(1);
}
