/**
 * @file CosaCanvasClock.ino
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
 * Cosa demonstration of Canvas Segment Font handling and device 
 * driver for ST7735R, 262K Color Single-Chip TFT Controller.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/SPI/Driver/ST7735R.hh"
#include "Cosa/Canvas/Font/Segment32x50.hh"

ST7735R tft;

void setup()
{
  // Start the watchdog and initiate the display
  Watchdog::begin();
  tft.begin();
  tft.set_canvas_color(Canvas::BLACK);
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.set_text_font(&segment32x50);
  tft.set_text_color(tft.shade(Canvas::RED, 75));
  tft.set_pen_color(tft.shade(Canvas::RED, 75));
  tft.fill_screen();
}

void loop()
{
  SLEEP(1);
  static uint8_t min = 30;
  static uint8_t sec = 00;

  tft.fill_screen();
  tft.set_cursor(8, 30);
  tft.draw_char('0' + min/10);
  tft.draw_char('0' + min%10);
  tft.move_cursor(-12, 0);
  tft.draw_char('9' + 1);
  tft.move_cursor(-8, 0);
  tft.draw_char('0' + sec/10);
  tft.draw_char('0' + sec%10);

  if (sec == 0) {
    if (min != 00) {
      sec = 59;
      min -= 1;
    }
  } else
    sec -= 1;
}
