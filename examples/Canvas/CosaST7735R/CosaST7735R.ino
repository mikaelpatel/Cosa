/**
 * @file CosaST7735R.ino
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
 * Cosa demonstration of device driver for ST7735R, 262K Color
 * Single-Chip TFT Controller. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/SPI/ST7735R.hh"

ST7735R tft;
IOStream cout(&tft);

void setup()
{
  // Initiate trace stream
  trace.begin(9600, PSTR("CosaST7735R: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(tft));
  TRACE(sizeof(cout));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();

  // Initiate the display
  TRACE(tft.begin());
}

void loop()
{
  uint32_t start, ms;

  // Fill screen
  start = micros();
  tft.set_pen_color(tft.WHITE);
  tft.fill_screen();
  ms = (micros() - start) / 1000L;
  INFO("fill_screen: %ul ms", ms);

  // Print on the output stream
  start = micros();
  tft.set_text_color(tft.BLUE);
  tft.set_scale(1);
  tft.set_cursor(0, 0);
  cout.print_P(PSTR("CosaST7735R: started"));
  cout.println();
  cout.printf_P(PSTR("color(%hd)\n"), tft.get_text_color());
  uint8_t x, y;
  tft.get_cursor(x, y);
  cout.printf_P(PSTR("cursor(x = %d, y = %d)\n"), x, y);
  tft.set_text_color(tft.RED);
  tft.set_scale(2);
  cout.print_P(PSTR("  Hello\n  World"));
  cout.println();
  tft.set_text_color(tft.BLACK);
  tft.set_scale(1);
  ms = (micros() - start) / 1000L;
  cout.printf_P(PSTR("draw_intro: %ul ms\n"), ms);
  Watchdog::delay(2048);

  // Draw grid with pin status
  tft.set_pen_color(tft.WHITE);
  tft.fill_screen();
  start = micros();
  for (uint8_t x = 0, y = 2; y < tft.SCREEN_HEIGHT; y += 20, x++) {
    tft.set_pen_color(tft.grayscale(75));
    tft.fill_rect(10, y, tft.SCREEN_WIDTH - 20, 16);
    tft.set_pen_color(tft.BLACK);
    tft.draw_rect(10, y, tft.SCREEN_WIDTH - 20, 16);
    tft.set_pen_color(digitalRead(x) ? tft.RED : tft.GREEN);
    tft.fill_circle(28, y + 8, 5);
    tft.set_cursor(50, y + 5);
    cout.printf_P(PSTR("A%d = %d"), x, analogRead(x));
  }
  ms = (micros() - start) / 1000L;
  INFO("pin_status: %ul ms", ms);
  Watchdog::delay(2048);  

  // Grid with draw pixel
  tft.set_pen_color(tft.WHITE);
  tft.fill_screen();
  tft.set_pen_color(tft.BLACK);
  start = micros();
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 2) {
    for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 2) {
      tft.draw_pixel(x, y);
    }
  }
  ms = (micros() - start) / 1000L;
  INFO("draw_pixel: %ul ms", ms);
  Watchdog::delay(2048);

  // Grid with draw rectangle
  tft.set_pen_color(tft.BLACK);
  start = micros();
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 20) {
    for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 20) {
      tft.draw_rect(x, y, 20, 20);
    }
  }
  ms = (micros() - start) / 1000L;
  INFO("draw_rect: %ul ms", ms);
  Watchdog::delay(2048);
  
  // Fill rectangles
  tft.set_pen_color(tft.WHITE);
  start = micros();
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 20) {
    for (uint8_t y = x; y < tft.SCREEN_HEIGHT; y += 40) {
      tft.fill_rect(x + 1, y + 1, 19, 19);
    }
  }
  ms = (micros() - start) / 1000L;
  INFO("fill_rect: %ul ms", ms);
  Watchdog::delay(2048);
  
  // Grid with draw circle
  tft.set_pen_color(tft.WHITE);
  tft.fill_screen();
  tft.set_pen_color(tft.BLACK);
  start = micros();
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 20) {
    for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 20) {
      tft.draw_circle(x + 10, y + 10, 10);
    }
  }
  ms = (micros() - start) / 1000L;
  INFO("draw_circle: %ul ms", ms);
  Watchdog::delay(2048);

  // Fill circles
  tft.set_pen_color(tft.RED);
  start = micros();
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 20) {
    for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 20) {
      tft.fill_circle(x + 10, y + 10, 9);
    }
  }
  ms = (micros() - start) / 1000L;
  INFO("fill_circle: %ul ms", ms);
  Watchdog::delay(2048);
  
  // Draw lines
  tft.set_pen_color(tft.BLACK);
  tft.fill_screen();
  start = micros();
  tft.set_pen_color(tft.RED);
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 6) {
    tft.draw_line(0, 0, x, tft.SCREEN_HEIGHT - 1);
  }
  tft.set_pen_color(tft.GREEN);
  for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 6) {
    tft.draw_line(0, 0, tft.SCREEN_WIDTH - 1, y);
  }
  tft.set_pen_color(tft.BLUE);
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 6) {
    tft.draw_line(tft.SCREEN_WIDTH - 1, 0, x, tft.SCREEN_HEIGHT - 1);
  }
  ms = (micros() - start) / 1000L;
  INFO("draw_line: %ul ms", ms);
  Watchdog::delay(2048);

  tft.set_pen_color(tft.BLACK);
  tft.fill_screen();
  start = micros();
  tft.set_pen_color(tft.CYAN);
  for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 6) {
    tft.draw_line(tft.SCREEN_WIDTH - 1, 0, 0, y);
  }
  tft.set_pen_color(tft.MAGENTA);
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 6) {
    tft.draw_line(0, tft.SCREEN_HEIGHT - 1, x, 0);
  }
  ms = (micros() - start) / 1000L;
  INFO("draw_line: %ul ms", ms);
  Watchdog::delay(2048);

  tft.set_pen_color(tft.BLACK);
  tft.fill_screen();
  tft.set_pen_color(tft.YELLOW);
  start = micros();
  for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 6) {
    tft.draw_line(0, tft.SCREEN_HEIGHT - 1, tft.SCREEN_WIDTH - 1, y);
  }
  for (uint8_t x = 0; x < tft.SCREEN_WIDTH; x += 6) {
    tft.draw_line(tft.SCREEN_WIDTH - 1, tft.SCREEN_HEIGHT - 1, x, 0);
  }
  for (uint8_t y = 0; y < tft.SCREEN_HEIGHT; y += 6) {
    tft.draw_line(tft.SCREEN_WIDTH - 1, tft.SCREEN_HEIGHT - 1, 0, y);
  }
  ms = (micros() - start) / 1000L;
  INFO("draw_line: %ul ms", ms);
  Watchdog::delay(2048);
}
