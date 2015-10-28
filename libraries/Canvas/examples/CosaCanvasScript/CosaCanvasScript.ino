/**
 * @file CosaCanvasScript.ino
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
 * Cosa Canvas script demonstration for ST7735 device driver.
 * Runs a small script and measures the execution time.
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
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <GDDRAM.h>
#include <ST7735.h>

#include <Canvas.h>
#include "Canvas/Icon/arduino_icon_34x32.h"

#include <Font.h>
#include "System5x7.hh"
#include "FixedNums8x16.hh"

#include "Cosa/RTT.hh"
#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

ST7735 tft;

// An init-script: Draw arduino icon and canvas script banner
CANVAS_BEGIN_SCRIPT(init_script)
  CANVAS_SET_CANVAS_COLOR(20, 20, 20)
  CANVAS_SET_CURSOR((128-34)/2, (160-32)/2)
  CANVAS_FILL_SCREEN()
  CANVAS_SET_TEXT_COLOR(255, 255, 255)
  CANVAS_SET_PEN_COLOR(255, 255, 255)
  CANVAS_DRAW_ICON(1, 1)
  CANVAS_MOVE_CURSOR(-30, 36)
  CANVAS_DRAW_STRING(5)
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_SET_TEXT_COLOR(0, 0, 0)
CANVAS_END_SCRIPT

// The test drawing script: Show-off the script commands
CANVAS_BEGIN_SCRIPT(script)
  CANVAS_SET_CANVAS_COLOR(100, 100, 100)
  CANVAS_FILL_SCREEN()

  // Draw light blue filled/outlined rectangle with 'Hello World' in center
  CANVAS_SET_PEN_COLOR(100, 100, 200)
  CANVAS_SET_CURSOR(10, 10)
  CANVAS_FILL_RECT(108, 30)
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_DRAW_RECT(108, 30)
  CANVAS_SET_CURSOR(30, 20)
  CANVAS_DRAW_STRING(4)

  // Draw triangle in the center of the screen
  CANVAS_SET_CURSOR(60, 60)
  CANVAS_DRAW_POLY(8, 1)

  // Draw magenta filled/outlined rectangle with a character in center
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_SET_CURSOR(20, 100)
  CANVAS_DRAW_LINE(20, 130)
  CANVAS_SET_PEN_COLOR(255, 0, 255)
  CANVAS_CALL_SCRIPT(3)
  CANVAS_DRAW_CHAR(3)

  // Draw yellow filled/outlined rectangle with a character in center
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_SET_CURSOR(60, 60)
  CANVAS_DRAW_LINE(60, 130)
  CANVAS_SET_PEN_COLOR(255, 255, 0)
  CANVAS_CALL_SCRIPT(3)
  CANVAS_DRAW_CHAR(4)

  // Add an extra outline to the rectangle
  CANVAS_MOVE_CURSOR(-16, -10)
  CANVAS_DRAW_RECT(24, 24)

  // Draw cyan filled/outlined rectangle with a character in center
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_SET_CURSOR(100, 100)
  CANVAS_DRAW_LINE(100, 130)
  CANVAS_SET_PEN_COLOR(0, 255, 255)
  CANVAS_CALL_SCRIPT(3)
  CANVAS_DRAW_CHAR(5)

  // Draw white filled/outlined circles with numbers in fixednumsfont8x16
  CANVAS_SET_TEXT_FONT(7)
  CANVAS_SET_PEN_COLOR(255, 255, 255)
  CANVAS_SET_CURSOR(60, 60)
  CANVAS_FILL_CIRCLE(10)
  CANVAS_SET_CURSOR(100, 100)
  CANVAS_FILL_CIRCLE(10)
  CANVAS_SET_CURSOR(20, 100)
  CANVAS_FILL_CIRCLE(10)
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_SET_CURSOR(60, 60)
  CANVAS_DRAW_CIRCLE(10)
  CANVAS_MOVE_CURSOR(-3, -7)
  CANVAS_DRAW_CHAR('1')
  CANVAS_SET_CURSOR(100, 100)
  CANVAS_DRAW_CIRCLE(10)
  CANVAS_MOVE_CURSOR(-3, -7)
  CANVAS_DRAW_CHAR('2')
  CANVAS_SET_CURSOR(20, 100)
  CANVAS_DRAW_CIRCLE(10)
  CANVAS_MOVE_CURSOR(-3, -7)
  CANVAS_DRAW_CHAR('4')
  CANVAS_SET_TEXT_FONT(6)
CANVAS_END_SCRIPT

// A sub-script: Draw filled/outlined rectangle(20, 20) at current position
// And center cursor for text
CANVAS_BEGIN_SCRIPT(sub_script)
  CANVAS_MOVE_CURSOR(-10, -10)
  CANVAS_FILL_RECT(20, 20)
  CANVAS_SET_PEN_COLOR(0, 0, 0)
  CANVAS_DRAW_RECT(20, 20)
  CANVAS_MOVE_CURSOR(8, 8)
CANVAS_END_SCRIPT

// The messages and banner
const char msg[] __PROGMEM = "Hello World";
const char banner[] __PROGMEM = "Cosa Canvas 1.0";

// A polygon
const int8_t poly[] __PROGMEM = {
   40,  40,
  -80,   0,
   40, -40,
    0,   0
};

// The script table: the order is important as index is used in scripts
const void_P table[] __PROGMEM = {
  init_script,
  arduino_icon_34x32,
  script,
  sub_script,
  msg,
  banner,
  &system5x7,
  &fixednums8x16,
  poly
};

void setup()
{
  // Initiate trace stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCanvasScript: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(Canvas));
  TRACE(sizeof(tft));
  TRACE(sizeof(table));
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
  RTT::begin();

  // Benchmark the display start
  uint32_t start, stop, ms;
  start = RTT::micros();
  tft.begin();
  tft.fill_screen();
  stop = RTT::micros();
  ms = (stop - start) / 1000L;
  INFO("begin: %ul ms", ms);

  TRACE(tft.WIDTH);
  TRACE(tft.HEIGHT);
  TRACE(tft.get_orientation());
}

void loop()
{
  uint32_t start, stop, ms;

  // Run init script to display arduino icon and banner
  start = RTT::micros();
  tft.run(0, table, membersof(table));
  stop = RTT::micros();
  ms = (stop - start) / 1000L;
  INFO("init script run: %ul ms", ms);
  sleep(2);

  // Run script/sub-script and measure execution time
  start = RTT::micros();
  tft.run(2, table, membersof(table));
  stop = RTT::micros();
  ms = (stop - start) / 1000L;
  INFO("scipt run: %ul ms", ms);
  sleep(2);
}
