/**
 * @file CosaHD44780.ino
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
 * Demonstration of the PCD8544 device driver with mapping to 
 * IOStream::Device.
 * 
 * @section Circuit
 * Use default pin configuration (Arduino/Standard, Mighty, ATtiny ==> LCD); 
 * D4 ==> D4, D5 ==> D5, D6 ==> D6, D7 ==> D7, D8 ==> RS, and D9 ==> EN.
 * For Arduino/Mega: D8 ==> RS, D9 ==> EN, D10 ==> D4, D11 => D5, 
 * D12 ==> D6, and D13 ==> D7.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/HD44780.hh"

HD44780 lcd;
#undef putchar

const uint8_t bitmaps[] PROGMEM = {
  // 0: Arrow right
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000,
  // 1: Arrow left
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000,
  // 2: Target
  0b01110,
  0b10101,
  0b10101,
  0b11111,
  0b10101,
  0b10101,
  0b01110,
  0b00000,
  // 3: Heart
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  // 4: Happy
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000,
  // 5: Sad
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  // 6: Arms down
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010,
  // 7: Arms up
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

void setup()
{
  Watchdog::begin();
  lcd.begin();
  for (char c = 0; c < 8; c++) lcd.set_custom_char_P(c, &bitmaps[c*8]);
  trace.begin(&lcd, PSTR("\fCosaHD44780"));
  lcd.cursor_underline_off();
  lcd.cursor_blink_off();
  for (uint8_t i = 0; i < 8; i++) {
    lcd.set_cursor(lcd.WIDTH - 1, 0);
    trace << (char) 6;
    Watchdog::delay(256);
    lcd.set_cursor(lcd.WIDTH - 1, 0);
    trace << (char) 7;
    Watchdog::delay(256);
  }
}

void loop()
{
  static char c = 0;
  if (c == 0) trace << clear;
  trace << c++;
  Watchdog::delay(64);
}
