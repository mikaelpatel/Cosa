/**
 * @file CosaLCDhbar.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Cosa demonstration of LCD driver and semi-graphical drawing of
 * horizontal bar.
 *
 * @section Circuit
 * Designed for a LCD Keypad shield with 16x2 HD44780 LCD and six
 * buttons. Alternatively connect potentiometer to Analog pin 0.
 * @code
 *                           HD44780
 *                       +------------+
 * (GND)---------------1-|VSS         |
 * (VCC)---------------2-|VDD         |
 * (D10)--(BT)---------3-|VO          |
 * (D8)----------------4-|RS          |
 *                     5-|RW          |
 * (D9)----------------6-|EN          |
 *                     7-|D0          |
 *                     8-|D1          |
 *                     9-|D2          |
 *                    10-|D3          |
 * (D4/D0)------------11-|D4          |
 * (D5/D1)------------12-|D5          |
 * (D6/D2)------------13-|D6          |
 * (D7/D3)------------14-|D7          |
 * (VCC)----[330]-----15-|A           |
 * (GND)--------------16-|K           |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"

// Select port type to use with the LCD device driver.
// LCD and communication port
#include <HD44780.h>

// HD44780 driver built-in adapters
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;

// I2C expander io port based adapters
// #include <PCF8574.h>
// #include <MJKDZ_LCD_Module.h>
// MJKDZ_LCD_Module port;
// MJKDZ_LCD_Module port(0);
// #include <GY_IICLCD.h>
// GY_IICLCD port;
// #include <DFRobot_IIC_LCD_Module.h>
// DFRobot_IIC_LCD_Module port;
// #include <SainSmart_LCD2004.h>
// SainSmart_LCD2004 port;
#include <MCP23008.h>
#include <Adafruit_I2C_LCD_Backpack.h>
Adafruit_I2C_LCD_Backpack port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);
IOStream cout(&lcd);
#if defined(BOARD_ATTINY)
AnalogPin keypad(Board::A3);
#else
AnalogPin keypad(Board::A0);
#endif

// Initiate horizontal bar (left to right, 1..5)
void init_bar()
{
  static const uint8_t bitmaps[] __PROGMEM = {
    0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000,
    0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000,
    0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100,
    0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110, 0b11110,
    0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111,
  };
  const uint8_t SIZEOF_BITMAP = 8;
  const uint8_t BITMAPS_MAX = membersof(bitmaps) / SIZEOF_BITMAP;
  for (char c = 0; c < BITMAPS_MAX; c++)
    lcd.set_custom_char_P(c, &bitmaps[c*SIZEOF_BITMAP]);
}

void draw_spaces(IOStream& cout, uint16_t value, uint16_t max)
{
  for (uint16_t n = 10; n <= max; n *= 10)
    if (value < n) cout << ' ';
}

void draw_bar(uint8_t percent, uint8_t width)
{
  uint16_t bar = (percent * width) / 20;
  uint8_t full = bar / 5;
  uint8_t partial = bar % 5;
  uint8_t i = 0;
  for (; i < full; i++) lcd.putchar(4);
  if (partial > 0) {
    lcd.putchar(partial - 1);
    i += 1;
  }
  for (; i < width; i++) lcd.putchar(' ');
}

void setup()
{
  // Initiate watchdog (for timing), LCD driver and setup bar characters
  Watchdog::begin();
  lcd.begin();
  init_bar();
}

void loop()
{
  static uint16_t percent = 0;
  uint16_t value = keypad.sample();

  // Percent horizontal bar
  lcd.set_cursor(0, 0);
  draw_spaces(cout, percent, 100);
  cout << percent << PSTR("% ");
  draw_bar(percent, lcd.WIDTH - 5);
  percent += 2;
  if (percent > 100) percent = 0;

  // Analog sample (keypad) horizontal bar
  lcd.set_cursor(0, 1);
  draw_spaces(cout, value, 1023);
  cout << value << ' ';
  draw_bar(((value * 100L) + 100) >> 10, lcd.WIDTH - 5);

  delay(300);
}

