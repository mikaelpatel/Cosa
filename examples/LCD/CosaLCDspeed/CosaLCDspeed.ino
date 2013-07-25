/**
 * @file CosaLCDspeed.ino
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
 * Benchmarking the LCD device drivers.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

// Select the LCD device for the benchmark
// #include "Cosa/LCD/Driver/PCD8544.hh"
// PCD8544 lcd;

// #include "Cosa/LCD/Driver/ST7565.hh"
// ST7565 lcd;

// #include "Cosa/VLCD.hh"
// VLCD lcd;

#include "Cosa/LCD/Driver/HD44780.hh"
// Select the HD44780 port adapter for the benchmark
// HD44780::Port port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::MJKDZ port;
HD44780::DFRobot port;
HD44780 lcd(&port);

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
void measure(const char* name, benchmark_t fn, uint16_t nr);
#define MEASURE(fn,nr)						\
  do {								\
    measure(PSTR(#fn),fn,nr);					\
    SLEEP(4);							\
  } while (0)

void setup()
{
  RTC::begin();
  Watchdog::begin();
  lcd.begin();
  trace.begin(&lcd, PSTR("CosaLCDspeed:"));
#if defined(__COSA_VLCD_HH__)
  trace << PSTR("REV = ") << lcd.MAJOR << '.' << lcd.MINOR;
  SLEEP(2);
  trace << clear;
  trace << PSTR("WIDTH = ") << lcd.WIDTH << endl;
  trace << PSTR("HEIGHT = ") << lcd.HEIGHT;
#endif
  SLEEP(2);
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
#if defined(__COSA_LCD_DRIVER_HD44780_HH__) || defined(__COSA_VLCD_HH__) 
  const uint8_t WIDTH = 16;
  const uint8_t HEIGHT = 2;
#else
  const uint8_t WIDTH = lcd.WIDTH / lcd.get_text_font()->get_width(' ');
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

#ifdef __COSA_LCD_DRIVER_PCD8544_HH__
# define NUM_STR   "12345678901234"
# define ALPHA_STR "ABCDEFGHIJKLMN"
#else
# define NUM_STR   "1234567890123456"
# define ALPHA_STR "ABCDEFGHIJKLMNOP"
#endif

void write_str(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    lcd.puts(NUM_STR);
    lcd.set_cursor(0, 1);
    lcd.puts(ALPHA_STR);
  }
}

void write_pstr(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    lcd.puts_P(PSTR(NUM_STR));
    lcd.set_cursor(0, 1);
    lcd.puts_P(PSTR(ALPHA_STR));
  }
}

void write_dec_uint16(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
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
#if defined(__COSA_LCD_DRIVER_HD44780_HH__) || defined(__COSA_VLCD_HH__) 
  const uint8_t WIDTH = 16;
  const uint8_t HEIGHT = 2;
#else
  const uint8_t WIDTH = lcd.WIDTH / lcd.get_text_font()->get_width(' ');
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

void measure(const char* name, benchmark_t fn, uint16_t nr)
{
  lcd.display_clear();
  lcd.puts_P(name);
  uint32_t start = RTC::micros();
  fn(nr);
  uint32_t us = (RTC::micros() - start) / nr;
  uint32_t ops = 1000000L / us;
  trace << clear << name << endl;
  trace << ops << PSTR(" ops, ") << us << PSTR(" us");
}

