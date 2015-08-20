/**
 * @file CosaLCDvbar.ino
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
 * vertical bar.
 *
 * @section Circuit
 * Designed for a LCD Keypad shield with 16x2 HD44780 LCD and six
 * buttons. Samples and displays analog pin 0..5.
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
 * (VCC)--------------15-|A           |
 * (GND)--------------16-|K           |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Math.hh"
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

// #include <HD44780.h>
// #include <ST7920.h>
// HD44780::Port4b port;
// ST7920 lcd(&port);

// #include <PCD8544.h>
// LCD::Serial3W port;
// LCD::SPI3W port;
// PCD8544 lcd(&port);

// #include <ST7565.h>
// LCD::Serial3W port;
// LCD::SPI3W port;
// ST7565 lcd(&port);

// #include <VLCD.h>
// VLCD lcd;

IOStream cout(&lcd);

// Initiate vertical bar (bottom to top, 1..7)
void init_bar()
{
  static const uint8_t bitmaps[] __PROGMEM = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b00000,
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b00000,
    0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b00000,
    0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000,
    0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000,
    0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000,
    0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000
  };
  const uint8_t SIZEOF_BITMAP = 8;
  const uint8_t BITMAPS_MAX = membersof(bitmaps) / SIZEOF_BITMAP;
  for (char c = 0; c < BITMAPS_MAX; c++)
    lcd.set_custom_char_P(c, &bitmaps[c*SIZEOF_BITMAP]);
}

void draw_vertical_bar(uint8_t x, uint8_t percent)
{
  uint8_t c;
  lcd.set_cursor(x, 0);
  if (percent < 50) {
    lcd.putchar(' ');
    lcd.set_cursor(x, 1);
    c = percent / 7;
    if (c == 0) c = ' '; else c = c - 1;
    lcd.putchar(c);
  }
  else {
    c = (percent - 50) / 7;
    if (c == 0) c = ' '; else c = c - 1;
    lcd.putchar(c);
    lcd.set_cursor(x, 1);
    lcd.putchar(6);
  }
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
  // Sample analog pins and draw vertical bar
  cout << PSTR("\fSample: ") << endl;
  cout << AnalogPin::sample(Board::A0);
  for (uint8_t i = 0; i < membersof(analog_pin_map); i++) {
    Board::AnalogPin pin;
    pin = (Board::AnalogPin) pgm_read_byte(analog_pin_map + i);
    uint16_t value = AnalogPin::sample(pin);
    uint8_t x = i + 8;
    draw_vertical_bar(x, map<uint32_t,0,1023,0,100>(value));
  }

  delay(300);
}

