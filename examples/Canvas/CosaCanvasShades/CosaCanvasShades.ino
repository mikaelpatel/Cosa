/**
 * @file CosaCanvasShades.ino
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
 * Cosa demonstration of Cosa Canvas color shades on ST7735, 
 * 262K Color Single-Chip TFT Controller. 
 *
 * @section Circuit
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"

#define USE_TFT_ST7735
//#define USE_TFT_ILI9341

#if defined(USE_TFT_ST7735)
#include "Cosa/Canvas/Driver/ST7735.hh"
ST7735 tft;
#endif

#if defined(USE_TFT_ILI9341)
#include "Cosa/Canvas/Driver/ILI9341.hh"
ILI9341 tft;
#endif

void setup()
{
  Watchdog::begin();
  tft.begin();
  tft.set_canvas_color(tft.BLACK);
  tft.fill_screen();
}

void draw_shade(Canvas::color16_t color)
{
  Canvas::color16_t base = color;
  for (uint16_t x = 75; x > 0; x -= 25) {
    tft.set_pen_color(tft.WHITE);
    tft.draw_rect(9, 9, tft.WIDTH - 19, tft.HEIGHT - 19);
    for (uint16_t y = 10; y < tft.HEIGHT - 10; y += 4) {
      uint16_t level = ((y - 10) * 100L) / (tft.HEIGHT - 10);
      tft.set_pen_color(tft.shade(color, level));
      tft.fill_rect(10, y, tft.WIDTH - 20, 4);
    }
    sleep(1);
    color = tft.shade(base, x);
  }
}

void loop()
{
  draw_shade(tft.WHITE);
  draw_shade(tft.RED);
  draw_shade(tft.GREEN);
  draw_shade(tft.BLUE);
  draw_shade(tft.YELLOW);
  draw_shade(tft.CYAN);
  draw_shade(tft.MAGENTA);
}
