/**
 * @file CosaFont.ino
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
 * Cosa demonstration of Canvas/Font and device driver for ST7735R, 
 * 262K Color Single-Chip TFT Controller.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/SPI/ST7735R.hh"

ST7735R tft;

void setup()
{
  Watchdog::begin();
  tft.begin();
}

void loop()
{
  tft.set_pen_color(tft.WHITE);
  tft.fill_screen();
  tft.set_scale(1);
  tft.set_pen_color(tft.BLACK);
  tft.set_text_color(tft.red(75));
  char c = 0;
  for (uint8_t x = 3; x < tft.SCREEN_WIDTH - 12; x += 12) {
    for (uint8_t y = 2; y < tft.SCREEN_HEIGHT - 12; y += 12) {
      tft.set_pen_color(tft.grayscale(75));
      tft.fill_rect(x, y, 12, 12);
      tft.set_pen_color(tft.BLACK);
      tft.draw_rect(x, y, 12, 12);
      tft.set_cursor(x + 3, y + 3);
      tft.draw_char(c++);
    }
  }
  Watchdog::delay(2048);

  tft.set_scale(16);
  tft.set_pen_color(tft.grayscale(75));
  tft.fill_rect(3, 2, tft.SCREEN_WIDTH - 6, tft.SCREEN_HEIGHT - 4);
  tft.set_pen_color(tft.BLACK);
  tft.draw_rect(3, 2, tft.SCREEN_WIDTH - 6, tft.SCREEN_HEIGHT - 4);
  tft.set_pen_color(tft.grayscale(75));
  for (uint8_t c = 0; c < 128; c++) {
    tft.fill_rect(tft.SCREEN_WIDTH/2 - 40, tft.SCREEN_HEIGHT/2 - 63, 83, 131);
    tft.set_text_color(tft.red(50));
    tft.set_cursor(tft.SCREEN_WIDTH/2 - 37, tft.SCREEN_HEIGHT/2 - 63);
    tft.draw_char(c);
    tft.set_text_color(tft.red(75));
    tft.set_cursor(tft.SCREEN_WIDTH/2 - 40, tft.SCREEN_HEIGHT/2 - 60);
    tft.draw_char(c);
    Watchdog::delay(512);
  }
}
