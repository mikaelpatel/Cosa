/**
 * @file CosaCanvasClock.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa demonstration of Canvas Segment Font handling and device 
 * driver for ST7735, 262K Color Single-Chip TFT Controller.
 *
 * @section Circuit
 * Connect Arduino to ST7735 Module (Arduino ==> HY-1.8 SPI):
 * GND ==> GND(1), VCC(5V) ==> VCC(2), RST ==> RESET(6),
 * D9 ==> A0(7), MOSI/D11 ==> SDA(8), SCK/D13 ==> SCK(9),
 * SS/D10 ==> CS(10), VCC(5V) ==> LED+(15), GND ==> LED-(16)    
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/SPI/Driver/ST7735.hh"
#include "Cosa/Canvas/Font/Segment32x50.hh"

ST7735 tft;
const Canvas::color16_t BACKGROUND = tft.shade(Canvas::RED, 25);
const Canvas::color16_t TEXT_COLOR = tft.shade(Canvas::RED, 75);
uint8_t min = 30;
uint8_t sec = 00;

void setup()
{
  // Start the watchdog for low power sleep
  Watchdog::begin(1024, SLEEP_MODE_IDLE, Watchdog::push_watchdog_event);

  // Initiate the display and set orientation and color scheme
  tft.begin();
  tft.set_canvas_color(Canvas::BLACK);
  tft.set_orientation(Canvas::LANDSCAPE);
  tft.set_text_font(&segment32x50);
  tft.set_text_color(TEXT_COLOR);
  tft.fill_screen();

  // Draw a filled boundary box using the round rectangle
  Canvas::color16_t saved = tft.set_pen_color(BACKGROUND);
  tft.fill_roundrect(0, 20, tft.WIDTH - 1, 70, 12);
  tft.set_pen_color(TEXT_COLOR);
  tft.draw_roundrect(0, 20, tft.WIDTH - 1, 70, 12);
  tft.set_pen_color(saved);

  // Draw initial time
  tft.set_cursor(8, 30);
  tft.draw_char('0' + min/10);
  tft.draw_char('0' + min%10);
  tft.move_cursor(-12, 0);
  tft.draw_char(':');
  tft.move_cursor(-8, 0);
  tft.draw_char('0' + sec/10);
  tft.draw_char('0' + sec%10);
}

void draw_digit(uint8_t pos, uint8_t digit)
{
  uint8_t x, y;
  uint8_t adjust = (pos < 2 ? 0 : segment32x50.WIDTH - 17);
  tft.set_cursor(8 + (pos * segment32x50.WIDTH) + adjust, 30);
  Canvas::color16_t saved = tft.set_pen_color(BACKGROUND);
  tft.get_cursor(x, y);
  tft.fill_rect(x, y, segment32x50.WIDTH, segment32x50.HEIGHT);
  tft.set_pen_color(saved);
  tft.draw_char('0' + digit);
}

void loop()
{
  // Receive a watchdog event every second
  Event event;
  Event::queue.await(&event);

  // Decrement counter
  if (sec == 0) {
    if (min != 00) {
      sec = 59;
      min -= 1;
    }
  } else {
    sec -= 1;
  }

  // Draw the digits that are updated (to avoid flicker)
  draw_digit(3, sec%10);
  if ((sec%10) != 9) return;
  draw_digit(2, sec/10);
  if ((sec/10) != 5) return;
  draw_digit(1, min%10);
  if ((min%10) != 9) return;
  draw_digit(0, min/10);
}
