/**
 * @file CosaLCDbench.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Benchmarking the LCD device drivers towards New LiquidCrystal
 * Library (https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/).
 * This is a Cosa version of performanceLCD.pde.
 *
 * @section Circuit
 * See LCD header files for description of LCD adapter circuits.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream.hh"

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

// Connect IOStream to LCD
IOStream cout(&lcd);

// Display configuration
#if defined(COSA_PCD8544_HH)
const uint16_t WIDTH = 14;
const uint16_t HEIGHT = 2;
#else
const uint16_t WIDTH = 16;
const uint16_t HEIGHT = 2;
#endif

// Measurement support
typedef void (*benchmark_t)(uint16_t);
void measure(str_P name, benchmark_t fn, uint16_t nr, uint16_t bytes);
#define MEASURE(fn,bytes) measure(PSTR(#fn),fn,10,bytes)

// The benchmarks
void benchmark1(uint16_t nr);
void benchmark2(uint16_t nr);
void benchmark3(uint16_t nr);
void benchmark4(uint16_t nr);

// Benchmark configuration
#define CHAR_MAX 6
#define NUM_STR   "1234567890123456"
#define ALPHA_STR "ABCDEFGHIJKLMNOP"

void setup()
{
  RTT::begin();
  Watchdog::begin();
#if defined(COSA_ADAFRUIT_I2C_LCD_BACKPACK_H)
  twi.set_freq(TWI::MAX_FREQ);
#endif
  lcd.begin();
  cout << PSTR("CosaLCDbench:");
  sleep(2);
}

void loop()
{
  MEASURE(benchmark1, WIDTH * HEIGHT + 2);
  MEASURE(benchmark2, WIDTH * HEIGHT * CHAR_MAX * 2);
  MEASURE(benchmark3, WIDTH * HEIGHT + 2);
  MEASURE(benchmark4, WIDTH * HEIGHT + 2);
}

void benchmark1(uint16_t nr)
{
  while (nr--) {
    char c = ' ' + (nr & 0x1f);
    for (uint8_t height = 0; height < HEIGHT; height++) {
      lcd.set_cursor(0, height);
      for (uint8_t width = 0; width < WIDTH; width++)
	lcd.putchar(c);
    }
  }
}

void benchmark2(uint16_t nr)
{
  while (nr--) {
    for (uint8_t c = 'A'; c < 'A' + CHAR_MAX; c++) {
      for (uint8_t height = 0; height < HEIGHT; height++) {
	for (uint8_t width = 0; width < WIDTH; width++) {
	  lcd.set_cursor(width, height);
	  lcd.putchar(c);
	}
      }
    }
  }
}

void benchmark3(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    lcd.puts(NUM_STR);
    lcd.set_cursor(0, 1);
    lcd.puts(ALPHA_STR);
  }
}

void benchmark4(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    lcd.puts(PSTR(NUM_STR));
    lcd.set_cursor(0, 1);
    lcd.puts(PSTR(ALPHA_STR));
  }
}

void measure(str_P name, benchmark_t fn, uint16_t nr, uint16_t bytes)
{
  cout << clear << name;
  uint32_t start = RTT::micros();
  {
    fn(nr);
  }
  uint32_t us = (RTT::micros() - start) / nr;
  cout << clear << name << endl;
  cout << us << PSTR(" us (") << us / bytes << PSTR(")");
  sleep(4);
}
