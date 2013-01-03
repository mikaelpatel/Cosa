/**
 * @file CosaCanvasScript.ino
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
 * Cosa Canvas script demonstration for ST7735R device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/SPI/ST7735R.hh"

ST7735R tft;

const uint8_t script[] PROGMEM = {
  Canvas::SET_PEN_COLOR, 100, 100, 100,
  Canvas::FILL_SCREEN,

  Canvas::SET_PEN_COLOR, 255, 100, 100,
  Canvas::SET_CURSOR, 10, 10,
  Canvas::FILL_RECT, 108, 30,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::DRAW_RECT, 108, 30,
  Canvas::SET_CURSOR, 30, 20,
  Canvas::DRAW_STRING_P, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', 0,

  Canvas::SET_CURSOR, 60, 60,
  Canvas::DRAW_CIRCLE, 10,
  Canvas::DRAW_LINE, 100, 100,
  Canvas::DRAW_CIRCLE, 10,
  Canvas::DRAW_LINE, 20, 100,
  Canvas::DRAW_CIRCLE, 10,
  Canvas::DRAW_LINE, 60, 60,

  Canvas::SET_CURSOR, 50, 130,
  Canvas::SET_PEN_COLOR, 100, 255, 0,
  Canvas::FILL_RECT, 20, 20,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::DRAW_RECT, 20, 20,
  Canvas::SET_CURSOR, 10, 130,
  Canvas::SET_PEN_COLOR, 255, 0, 100,
  Canvas::FILL_RECT, 20, 20,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::DRAW_RECT, 20, 20,
  Canvas::SET_CURSOR, 90, 130,
  Canvas::SET_PEN_COLOR, 0, 100, 255,
  Canvas::FILL_RECT, 20, 20,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::DRAW_RECT, 20, 20,

  Canvas::END_SCRIPT
};

void setup()
{
  // Initiate trace stream
  trace.begin(9600, PSTR("CosaCanvasScript: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(tft));
  TRACE(sizeof(script));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Initiate the display
  TRACE(tft.begin());

  // Benchmark the script run
  uint32_t start, ms;
  start = micros();
  tft.run(script);
  ms = (micros() - start) / 1000L;
  INFO("run script: %ul ms", ms);
}

void loop()
{
}
