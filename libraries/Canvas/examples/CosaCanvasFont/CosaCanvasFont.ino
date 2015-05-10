/**
 * @file CosaCanvasFont.ino
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
 * Cosa demonstration of Canvas Font handling and device driver for
 * ST7735 or ILI9341.
 *
 * @section Circuit
 * @code
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
 *                           ILI9341
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST)---------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 * (MISO/D12)----------9-|SDO         |
 *                       +------------+
 *
 *                           ILI9163
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST)---------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 *                       +------------+
 * @endcode
 *
 * Note: ILI9341 and ILI9163 signals are 3V3.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"

#include <Canvas.h>

//#define USE_TFT_ST7735
//#include <GDDRAM.h>
//#include <ST7735.h>
//ST7735 tft;

//#define USE_TFT_ILI9341
//#include <GDDRAM.h>
//#include <ILI9341.h>
//ILI9341 tft;

#define USE_TFT_ILI9163
#include <GDDRAM.h>
#include <ILI9163.h>
ILI9163 tft;

void setup()
{
  // Start the watchdog and initiate the display
  Watchdog::begin();
  tft.begin();
}

void loop()
{
  // Draw the font table
  tft.set_pen_color(Canvas::WHITE);
  tft.fill_screen();
  tft.set_text_scale(1);
  tft.set_pen_color(Canvas::BLACK);
  tft.set_text_color(tft.shade(Canvas::RED, 75));
  char c = 0;
  for (uint16_t x = 3; x < tft.WIDTH - 12; x += 12) {
    for (uint16_t y = 2; y < tft.HEIGHT - 12; y += 12) {
      tft.set_pen_color(tft.shade(Canvas::WHITE, 75));
      tft.fill_rect(x, y, 12, 12);
      tft.set_pen_color(Canvas::BLACK);
      tft.draw_rect(x, y, 12, 12);
      tft.set_cursor(x + 3, y + 3);
      tft.draw_char(c++);
    }
  }
  sleep(4);

  // Draw each character in the font table
  tft.set_text_scale(16);
  tft.set_pen_color(tft.shade(Canvas::WHITE, 75));
  tft.fill_rect(3, 2, tft.WIDTH - 6, tft.HEIGHT - 4);
  tft.set_pen_color(Canvas::BLACK);
  tft.draw_rect(3, 2, tft.WIDTH - 6, tft.HEIGHT - 4);
  tft.set_pen_color(tft.shade(Canvas::WHITE, 75));
  for (uint8_t c = 0; c < 128; c++) {
    tft.fill_rect(tft.WIDTH/2 - 40, tft.HEIGHT/2 - 63, 83, 131);
    tft.set_text_color(tft.shade(Canvas::RED, 50));
    tft.set_cursor(tft.WIDTH/2 - 37, tft.HEIGHT/2 - 60);
    tft.draw_char(c);
    tft.set_text_color(tft.shade(Canvas::RED, 75));
    tft.set_cursor(tft.WIDTH/2 - 40, tft.HEIGHT/2 - 63);
    tft.draw_char(c);
    sleep(1);
  }
}
