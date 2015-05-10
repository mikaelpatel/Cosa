/**
 * @file CosaCanvasSweep.ino
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
 * Single-Chip TFT Controller, and monitoring of an analog pin in
 * EKG style.
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
 * (A0)----------------->
 *
 * Note: ILI9341 and ILI9163 signals are 3V3.
 *
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Canvas.h>

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"

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

AnalogPin probe(Board::A0);
Canvas::color16_t CANVAS, PEN, CARET;

void setup()
{
  // Start the watchdogy for delay
  Watchdog::begin();

  // Initiate the display, setup colors and clear the screen
  tft.begin();
  PEN = tft.shade(Canvas::GREEN, 50);
  CANVAS = tft.shade(Canvas::WHITE, 10);
  CARET = tft.shade(Canvas::YELLOW, 50);
  tft.set_pen_color(PEN);
  tft.set_text_color(CARET);
  tft.set_canvas_color(CANVAS);
  tft.fill_screen();
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.draw_rect(0, 0, tft.WIDTH - 1, tft.HEIGHT - 1);
}

void loop()
{
  const uint8_t STEP = 8;
  static uint16_t x0 = 0;
  static uint16_t y0 = 0;

  // Sample the probe and calculate the position
  uint16_t sample = probe.sample();
  uint16_t x1 = x0 + STEP;
  uint16_t y1 = tft.HEIGHT - (sample >> 3);

  // Calculate the region to erase before drawing the line
  uint16_t width = STEP * 2;
  if (x0 + 2 + width > tft.WIDTH)
    width = tft.WIDTH - x0 - 2;
  tft.set_pen_color(CANVAS);
  tft.fill_rect(x0 + 1, 1, width, tft.HEIGHT - 2);

  // Draw the line and a simple marker to follow
  tft.set_pen_color(PEN);
  tft.draw_line(x0, y0, x1, y1);
  tft.set_cursor(x1 + 1, y1 - 3);
  tft.draw_char('*');

  // Update position for the next sample
  x0 = x1;
  y0 = y1;

  // Check for wrap-around
  if (x0 >= tft.WIDTH) {
    x0 = 0;
    tft.draw_rect(0, 0, tft.WIDTH - 1, tft.HEIGHT - 1);
  }

  // Wait for the next sample, 8 samples per second
  Watchdog::delay(128);
}
