/**
 * @file CosaHD44780.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Demonstration of the HD44780 (aka 1602) device driver with mapping 
 * to IOStream::Device.
 *
 * @section Circuit
 * See HD44780.hh for description of LCD adapter circuits.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/LCD/Driver/HD44780.hh"

// LCD and communication port
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;
HD44780::MJKDZ port(0);
// HD44780::MJKDZ port;
// HD44780::GYIICLCD port;
// HD44780::DFRobot port;
// HD44780::SainSmart port;
// HD44780::ERM1602_5 port;
// HD44780 lcd(&port, 20, 4);
HD44780 lcd(&port, 16, 4);
// HD44780 lcd(&port);

const uint8_t bitmaps[] __PROGMEM = {
  // Bar(1)
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  // Bar(2)
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  // Bar(3)
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  // Bar(4)
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  // Bar(5)
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
};
const uint8_t SIZEOF_BITMAP = 8;
const uint8_t BITMAPS_MAX = membersof(bitmaps) / SIZEOF_BITMAP;

void setup()
{
  // Initiate Watchdog for low power delay
  Watchdog::begin();

  // Initate LCD and load custom character bitmaps
  lcd.begin();
  lcd.set_tab_step(2);
  lcd.cursor_underline_off();
  lcd.cursor_blink_off();
  for (char c = 0; c < BITMAPS_MAX; c++) 
    lcd.set_custom_char_P(c, &bitmaps[c*SIZEOF_BITMAP]);

  // Bind LCD to trace output and print the custom characters
  trace.begin(&lcd, PSTR("\f\aCosaHD44780\a"));
  for (uint8_t i = 0; i < lcd.WIDTH; i++) {
    uint8_t x, y;
    lcd.get_cursor(x, y);
    for (char c = 0; c < BITMAPS_MAX; c++) {
      lcd.set_cursor(x, y);
      trace << c;
      delay(64);
    }
  }
  trace << clear;

  trace << PSTR("\f\aBACKLIGHT\a\n");
  for (uint8_t i = 0; i < 4; i++) {
    delay(100);
    lcd.backlight_off();
    delay(100);
    lcd.backlight_on();
  }

  // Simple scrolling text
  trace << PSTR("\f\aSCROLLING\a\n");
  static const char msg[] __PROGMEM = 
    "The quick brown fox jumps over the lazy dog.";
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t len = strlen_P(msg);
    for (uint8_t j = 0; j < len; j++) {
      trace << (char) pgm_read_byte(msg + j);
      delay(64);
    }
    trace << ' ';
  }

  // Use number base handling 
  trace << PSTR("\f\aNUMBER BASE\a\n");
  trace << bcd << 0x55 << ' ' 
	<< oct << 0x55 << ' '
	<< dec << 0x55 << ' '
	<< hex << 0x55;
  sleep(2);

  // Banner for font display
  trace << PSTR("\f\aFONT\a\n");
}

void loop()
{
  // Step through the LCD font. Use tab steps between characters
  static char c = 0;
  trace << c++ << '\t';
  delay(64);
}
