/**
 * @file CosaPCD8544.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Demonstration of the PCD8544 device driver with mapping to
 * IOStream::Device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/IOStream/Driver/PCD8544.hh"
#include "Cosa/Canvas/Icon/arduino_icon_64x32.h"
#include "Cosa/Canvas/Icon/arduino_icon_96x32.h"

PCD8544 lcd;
#undef putchar

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD and clean screen
  lcd.begin();

  // Clear the display with a form-feed and draw the Arduino icons
  lcd.putchar('\f');
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);
  SLEEP(2);

  // Use LCD bind to trace and use inverted text mode for banner
  trace.begin(&lcd);
  PCD8544::TextMode saved = lcd.set_text_mode(PCD8544::INVERTED_TEXT_MODE);
  trace << PSTR("\fCosaPCD8544: started\n");
  INFO("saved = %d", saved);
  lcd.set_text_mode(saved);
  SLEEP(2);

  // Use the trace iostream onto the LCD with output operator
  lcd.putchar('\f');
  trace << PSTR("01234568901234");
  for (uint8_t i = 0; i < 16; i++) {
    Watchdog::delay(256);
    trace << PSTR("\b \b");
  }
  uint16_t ticks = Watchdog::get_ticks();
  TRACE(ticks);
  trace << bin << 0x55 << endl;
  trace << oct << 0x55 << endl;
  trace << dec << 0x55 << endl;
  trace << hex << 0x55 << endl;
  trace << &lcd;
  SLEEP(2);

  // Dump the LCD object raw format with normal print function
  lcd.putchar('\f');
  trace.print(&lcd, sizeof(lcd) - 1, 16, 2);
  SLEEP(2);

  // Dump characters in system font; two pages, 64 characters each
  saved = lcd.set_text_mode(PCD8544::INVERTED_TEXT_MODE);
  trace << PSTR("\fFont page#1\n");
  lcd.set_text_mode(saved);
  uint8_t c;
  for (c = 0; c < 64; c++) {
    trace << (char) (((c == '\n') || (c == '\f') || (c == '\b')) ? ' ' : c);
  }
  SLEEP(4);
  saved = lcd.set_text_mode(PCD8544::INVERTED_TEXT_MODE);
  trace << PSTR("\fFont page#2\n");
  lcd.set_text_mode(saved);
  for (; c < 128; c++) {
    trace << (char) c;
  }
  SLEEP(4);

  // Play around with the offscreen canvas
  OffScreen offscreen(PCD8544::WIDTH, PCD8544::HEIGHT);
  offscreen.begin();
  offscreen.draw_rect(0, 0, 10, 10);
  offscreen.fill_rect(2, 2, 7, 7);
  offscreen.draw_circle(20, 20, 10);
  offscreen.fill_circle(20, 20, 8);
  Canvas::color16_t color = offscreen.set_pen_color(Canvas::WHITE);
  offscreen.fill_roundrect(20, 20, 40, 20, 10);
  offscreen.set_pen_color(color);
  offscreen.draw_roundrect(20, 20, 40, 20, 10);
  offscreen.fill_roundrect(22, 22, 36, 17, 8);
  offscreen.set_cursor(15, 0);
  offscreen.draw_string_P(PSTR("Hello World"));

  // Draw the created off-screen canvas on the LCD
  lcd.set_cursor(0, 0);
  lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
  SLEEP(4);
}

void loop()
{
  // Draw bars with analog sample values (A0..A5)
  lcd.putchar('\f');
  for (uint8_t i = 0; i < PCD8544::LINES; i++) {
    trace.printf_P(PSTR("A%d:"), i);
    uint8_t procent = (AnalogPin::sample(i) * 100L) / 1023;
    lcd.draw_bar(procent, PCD8544::WIDTH - 20);
    if (i != PCD8544::LINES - 1) trace << endl;
  }
  SLEEP(2);

  // Display the Arduino icon
  static const uint8_t SHOW_BANNER = 4;
  static uint8_t banner = 0;
  if (++banner == SHOW_BANNER) {
    lcd.putchar('\f');
    lcd.set_cursor(0, 1);
    lcd.set_display_mode(PCD8544::INVERSE_MODE);
    lcd.draw_icon(arduino_icon_96x32);
    SLEEP(4);
    lcd.set_display_mode(PCD8544::NORMAL_MODE);
    banner = 0;
  }
}
