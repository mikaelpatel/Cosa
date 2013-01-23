/**
 * @file CosaCanvasPins.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa demonstration of Canvas device driver for ST7735R, 262K Color 
 * Single-Chip TFT Controller, and monitoring of Arduino pins.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/SPI/Driver/ST7735R.hh"
#include "Cosa/Canvas/Element/Textbox.hh"

// The display and an iostream to the device
ST7735R tft;
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
    cout.printf_P(PSTR("D%d"), x);
    tft.set_pen_color(InputPin::read(x) ? Canvas::RED : Canvas::GREEN);
    tft.fill_circle(35, y + 8, 5);
    textbox.set_cursor(55, y + 5);
    cout.printf_P(PSTR("A%d %d mV"), x, (AnalogPin::sample(x) * 500L) / 1024);
  }
  SLEEP(1);
}
