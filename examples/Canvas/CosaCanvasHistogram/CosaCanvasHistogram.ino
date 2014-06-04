/**
 * @file CosaCanvasHistogram.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * (A0)----------------->
 * (A1)----------------->
 * (A2)----------------->
 * (A3)----------------->
 * (A4)----------------->
 * (A5)----------------->
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Canvas/Driver/ST7735.hh"
#include "Cosa/Canvas/Element/Textbox.hh"

// The display and an iostream to the device
ST7735 tft;
Textbox textbox(&tft);
IOStream cout(&textbox);
Canvas::color16_t CANVAS, PEN;

// Map with analog pins
static const Board::AnalogPin analog_pin_map[] __PROGMEM = {
  Board::A0, 
  Board::A1, 
  Board::A2, 
  Board::A3, 
  Board::A4, 
  Board::A5
};

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
  for (uint8_t ix = 0; ix < membersof(analog_pin_map); ix++) {
    static uint16_t sample[membersof(analog_pin_map)];
    if (sample[ix] < 20) sample[ix] = 20;
    uint8_t x = 6 + (25*ix);
    uint8_t y = tft.HEIGHT - sample[ix];
    uint8_t width = 20;
    uint8_t height = sample[ix];

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
