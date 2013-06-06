/**
 * @file CosaCanvasST7735.ino
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
 * Cosa demonstration of device driver for ST7735, 262K Color
 * Single-Chip TFT Controller. 
 *
 * @section Circuit
 * Connect Arduino to ST7735 Module (Arduino ==> HY-1.8 SPI):
 * GND ==> GND(1), VCC(5V) ==> VCC(2), RST ==> RESET(6),
 * D9 ==> A0(7), MOSI/D11 ==> SDA(8), SCK/D13 ==> SCK(9),
 * SS/D10 ==> CS(10), VCC(5V) ==> LED+(15), GND ==> LED-(16)    
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Memory.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Element/Textbox.hh"
#include "Cosa/Canvas/Icon/arduino_icon_34x32.h"
#include "Cosa/Canvas/Icon/arduino_icon_64x64.h"
#include "Cosa/Canvas/Icon/arduino_icon_96x32.h"
#include "Cosa/Canvas/Font/System5x7.hh"
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#include "Cosa/SPI/Driver/ST7735.hh"

ST7735 tft;
Textbox textbox(&tft);
IOStream console(&textbox);
#undef putchar

void setup()
{
  // Initiate trace stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaST7735: started"));

  // Check amount of free memory and size of objects
  TRACE(free_memory());
  TRACE(sizeof(Trace));
  TRACE(sizeof(IOStream::Device));
  TRACE(sizeof(UART));
  TRACE(sizeof(Canvas));
  TRACE(sizeof(Font));
  TRACE(sizeof(ST7735));
  TRACE(sizeof(Canvas::Context));
  TRACE(sizeof(Canvas::Element));
  TRACE(sizeof(IOStream));
  TRACE(sizeof(Textbox));

  // Start the watchdog with default timeout (16 ms)
  Watchdog::begin();
  RTC::begin();

  // Initiate the display
  TRACE(tft.begin());
}

void loop()
{
  static uint8_t direction = Canvas::PORTRAIT;
  uint32_t start, ms;

  // Test#1: Fill screen
  start = RTC::micros();
  tft.set_canvas_color(Canvas::WHITE);
  tft.fill_screen();
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#1:fill screen: %ul ms", ms);

  // Test#2: Use the display as an output stream
  start = RTC::micros();
  textbox.set_text_color(Canvas::BLUE);
  textbox.set_text_scale(1);
  textbox.set_text_port(2, 2, tft.WIDTH, tft.HEIGHT);
  console.print_P(PSTR("CosaST7735: started"));
  console.println();
  console.printf_P(PSTR("test#1:fill screen %ul ms\n"), ms);
  console.println();
  console.printf_P(PSTR("text_color(%od)\n"), textbox.get_text_color().rgb);
  console.printf_P(PSTR("text_scale(%d)\n"), textbox.get_text_scale());
  uint8_t x, y;
  textbox.get_cursor(x, y);
  console.printf_P(PSTR("cursor(x = %d, y = %d)\n"), x, y);
  textbox.set_text_color(Canvas::RED);
  textbox.set_text_scale(2);
  console.print_P(PSTR("  Hello\n  World"));
  console.println();
  ms = (RTC::micros() - start) / 1000L;
  textbox.set_text_color(Canvas::BLACK);
  textbox.set_text_scale(1);
  console.printf_P(PSTR("test#2:output stream: %ul ms\n"), ms);
  INFO("test#2:output stream: %ul ms", ms);
  SLEEP(2);

  // Test#3: Scroll text port
  start = RTC::micros();
  tft.set_canvas_color(tft.shade(Canvas::WHITE, 50));
  tft.fill_screen();
  tft.set_canvas_color(Canvas::WHITE);
  tft.draw_rect(4, 4, tft.WIDTH-8, tft.HEIGHT-8);
  textbox.set_text_port(5, 5, tft.WIDTH-10, tft.HEIGHT-10);
  console.print('\f');
  console.print(&tft, 200, IOStream::hex, 
		tft.get_orientation() == Canvas::PORTRAIT ? 4 : 6);
  ms = (RTC::micros() - start) / 1000L;
  console.printf_P(PSTR("%ul ms"), ms);
  INFO("test#3:scroll text mode: %ul ms", ms);
  SLEEP(2);

  // Test#4: Grid with draw pixel
  tft.fill_screen();
  tft.set_pen_color(Canvas::BLACK);
  start = RTC::micros();
  for (uint8_t x = 0; x < tft.WIDTH; x += 2) {
    for (uint8_t y = 0; y < tft.HEIGHT; y += 2) {
      tft.draw_pixel(x, y);
    }
  }
  tft.set_pen_color(Canvas::RED);
  tft.fill_rect(20, 20, 20, 20);
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#4:draw pixel grid: %ul ms", ms);
  SLEEP(2);

  // Test#5: Grid with draw rectangle
  tft.set_pen_color(Canvas::BLACK);
  start = RTC::micros();
  for (uint8_t x = 0; x < tft.WIDTH; x += 20) {
    for (uint8_t y = 0; y < tft.HEIGHT; y += 20) {
      tft.draw_rect(x, y, 20, 20);
    }
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#5:draw rect grid: %ul ms", ms);
  SLEEP(2);
  
  // Test#6: Fill some of the rectangles
  tft.set_pen_color(Canvas::WHITE);
  start = RTC::micros();
  for (uint8_t x = 0; x < tft.WIDTH; x += 20) {
    for (uint8_t y = x; y < tft.HEIGHT; y += 40) {
      tft.fill_rect(x + 1, y + 1, 19, 19);
    }
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#6:fill rect grid: %ul ms", ms);
  SLEEP(2);

  // Test#7: Fill circles
  tft.fill_screen();
  tft.set_text_color(Canvas::WHITE);
  tft.set_text_scale(1);
  start = RTC::micros();
  uint16_t color = Canvas::BLUE;
  for (uint8_t x = 0; x < tft.WIDTH; x += 30) {
    for (uint8_t y = 0; y < tft.HEIGHT; y += 30) {
      tft.set_pen_color(tft.shade(color, y * 100/tft.HEIGHT));
      tft.fill_circle(x, y, 12);
      tft.set_cursor(x - 2, y - 3);
      tft.draw_char('A' + (x*(tft.HEIGHT+30))/900 + y/30);
      tft.set_pen_color(Canvas::BLACK);
      tft.draw_circle(x, y, 12);
    }
    color <<= 3;
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#7:draw circle grid: %ul ms", ms);
  SLEEP(2);

  // Test#8: Draw lines
  tft.set_canvas_color(tft.shade(Canvas::WHITE, 20));
  tft.fill_screen();
  start = RTC::micros();
  tft.set_pen_color(Canvas::RED);
  for (uint8_t x = 0; x < tft.WIDTH; x += 6) {
    tft.draw_line(0, 0, x, tft.HEIGHT - 1);
  }
  tft.set_pen_color(Canvas::GREEN);
  for (uint8_t y = 0; y < tft.HEIGHT; y += 6) {
    tft.draw_line(0, 0, tft.WIDTH - 1, y);
  }
  tft.set_pen_color(Canvas::BLUE);
  for (uint8_t x = 0; x < tft.WIDTH; x += 6) {
    tft.draw_line(tft.WIDTH - 1, 0, x, tft.HEIGHT - 1);
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#8:draw lines: %ul ms", ms);
  SLEEP(2);

  // Test#9: Draw more lines
  start = RTC::micros();
  tft.set_pen_color(Canvas::BLACK);
  tft.fill_screen();
  tft.set_pen_color(Canvas::YELLOW);
  for (uint8_t y = 0; y < tft.HEIGHT; y += 6) {
    tft.draw_line(0, tft.HEIGHT - 1, tft.WIDTH - 1, y);
  }
  for (uint8_t x = 0; x < tft.WIDTH; x += 6) {
    tft.draw_line(tft.WIDTH - 1, tft.HEIGHT - 1, x, 0);
  }
  for (uint8_t y = 0; y < tft.HEIGHT; y += 6) {
    tft.draw_line(tft.WIDTH - 1, tft.HEIGHT - 1, 0, y);
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#9:draw more lines: %ul ms", ms);
  SLEEP(2);

  // Test#10: Display polygons 
  start = RTC::micros();
  tft.set_canvas_color(Canvas::WHITE);
  tft.fill_screen();
  static const int8_t polygon[] PROGMEM = { 
    100, 100, 
    -100, 0, 
    50, -50, 
    0, 50, 
    50, 50,
    -10, -10,
    0, 0 
  };
  for (uint8_t x = 10; x < tft.WIDTH - 40; x += 10) {
    tft.set_pen_color(tft.shade(Canvas::GREEN, 20 + x));
    tft.set_cursor(10 + x, 50 - x);
    tft.draw_poly_P(polygon);
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#10:polygon: %ul ms", ms);
  SLEEP(2);

  // Test#11: Display stroke
  start = RTC::micros();
  tft.set_canvas_color(Canvas::WHITE);
  tft.fill_screen();
  static const int8_t stroke[] PROGMEM = { 
    20, -100,
    20, 100,
    -30, -50,
    20, 0,
    0, 0 
  };
  for (uint8_t x = 10; x < tft.WIDTH - 40; x += 10) {
    tft.set_pen_color(tft.shade(Canvas::RED, 20 + x));
    tft.set_cursor(x, 120);
    tft.draw_stroke_P(stroke);
  }
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#11:stroke: %ul ms", ms);
  SLEEP(2);

  // Test#12: Display the Arduino Icons
  tft.set_canvas_color(tft.shade(Canvas::WHITE, 10));
  tft.fill_screen();
  tft.set_pen_color(tft.shade(Canvas::CYAN, 80));
  tft.draw_icon((tft.WIDTH-34)/2, (tft.HEIGHT-32)/2, arduino_icon_34x32);
  SLEEP(2);
  tft.fill_screen();
  tft.draw_icon((tft.WIDTH-64)/2, (tft.HEIGHT-64)/2, arduino_icon_64x64);
  SLEEP(2);
  tft.fill_screen();
  start = RTC::micros();
  tft.draw_icon((tft.WIDTH-96)/2, (tft.HEIGHT-32)/2, arduino_icon_96x32);
  ms = (RTC::micros() - start) / 1000L;
  INFO("test#12:draw arduino icon: %ul ms", ms);
  SLEEP(2);

  // Rotate display
  direction = !direction;
  tft.set_orientation(direction);
}
