/**
 * @file CosaST7565.ino
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
 * Demonstration of the ST7565 device driver with mapping to
 * IOStream::Device.
 *
 * @section Circuit
 * @code
 *                       ST7565/Serial3W
 *                       +------------+
 *                     1-|DB0         |
 *                     2-|DB1         |
 *                     3-|DB2         |
 *                     4-|DB3         |
 *                     5-|DB4         |
 *                     6-|DB5         |
 * (D7/D1)-------------7-|DB6(SCL)    |
 * (D6/D0)-------------8-|DB7(SI)     |
 * (VCC)---------------9-|VDD         |
 * (GND)--------------10-|VSS         |
 * (VCC)---|220|------11-|A           |
 * (D9/D3)------------12-|CS          |
 * (RST)--------------13-|RST         |
 * (D8/D2)------------14-|DC          |
 *                    15-|WR(R/W)     |
 *                    16-|RD(E)       |
 *                       +------------+
 *
 *                        ST7565/SPI3W
 *                       +------------+
 *                     1-|DB0         |
 *                     2-|DB1         |
 *                     3-|DB2         |
 *                     4-|DB3         |
 *                     5-|DB4         |
 *                     6-|DB5         |
 * (SCK/D13/D4)--------7-|DB6(SCL)    |
 * (MOSI/D11/D5)-------8-|DB7(SI)     |
 * (VCC)---------------9-|VDD         |
 * (GND)--------------10-|VSS         |
 * (VCC)---|220|------11-|A           |
 * (D9/D3)------------12-|CS          |
 * (RST)--------------13-|RST         |
 * (D8/D2)------------14-|DC          |
 *                    15-|WR(R/W)     |
 *                    16-|RD(E)       |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/AnalogPin.hh"

#include <Canvas.h>
#include "Canvas/OffScreen.hh"
#include "Canvas/Icon/arduino_icon_64x32.h"
#include "Canvas/Icon/arduino_icon_96x32.h"

#include <Font.h>
#include "FixedNums8x16.hh"

// Select ST7565 IO Adapter; Serial Output Pins or SPI
#include <ST7565.h>
// LCD::Serial3W port;
LCD::SPI3W port;
ST7565 lcd(&port);

void setup()
{
  // Set up watchdog for low power sleep
  Watchdog::begin();

  // Initiate the LCD screen and show arduino icon
  lcd.begin();
  lcd.set_cursor((lcd.WIDTH - 64)/2, 1);
  lcd.draw_icon(arduino_icon_64x32);

  // Use LCD bind to trace and use inverted text mode for banner
  trace.begin(&lcd);
  lcd.set_cursor(0, lcd.LINES - 1);
  trace << PSTR("CosaST7565: started");
  sleep(2);

  // Dump characters in system font
  trace << PSTR("\f\aSYSTEM FONT 5x7\a\n");
  for (uint8_t c = 0; c < 128; c++) {
    if ((c == '\n') || (c == '\f') || (c == '\b') || (c == '\a') || (c == '\t'))
      trace << ' ';
    else
      trace << (char) c;
  }
  sleep(2);

  // Use the trace iostream onto the LCD with output operator
  trace << PSTR("\f\aSPECIAL CHARACTERS\a\n");
  trace << PSTR("\aa\a - alert\n");
  trace << PSTR("\ab\a - backspace\n");
  trace << PSTR("\af\a - form-feed\n");
  trace << PSTR("\an\a - new-line\n");
  trace << PSTR("\at\a - tab\n\n");
  trace << PSTR("01\t23\t45\t67\t89");
  for (uint8_t i = 0; i < 22; i++) {
    Watchdog::delay(256);
    trace << PSTR("\b \b");
  }

  // Scrolling
  trace << PSTR("\f\aSCROLLING\a\n");
  static const char msg[] __PROGMEM =
    "The quick brown fox jumps over the lazy dog. ";
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t len = strlen_P(msg);
    for (uint8_t j = 0; j < len; j++) {
      trace << (char) pgm_read_byte(msg + j);
      Watchdog::delay(64);
    }
  }
  sleep(2);

  // Use number base handling
  trace << PSTR("\f\aNUMBER BASE\a\n");
  uint16_t millis = Watchdog::millis();
  TRACE(millis);
  trace << PSTR("bcd = ") << bcd << 0x55 << endl;
  trace << PSTR("bin = ") << bin << 0x55 << endl;
  trace << PSTR("oct = ") << oct << 0x55 << endl;
  trace << PSTR("dec = ") << dec << 0x55 << endl;
  trace << PSTR("hex = ") << hex << 0x55 << endl;
  trace << PSTR("ptr = ") << &lcd;
  sleep(2);

  // Dump the LCD object raw format with normal print function
  trace << PSTR("\f\aBUFFER DUMP\a\n");
  trace.print(&lcd, sizeof(lcd), IOStream::hex, 5);
  sleep(2);

  // Play around with the offscreen canvas
  OffScreen<ST7565::WIDTH, ST7565::HEIGHT> offscreen;
  offscreen.begin();
  offscreen.draw_rect(0, 0, 10, 10);
  offscreen.fill_rect(2, 2, 7, 7);
  offscreen.draw_rect(20, 20, 10, 10);
  offscreen.draw_circle(20, 20, 10);
  offscreen.fill_circle(20, 20, 8);
  offscreen.draw_roundrect(70, 10, 40, 20, 10);
  offscreen.fill_roundrect(72, 12, 36, 16, 7);
  offscreen.draw_rect(70, 10, 40, 20);
  offscreen.draw_rect(0, 0, offscreen.WIDTH - 1, offscreen.HEIGHT - 1);
  for (uint8_t width = 0; width < ST7565::WIDTH; width += 8) {
    offscreen.draw_line(20, 20, width, ST7565::HEIGHT - 1);
  }
  offscreen.set_cursor(15, 2);
  offscreen.draw_string(PSTR("OffScreen Canvas"));

  // Draw the off-screen canvas on the LCD
  lcd.set_cursor(0, 0);
  lcd.draw_bitmap(offscreen.bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
  sleep(4);
}

void loop()
{
  // Draw bars with analog sample values (A0..A5)
  lcd.putchar('\f');
  for (uint8_t i = 0; i < membersof(analog_pin_map); i++) {
    trace.printf(PSTR("A%d:"), i);
    Board::AnalogPin pin;
    pin = (Board::AnalogPin) pgm_read_byte(analog_pin_map + i);
    uint8_t percent = (AnalogPin::sample(pin) * 100L) / 1023;
    lcd.draw_bar(percent, ST7565::WIDTH - 20);
    if (i != ST7565::LINES - 1) trace << endl;
  }
  sleep(1);

  // Every 4 seconds display the arduino icon and count down time
  static const uint8_t SHOW_BANNER = 4;
  static uint8_t banner = 0;
  if (++banner == SHOW_BANNER) {

    // Display the Arduino icon
    lcd.putchar('\f');
    lcd.set_cursor((lcd.WIDTH - 96)/2, 1);
    lcd.display_inverse();
    lcd.draw_icon(arduino_icon_96x32);
    sleep(2);

    lcd.display_normal();
    banner = 0;

    // Counters
    static uint8_t min = 30;
    static uint8_t sec = 00;

    // Draw the current counter value off-screen
    OffScreen<ST7565::WIDTH, ST7565::HEIGHT> offscreen;
    offscreen.begin();
    offscreen.set_text_font(&fixednums8x16);
    offscreen.draw_roundrect(8, 8, lcd.WIDTH - 18, lcd.HEIGHT - 18, 8);
    offscreen.set_cursor((lcd.WIDTH - ((fixednums8x16.WIDTH + fixednums8x16.SPACING) * 5))/2,
			 (lcd.HEIGHT - fixednums8x16.WIDTH - fixednums8x16.SPACING)/2);
    offscreen.draw_char('0' + min/10);
    offscreen.draw_char('0' + min%10);
    offscreen.draw_char(':');
    offscreen.draw_char('0' + sec/10);
    offscreen.draw_char('0' + sec%10);

    // Draw the off-screen canvas on the LCD
    lcd.putchar('\f');
    lcd.draw_bitmap(offscreen.bitmap(), offscreen.WIDTH, offscreen.HEIGHT);
    // Decrement counter
    if (sec == 0) {
      if (min != 00) {
	sec = 50;
	min -= 1;
      }
    } else
      sec -= 10;
    sleep(4);
  }
}
