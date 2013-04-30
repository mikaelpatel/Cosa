/**
 * @file CosaTinyPCD8544.ino
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
 * IOStream::Device for the ATtinyX5.
 * 
 * @section Circuit
 * Connect ATtinyX5 to PCD8544 (ATtinyX5 => PCD8544):
 * D0 ==> SDIN, D1 ==> SCLK, D2 ==> DC, D3 ==> SCE.
 * 
 * The PCD8544 should be connect using 3.3 V signals and VCC. 
 * Back-light should be max 3.3 V. Reduce voltage with 100-500 ohm 
 * resistor to ground.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/PCD8544.hh"
#include "Cosa/Canvas/Icon/arduino_icon_64x32.h"

AnalogPin data(Board::A2);
PCD8544 lcd;
#undef putchar

void setup()
{
  Watchdog::begin();

  lcd.begin(0x38);
  lcd.putchar('\f');
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);
  SLEEP(2);

  trace.begin(&lcd);
  trace << clear;
  PCD8544::TextMode saved = lcd.set_text_mode(PCD8544::INVERTED_TEXT_MODE);
  trace << PSTR("CosaTinyPCD8544: started    ");
  lcd.set_text_mode(saved);
  trace << PSTR("01234568901234");
  for (uint8_t i = 0; i < 16; i++) {
    Watchdog::delay(256);
    trace << PSTR("\b \b");
  }
  uint16_t ticks = Watchdog::ticks();
  TRACE(ticks);
  SLEEP(2);

  trace << clear;
  trace << bin << 0x55 << endl;
  trace << oct << 0x55 << endl;
  trace << dec << 0x55 << endl;
  trace << hex << 0x55 << endl;
  trace << &lcd;
  SLEEP(2);

  trace << clear;
  trace.print(&lcd, sizeof(lcd) - 1, IOStream::hex, 2);
  SLEEP(2);

  trace << clear;
  saved = lcd.set_text_mode(PCD8544::INVERTED_TEXT_MODE);
  trace << PSTR("  CHARACTERS  ");
  lcd.set_text_mode(saved);
  for (char c = ' '; c < 96; c++) trace << c;
  SLEEP(4);
  trace << clear;
}

void loop()
{
  uint16_t pos = (data.sample() * 14L) / 1024;
  for (uint16_t i = 0; i < 14; i++)
    trace << ((i == pos) ? '*' : ' ');
  Watchdog::delay(128);
}
