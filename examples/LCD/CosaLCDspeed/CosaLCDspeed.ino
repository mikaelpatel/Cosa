/**
 * @file CosaLCDspeed.ino
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
 * Benchmarking the LCD device drivers.
 *
 * @section Circuit
 * See HD44780.hh for description of LCD adapter circuits.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#undef measure

// Enable power down during testing and delay
// #define USE_LOW_POWER
#if defined(USE_LOW_POWER)
#define sleep(x)				\
  do {						\
    Power::all_disable();			\
    Watchdog::delay(x * 1024);			\
    Power::all_enable();			\
  } while (0)
#endif

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

// Benchmarks
typedef void (*benchmark_t)(uint16_t);
void clear_display(uint16_t nr);
void write_char(uint16_t nr);
void write_str(uint16_t nr);
void write_pstr(uint16_t nr);
void write_dec_uint16(uint16_t nr);
void write_bin_uint16(uint16_t nr);
void write_pos(uint16_t nr);

// Measurement support
void measure(str_P name, benchmark_t fn, uint16_t nr);
#undef MEASURE
#define MEASURE(fn,nr) measure(PSTR(#fn),fn,nr)

void setup()
{
  // Power::set(SLEEP_MODE_PWR_DOWN);
  RTT::begin();
  Watchdog::begin();
  lcd.begin();
  trace.begin(&lcd, PSTR("CosaLCDspeed:"));
#if defined(COSA_VLCD_HH)
  trace << PSTR("REV = ") << lcd.MAJOR << '.' << lcd.MINOR;
  sleep(2);
  trace << clear;
  trace << PSTR("WIDTH = ") << lcd.WIDTH << endl;
  trace << PSTR("HEIGHT = ") << lcd.HEIGHT;
#endif
  sleep(2);
}

void loop()
{
  MEASURE(clear_display, 10);
  MEASURE(write_char, 1000);
  MEASURE(write_str, 1000);
  MEASURE(write_pstr, 1000);
  MEASURE(write_dec_uint16, 10000);
  MEASURE(write_bin_uint16, 1000);
  MEASURE(write_pos, 100);
}

void clear_display(uint16_t nr)
{
  while (nr--)
    lcd.display_clear();
}

void write_char(uint16_t nr)
{
#if defined(COSA_HD44780_HH) || defined(COSA_VLCD_HH)
  const uint8_t WIDTH = 16;
  const uint8_t HEIGHT = 2;
#else
  const uint8_t WIDTH = lcd.WIDTH / (lcd.get_text_font()->WIDTH + lcd.get_text_font()->SPACING);
  const uint8_t HEIGHT = lcd.LINES;
#endif
  while (nr--) {
    char c = ' ' + (nr & 0x1f);
    for (uint8_t height = 0; height < HEIGHT; height++) {
      lcd.set_cursor(0, height);
      for (uint8_t width = 0; width < WIDTH; width++)
	lcd.putchar(c);
    }
  }
}

#if defined(COSA_PCD8544_HH)
# define NUM_STR   "12345678901234"
# define ALPHA_STR "ABCDEFGHIJKLMN"
#else
# define NUM_STR   "1234567890123456"
# define ALPHA_STR "ABCDEFGHIJKLMNOP"
#endif

void write_str(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 0);
    lcd.puts(NUM_STR);
    lcd.set_cursor(0, 0);
    lcd.puts(ALPHA_STR);
  }
}

void write_pstr(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 0);
    lcd.puts(PSTR(NUM_STR));
    lcd.set_cursor(0, 0);
    lcd.puts(PSTR(ALPHA_STR));
  }
}

void write_dec_uint16(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 0);
    trace << nr;
  }
}

void write_bin_uint16(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    trace << bin << nr;
  }
}

void write_pos(uint16_t nr)
{
#if defined(COSA_HD44780_HH) || defined(COSA_VLCD_HH)
  const uint8_t WIDTH = 16;
  const uint8_t HEIGHT = 2;
#else
  const uint8_t WIDTH = lcd.WIDTH / (lcd.get_text_font()->WIDTH + lcd.get_text_font()->SPACING);
  const uint8_t HEIGHT = lcd.LINES;
#endif
  while (nr--) {
    for (uint8_t c = 'A'; c < 'F'; c++) {
      for (uint8_t height = 0; height < HEIGHT; height++) {
	for (uint8_t width = 0; width < WIDTH; width++) {
	  lcd.set_cursor(width, height);
	  lcd.putchar(c);
	}
      }
    }
  }
}

void measure(str_P name, benchmark_t fn, uint16_t nr)
{
#if defined(USE_LOW_POWER)
  lcd.display_off();
  lcd.backlight_off();
#endif
  lcd.display_clear();
  uint32_t start = RTT::micros();
  fn(nr);
  uint32_t us = (RTT::micros() - start) / nr;
  uint32_t ops = 1000000L / us;
  lcd.display_clear();
  lcd.puts(name);
  trace << clear << name << endl;
  trace << ops << PSTR(" ops, ") << us << PSTR(" us");
#if defined(USE_LOW_POWER)
  lcd.backlight_on();
  lcd.display_on();
#endif
  sleep(4);
}
