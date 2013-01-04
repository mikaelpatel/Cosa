/**
 * @file CosaShade.ino
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
 * Cosa demonstration of Cosa Canvas color shades on ST7735R, 
 * 262K Color Single-Chip TFT Controller. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/SPI/ST7735R.hh"

ST7735R tft;

void setup()
{
  // Initiate trace stream
  trace.begin(9600, PSTR("CosaShade: started"));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Initiate the display
  tft.begin();
  tft.set_pen_color(tft.BLACK);
  tft.fill_screen();
}

uint32_t draw_shade(uint16_t color)
{
  uint32_t start, ms;
  start = micros();
  tft.set_pen_color(tft.WHITE);
  tft.draw_rect(9, 9, tft.WIDTH - 19, tft.HEIGHT - 19);
  for (uint8_t y = 10; y < tft.HEIGHT - 10; y += 4) {
    uint8_t level = ((y - 10) * 100L) / (tft.HEIGHT - 10);
    tft.set_pen_color(tft.shade(color, level));
    tft.fill_rect(10, y, tft.WIDTH - 20, 4);
  }
  ms = (micros() - start) / 1000L;
  Watchdog::delay(2048);
  return (ms);
}

void loop()
{
  uint32_t ms;

  // Draw grayscale 
  ms = draw_shade(tft.WHITE);
  INFO("grayscale/fill_rect: %ul ms", ms);

  // Draw red shades
  ms = draw_shade(tft.RED);
  INFO("red/fill_rect: %ul ms", ms);

  // Draw green shades
  ms = draw_shade(tft.GREEN);
  INFO("green/fill_rect: %ul ms", ms);

  // Draw blue shades
  ms = draw_shade(tft.BLUE);
  INFO("blue/fill_rect: %ul ms", ms);
}
