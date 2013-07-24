/**
 * @file CosaLCDbench.ino
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
 * Benchmarking the LCD device drivers towards New LiquidCrystal
 * Library (https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/).
 * This is a Cosa version of performanceLCD.pde.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/LCD/Driver/HD44780.hh"

// Select the LCD port for the benchmark
// HD44780::Port port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
HD44780::MJKDZ port;
// HD44780::DFRobot port;
HD44780 lcd(&port);

// Benchmarks
typedef void (*benchmark_t)(uint16_t);
void benchmark1(uint16_t nr);
void benchmark2(uint16_t nr);
void benchmark3(uint16_t nr);
void benchmark4(uint16_t nr);

// Measurement support
void measure(const char* name, benchmark_t fn, uint16_t nr, uint16_t bytes);
#define MEASURE(fn,bytes)						\
  do {									\
    measure(PSTR(#fn),fn,10,bytes);					\
    SLEEP(4);								\
  } while (0)

void setup()
{
  RTC::begin();
  Watchdog::begin();
  lcd.begin();
  trace.begin(&lcd, PSTR("CosaLCDbench: "));
  SLEEP(2);
}

// Display configuration
const uint16_t WIDTH = 16;
const uint16_t HEIGHT = 2;

void loop()
{
  MEASURE(benchmark1, WIDTH * HEIGHT + 2);
  MEASURE(benchmark2, WIDTH * HEIGHT * 6 * 2);
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
    for (uint8_t c = 'A'; c <= 'A' + 5; c++) {
      for (uint8_t height = 0; height < HEIGHT; height++) {
	for (uint8_t width = 0; width < WIDTH; width++) {
	  lcd.set_cursor(width, height);
	  lcd.putchar(c);
	}
      }
    }
  }
}

#define NUM_STR   "1234567890123456"
#define ALPHA_STR "ABCDEFGHIJKLMNOP"

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
    lcd.puts_P(PSTR(NUM_STR));
    lcd.set_cursor(0, 1);
    lcd.puts_P(PSTR(ALPHA_STR));
  }
}

void measure(const char* name, benchmark_t fn, uint16_t nr, uint16_t bytes)
{
  lcd.display_clear();
  lcd.puts_P(name);
  uint32_t start = RTC::micros();
  {
    fn(nr);
  }
  uint32_t us = (RTC::micros() - start) / nr;
  trace << clear << name << endl;
  trace << us << PSTR(" us (") << us / bytes << PSTR(")");
}

