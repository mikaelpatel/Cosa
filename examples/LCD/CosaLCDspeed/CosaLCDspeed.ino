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
#include "Cosa/LCD/Driver/HD44780.hh"
HD44780::MJKDZ port;
// HD44780::DFRobot port;
// HD44780::Port port;
HD44780 lcd(&port);

// #include "Cosa/LCD/Driver/PCD8544.hh"
// PCD8544 lcd;

// #include "Cosa/LCD/Driver/ST7565.hh"
// ST7565 lcd;

#undef putchar

// Benchmarks
typedef void (*benchmark_t)(uint16_t);
void clear_display(uint16_t nr);
void write_char(uint16_t nr);
void write_str(uint16_t nr);
void write_pstr(uint16_t nr);
void write_dec_uint16(uint16_t nr);
void write_bin_uint16(uint16_t nr);

// Measurement support
void measure(const char* name, benchmark_t fn, uint16_t nr);
#define MEASURE(fn,nr) measure(PSTR(#fn),fn,nr)

void setup()
{
  RTC::begin();
  Watchdog::begin();
  lcd.begin();
  trace.begin(&lcd, PSTR("CosaLCDspeed:"));
  SLEEP(1);
}

void loop()
{
  MEASURE(clear_display, 10);
  SLEEP(4);
  MEASURE(write_char, 1000);
  SLEEP(4);
  MEASURE(write_str, 1000);
  SLEEP(4);
  MEASURE(write_pstr, 1000);
  SLEEP(4);
  MEASURE(write_dec_uint16, 10000);
  SLEEP(4);
  MEASURE(write_bin_uint16, 1000);
  SLEEP(4);
}

void clear_display(uint16_t nr)
{
  while (nr--)
    lcd.display_clear();
}

void write_char(uint16_t nr)
{
#if defined(__COSA_LCD_DRIVER_HD44780_HH__)
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

void write_str(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    lcd.puts("12345678901234");
    lcd.set_cursor(0, 1);
    lcd.puts("ABCDEFGHIJKLMN");
  }
}

void write_pstr(uint16_t nr)
{
  while (nr--) {
    lcd.set_cursor(0, 1);
    lcd.puts_P(PSTR("12345678901234"));
    lcd.set_cursor(0, 1);
    lcd.puts_P(PSTR("ABCDEFGHIJKLMN"));
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

