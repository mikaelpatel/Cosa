/**
 * @file CosaST7565P.ino
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
 * @section Description
 * Demonstration of the ST7565P device driver with mapping to 
 * IOStream::Device.
 * 
 * @section Circuit
 * Connect Arduino to ST7565P (Arduino => ST7565P):
 * D6 ==> SI, D7 ==> SCL, D8 ==> DC, D9 ==> CS.
 * RST ==> RST.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Canvas/OffScreen.hh"
#include "Cosa/IOStream/Driver/ST7565P.hh"
#include "Cosa/Canvas/Font/FixedNums8x16.hh"
#include "Cosa/Canvas/Icon/arduino_icon_64x32.h"
#include "Cosa/Canvas/Icon/arduino_icon_96x32.h"

ST7565P lcd;
#undef putchar

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD screen and show arduino icon
  lcd.begin();
  lcd.putchar('\f');
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);
  SLEEP(2);
  
  // Use LCD bind to trace and use inverted text mode for banner
  trace.begin(&lcd);
  ST7565P::TextMode saved = lcd.set_text_mode(ST7565P::INVERTED_TEXT_MODE);
  trace << PSTR("\fCosaST7565P: started\n");
  lcd.set_cursor(0, 1);
  TRACE(lcd.set_text_mode(saved));
  trace << PSTR("=====================\n");
  INFO("saved = %d", saved);
  SLEEP(2);

  // Use the trace iostream onto the LCD with output operator
  trace << PSTR("\fBackspace\n");
  trace << PSTR("\f0123456890ABCDEFGHIJK");
  for (uint8_t i = 0; i < 22; i++) {
    Watchdog::delay(256);
    trace << PSTR("\b \b");
  }
  
  // Use number base handling 
  trace << PSTR("\fNumber base\n");
  uint16_t ticks = Watchdog::ticks();
  TRACE(ticks);
  trace << PSTR("bcd = ") << bcd << 0x55 << endl;
  trace << PSTR("bin = ") << bin << 0x55 << endl;
  trace << PSTR("oct = ") << oct << 0x55 << endl;
  trace << PSTR("dec = ") << dec << 0x55 << endl;
  trace << PSTR("hex = ") << hex << 0x55 << endl;
  trace << PSTR("ptr = ") << &lcd;
  SLEEP(2);

  // Scrolling
  trace << PSTR("\fScrolling\n");
  for (uint8_t i = 0; i < 64; i++) {
    trace << PSTR("The quick brown fox jumps over the lazy dog. ");
    Watchdog::delay(128);
  }
  SLEEP(2);

  // Dump the LCD object raw format with normal print function
  trace << PSTR("\fBuffer dump\n");
  trace.print(&lcd, sizeof(lcd), IOStream::hex, 5);
  SLEEP(2);

  // Dump characters in system font
  trace << PSTR("\fSystem Font 5x7\n");
  for (uint8_t c = 0; c < 128; c++) {
    trace << (char) (((c == '\n') || (c == '\f') || (c == '\b')) ? ' ' : c);
  }
  SLEEP(2);

  // Play around with the offscreen canvas
  uint8_t buffer[ST7565P::WIDTH * ST7565P::HEIGHT / CHARBITS];
  OffScreen offscreen(ST7565P::WIDTH, ST7565P::HEIGHT, buffer);
  offscreen.begin();
  offscreen.draw_rect(0, 0, 10, 10);
  offscreen.fill_rect(2, 2, 7, 7);
  offscreen.draw_rect(20, 20, 10, 10);
  offscreen.draw_circle(20, 20, 10);
  offscreen.fill_circle(20, 20, 8);
  offscreen.draw_roundrect(20, 20, 40, 20, 10);
  offscreen.fill_roundrect(22, 22, 36, 16, 7);
  offscreen.draw_rect(20, 20, 40, 20);
  offscreen.draw_rect(0, 0, offscreen.WIDTH - 1, offscreen.HEIGHT - 1);
  offscreen.set_cursor(15, 2);
  offscreen.draw_string_P(PSTR("OffScreen Canvas"));

  // Draw the off-screen canvas on the LCD
  lcd.set_cursor(0, 0);
  lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
  SLEEP(4);
}

void loop()
{
  // Draw bars with analog sample values (A0..A5)
  lcd.putchar('\f');
  for (uint8_t i = 0; i < ST7565P::LINES; i++) {
    trace.printf_P(PSTR("A%d:"), i);
    uint8_t procent = (AnalogPin::sample(i) * 100L) / 1023;
    lcd.draw_bar(procent, ST7565P::WIDTH - 20);
    if (i != ST7565P::LINES - 1) trace << endl;
  }
  SLEEP(1);

  // Every 4 seconds display the arduino icon and count down time
  static const uint8_t SHOW_BANNER = 4;
  static uint8_t banner = 0;
  if (++banner == SHOW_BANNER) {

    // Display the Arduino icon
    lcd.putchar('\f');
    lcd.set_cursor((lcd.WIDTH - 96)/2, 1);
    lcd.set_display_mode(ST7565P::REVERSE_DISPLAY_MODE);
    lcd.draw_icon(arduino_icon_96x32);
    SLEEP(2);

    lcd.set_display_mode(ST7565P::NORMAL_DISPLAY_MODE);
    banner = 0;

    // Counters
    static uint8_t min = 30;
    static uint8_t sec = 00;

    // Draw the current counter value off-screen
    uint8_t buffer[ST7565P::WIDTH * ST7565P::HEIGHT / CHARBITS];
    OffScreen offscreen(ST7565P::WIDTH, ST7565P::HEIGHT, buffer);
    offscreen.begin();
    offscreen.set_text_font(&fixednums8x16);
    offscreen.draw_roundrect(8, 8, lcd.WIDTH - 18, lcd.HEIGHT - 18, 8);
    offscreen.set_cursor((lcd.WIDTH - (fixednums8x16.get_width('0') * 5))/2,
			 (lcd.HEIGHT - fixednums8x16.get_height('0'))/2);
    offscreen.draw_char('0' + min/10);
    offscreen.draw_char('0' + min%10);
    offscreen.draw_char(':');
    offscreen.draw_char('0' + sec/10);
    offscreen.draw_char('0' + sec%10);

    // Draw the off-screen canvas on the LCD
    lcd.putchar('\f');
    lcd.draw_bitmap(offscreen.get_bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
    // Decrement counter
    if (sec == 0) {
      if (min != 00) {
	sec = 50;
	min -= 1;
      }
    } else
      sec -= 10;
    SLEEP(4);
  }
}
