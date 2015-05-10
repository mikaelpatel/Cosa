/**
 * @file CosaCanvasClock.ino
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
 * A simple count down clock; Cosa demonstration of Canvas Segment
 * Font handling and device driver for ST7735, ILI9314 or ILI9163.
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
 *
 * Note: ILI9341 and ILI9163 signals are 3V3.
 *
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"

#include <Canvas.h>
#include <Font.h>
#include "Segment32x50.hh"

#define USE_TFT_ST7735
#include <GDDRAM.h>
#include <ST7735.h>
ST7735 tft;

//#define USE_TFT_ILI9341
//#include <GDDRAM.h>
//#include <ILI9341.h>
//ILI9341 tft;

//#define USE_TFT_ILI9163
//#include <GDDRAM.h>
//#include <ILI9163.h>
//ILI9163 tft;

const Canvas::color16_t BACKGROUND = tft.shade(Canvas::RED, 25);
const Canvas::color16_t TEXT_COLOR = tft.shade(Canvas::RED, 75);
uint8_t min = 30;
uint8_t sec = 00;

void setup()
{
  // Start the watchdog for low power sleep and 1 second ticks
  Power::set(SLEEP_MODE_PWR_DOWN);
  Watchdog::begin(1024);

  // Initiate the display and set orientation and color scheme
  tft.begin();
  tft.set_canvas_color(Canvas::BLACK);
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.set_text_font(&segment32x50);
  tft.set_text_color(TEXT_COLOR);
  tft.fill_screen();

  // Draw a filled boundary box using the round rectangle
  Canvas::color16_t saved = tft.set_pen_color(BACKGROUND);
  tft.fill_roundrect(0, 20, tft.WIDTH - 1, 70, 12);
  tft.set_pen_color(TEXT_COLOR);
  tft.draw_roundrect(0, 20, tft.WIDTH - 1, 70, 12);
  tft.set_pen_color(saved);

  // Draw initial time
  tft.set_cursor(8, 30);
  tft.draw_char('0' + min/10);
  tft.draw_char('0' + min%10);
  tft.move_cursor(-12, 0);
  tft.draw_char(':');
  tft.move_cursor(-8, 0);
  tft.draw_char('0' + sec/10);
  tft.draw_char('0' + sec%10);
}

void draw_digit(uint8_t pos, uint8_t digit)
{
  // Set digit position on screen
  uint8_t adjust = (pos < 2 ? 0 : segment32x50.WIDTH - 17);
  tft.set_cursor(8 + (pos * segment32x50.WIDTH) + adjust, 30);

  // Fill the background
  Canvas::color16_t saved = tft.set_pen_color(BACKGROUND);
  uint16_t x, y;
  tft.get_cursor(x, y);
  tft.fill_rect(x, y, segment32x50.WIDTH, segment32x50.HEIGHT);
  tft.set_pen_color(saved);

  // And draw the digit
  tft.draw_char('0' + digit);
}

void loop()
{
  // Wait for the next watchdog tick; power down while waiting
  Watchdog::await();

  // Decrement counter
  if (sec == 0) {
    if (min != 00) {
      sec = 59;
      min -= 1;
    }
  } else {
    sec -= 1;
  }

  // Draw only the digits that are updated (to avoid screen flicker)
  draw_digit(3, sec%10);
  if ((sec%10) != 9) return;
  draw_digit(2, sec/10);
  if ((sec/10) != 5) return;
  draw_digit(1, min%10);
  if ((min%10) != 9) return;
  draw_digit(0, min/10);
}
