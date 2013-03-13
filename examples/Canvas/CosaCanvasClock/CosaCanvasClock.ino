/**
 * @file CosaCanvasClock.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Cosa demonstration of Canvas Segment Font handling and device 
 * driver for ST7735R, 262K Color Single-Chip TFT Controller.
 *
 * @section Circuit
 * Connect Arduino to ST7735R Module (Arduino ==> HY-1.8 SPI):
 * GND ==> GND(1), VCC(5V) ==> VCC(2), RST ==> RESET(6),
 * D9 ==> A0(7), MOSI/D11 ==> SDA(8), SCK/D13 ==> SCK(9),
 * SS/D10 ==> CS(10), VCC(5V) ==> LED+(15), GND ==> LED-(16)    
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/SPI/Driver/ST7735R.hh"
#include "Cosa/Canvas/Font/Segment32x50.hh"

ST7735R tft;

void setup()
{
  // Start the watchdog for low power sleep
  Watchdog::begin();

  // Initiate the display and set orientation and color scheme
  tft.begin();
  tft.set_canvas_color(Canvas::BLACK);
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.set_text_font(&segment32x50);
  tft.set_text_color(tft.shade(Canvas::RED, 75));
  tft.fill_screen();
}

void loop()
{
  SLEEP(1);
  static uint8_t min = 30;
  static uint8_t sec = 00;

  // Draw a filled boundary box using the round rectangle
  Canvas::color16_t saved = tft.set_pen_color(tft.shade(Canvas::RED, 25));
  tft.fill_roundrect(0, 20, tft.WIDTH - 1, 70, 12);
  tft.set_pen_color(tft.get_text_color());
  tft.draw_roundrect(0, 20, tft.WIDTH - 1, 70, 12);
  tft.set_pen_color(saved);

  // Draw the current counter value
  tft.set_cursor(8, 30);
  tft.draw_char('0' + min/10);
  tft.draw_char('0' + min%10);
  tft.move_cursor(-12, 0);
  tft.draw_char('9' + 1);
  tft.move_cursor(-8, 0);
  tft.draw_char('0' + sec/10);
  tft.draw_char('0' + sec%10);

  // Decrement counter
  if (sec == 0) {
    if (min != 00) {
      sec = 59;
      min -= 1;
    }
  } else
    sec -= 1;
}
