/**
 * @file CosaCanvasHistogram.ino
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
 * Single-Chip TFT Controller, and monitoring of analog pins as
 * a histogram.
 *
 * @section Circuit
 * Use the analog pin(0..MAX-1) as the probe pins.
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
 * (A0)----------------->
 * (A1)----------------->
 * (A2)----------------->
 * (A3)----------------->
 * (A4)----------------->
 * (A5)----------------->
 *
 * Note: ILI9341 signals are 3V3.
 *
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#define USE_TFT_ST7735
#include <GDDRAM.h>
#include <ST7735.h>
ST7735 tft;

//#define USE_TFT_ILI9341
//#include <GDDRAM.h>
//#include <ILI9341.h>
//ILI9341 tft;

#include <Canvas.h>
#include "Canvas/Element/Textbox.hh"

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"

Textbox textbox(&tft);
IOStream cout(&textbox);
Canvas::color16_t CANVAS, PEN;

void setup()
{
  // Start the watchdogy for delay between sample suite
  Watchdog::begin();

  // Initiate the display, setup colors and clear the screen
  tft.begin();
  PEN = tft.shade(Canvas::GREEN, 50);
  CANVAS = tft.shade(Canvas::WHITE, 10);
  tft.set_pen_color(PEN);
  tft.set_canvas_color(CANVAS);
  tft.fill_screen();
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.draw_rect(0, 0, tft.WIDTH - 1, tft.HEIGHT - 1);
  textbox.set_text_color(PEN);
  textbox.set_text_port(0, 0, tft.WIDTH, tft.HEIGHT);
}

void loop()
{
  const uint8_t ANALOG_PIN_MAX = 6;
  for (uint8_t ix = 0; ix < ANALOG_PIN_MAX; ix++) {
    static uint16_t sample[ANALOG_PIN_MAX];
    if (sample[ix] < 20) sample[ix] = 20;
    uint16_t x = 6 + (25*ix);
    uint16_t y = tft.HEIGHT - sample[ix];
    uint16_t width = 20;
    uint16_t height = sample[ix];

    // Erase the old bar and text
    tft.set_pen_color(CANVAS);
    tft.fill_rect(x, y, width + 1, height - 1);

    // Sample a new value
    Board::AnalogPin pin;
    pin = (Board::AnalogPin) pgm_read_byte(analog_pin_map + ix);
    sample[ix] = (AnalogPin::sample(pin) >> 3);

    // Draw the new bar
    y = tft.HEIGHT - sample[ix];
    height = sample[ix];
    tft.set_pen_color(PEN);
    tft.draw_rect(x, y, width, height);
    textbox.set_cursor(x + 2, tft.HEIGHT - 10);
    cout.print((sample[ix]*500)/tft.HEIGHT);
  }
  Watchdog::delay(128);
}
