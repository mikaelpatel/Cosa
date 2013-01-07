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
#include "Cosa/Icon/arduino_icon_34x32.h"
#include "Cosa/Font/System5x7.hh"
#include "Cosa/Font/FixedNums8x16.hh"

// Use the TFT display as canvas
ST7735R tft;

// An init-script 
const uint8_t init_script[] PROGMEM = {
  Canvas::SET_CANVAS_COLOR, 20, 20, 20,
  Canvas::SET_CURSOR, (128-34)/2, (160-32)/2,
  Canvas::FILL_SCREEN,
  Canvas::SET_TEXT_COLOR, 255, 255, 255,
  Canvas::SET_PEN_COLOR, 255, 255, 255,
  Canvas::DRAW_ICON, 1, 1,
  Canvas::MOVE_CURSOR, -30, 36, 
  Canvas::DRAW_STRING_P, 5,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::SET_TEXT_COLOR, 0, 0, 0,
  Canvas::END_SCRIPT
};

// The test drawing script
const uint8_t script[] PROGMEM = {
  Canvas::SET_CANVAS_COLOR, 100, 100, 100,
  Canvas::FILL_SCREEN,

  Canvas::SET_PEN_COLOR, 100, 100, 200,
  Canvas::SET_CURSOR, 10, 10,
  Canvas::FILL_RECT, 108, 30,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::DRAW_RECT, 108, 30,
  Canvas::SET_CURSOR, 30, 20,
  Canvas::DRAW_STRING_P, 4,

  Canvas::SET_CURSOR, 60, 60,
  Canvas::DRAW_LINE, 100, 100,
  Canvas::DRAW_LINE, 20, 100,
  Canvas::DRAW_LINE, 60, 60,

  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::SET_CURSOR, 20, 100,
  Canvas::DRAW_LINE, 20, 130,
  Canvas::SET_PEN_COLOR, 255, 0, 255,
  Canvas::CALL_SCRIPT, 3,
  Canvas::DRAW_CHAR, 3,

  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::SET_CURSOR, 60, 60,
  Canvas::DRAW_LINE, 60, 130,
  Canvas::SET_PEN_COLOR, 255, 255, 0,
  Canvas::CALL_SCRIPT, 3,
  Canvas::DRAW_CHAR, 4,

  Canvas::MOVE_CURSOR, -16, -10,
  Canvas::DRAW_RECT, 24, 24,

  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::SET_CURSOR, 100, 100,
  Canvas::DRAW_LINE, 100, 130,
  Canvas::SET_PEN_COLOR, 0, 255, 255,
  Canvas::CALL_SCRIPT, 3,
  Canvas::DRAW_CHAR, 5,

  Canvas::SET_TEXT_FONT, 7,
  Canvas::SET_PEN_COLOR, 255, 255, 255,
  Canvas::SET_CURSOR, 60, 60,
  Canvas::FILL_CIRCLE, 10,
  Canvas::SET_CURSOR, 100, 100,
  Canvas::FILL_CIRCLE, 10,
  Canvas::SET_CURSOR, 20, 100,
  Canvas::FILL_CIRCLE, 10,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::SET_CURSOR, 60, 60,
  Canvas::DRAW_CIRCLE, 10,
  Canvas::MOVE_CURSOR, -3, -7,
  Canvas::DRAW_CHAR, '1',
  Canvas::SET_CURSOR, 100, 100,
  Canvas::DRAW_CIRCLE, 10,
  Canvas::MOVE_CURSOR, -3, -7,
  Canvas::DRAW_CHAR, '2',
  Canvas::SET_CURSOR, 20, 100,
  Canvas::DRAW_CIRCLE, 10,
  Canvas::MOVE_CURSOR, -3, -7,
  Canvas::DRAW_CHAR, '3',
  Canvas::SET_TEXT_FONT, 6,

  Canvas::END_SCRIPT
};

// A sub-script 
const uint8_t sub_script[] PROGMEM = {
  Canvas::MOVE_CURSOR, -10, -10,
  Canvas::FILL_RECT, 20, 20,
  Canvas::SET_PEN_COLOR, 0, 0, 0,
  Canvas::DRAW_RECT, 20, 20,
  Canvas::MOVE_CURSOR, 8, 8,
  Canvas::END_SCRIPT
};

// And a message
const char msg[] PROGMEM = "Hello World";
const char banner[] PROGMEM = "Cosa Canvas 1.0";

// The script table
PGM_VOID_P tab[] PROGMEM = {
  init_script,
  arduino_icon_34x32,
  script,
  sub_script,
  msg,
  banner,
  &system5x7,
  &fixednums8x16
};

void setup()
{
  // Initiate trace stream
  trace.begin(9600, PSTR("CosaCanvasScript: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(Canvas));
  TRACE(sizeof(tft));
  TRACE(sizeof(tab));
  TRACE(sizeof(init_script));
  TRACE(sizeof(script));
  TRACE(sizeof(sub_script));
  TRACE(sizeof(arduino_icon_34x32));
  TRACE(sizeof(system5x7));
  TRACE(sizeof(fixednums8x16));
  TRACE(sizeof(msg));
  TRACE(sizeof(banner));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Benchmark the display start
  uint32_t start, ms;
  start = micros();
  tft.begin();
  tft.fill_screen();
  ms = (micros() - start) / 1000L;
  INFO("begin: %ul ms", ms);
}

void loop()
{
  uint32_t start, ms;

  // Run init script to display arduino icon
  start = micros();
  tft.run(0, tab, membersof(tab));
  ms = (micros() - start) / 1000L;
  INFO("init script run: %ul ms", ms);
  SLEEP(2);

  // Run script and measure execution time
  start = micros();
  tft.run(2, tab, membersof(tab));
  ms = (micros() - start) / 1000L;
  INFO("scipt run: %ul ms", ms);
  SLEEP(2);
}
