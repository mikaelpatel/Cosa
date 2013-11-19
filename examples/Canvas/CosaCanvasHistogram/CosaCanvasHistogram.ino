/**
 * @file CosaCanvasHistogram.ino
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa demonstration of Canvas device driver for ST7735, 262K Color 
 * Single-Chip TFT Controller, and monitoring of analog pins as
 * a histogram.
 *
 * @section Circuit
 * Use the analog pin(0..MAX-1) as the probe pins. 
 * Connect Arduino to ST7735 Module;  
 * Arduino    ==> HY-1.8 SPI
 * -------------------------------
 *   GND      ==>   GND(1), 
 *   VCC(5V)  ==>   VCC(2), 
 *   RST      ==>   RESET(6),
 *   D9       ==>   A0(7), 
 *   MOSI/D11 ==>   SDA(8), 
 *   SCK/D13  ==>   SCK(9),
 *   SS/D10   ==>   CS(10), 
 *   VCC(5V)  ==>   LED+(15), 
 *   GND      ==>   LED-(16)    
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Canvas/Driver/ST7735.hh"
#include "Cosa/Canvas/Element/Textbox.hh"

// The display and an iostream to the device
ST7735 tft;
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
  static const uint8_t PIN_MAX = 6;
  for (uint8_t pin = 0; pin < PIN_MAX; pin++) {
    static uint16_t sample[PIN_MAX];
    if (sample[pin] < 20) sample[pin] = 20;
    uint8_t x = 6 + (25*pin);
    uint8_t y = tft.HEIGHT - sample[pin];
    uint8_t width = 20;
    uint8_t height = sample[pin];

    // Erase the old bar and text
    tft.set_pen_color(CANVAS);
    tft.fill_rect(x, y, width + 1, height - 1);

    // Sample a new value
    sample[pin] = (AnalogPin::sample(pin) >> 3);

    // Draw the new bar
    y = tft.HEIGHT - sample[pin];
    height = sample[pin];
    tft.set_pen_color(PEN);
    tft.draw_rect(x, y, width, height);
    textbox.set_cursor(x + 2, tft.HEIGHT - 10);
    cout.print((sample[pin]*500)/tft.HEIGHT);
  }
  Watchdog::delay(128);
}
