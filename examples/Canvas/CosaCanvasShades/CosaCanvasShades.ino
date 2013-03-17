/**
 * @file CosaCanvasShades.ino
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
 * Cosa demonstration of Cosa Canvas color shades on ST7735R, 
 * 262K Color Single-Chip TFT Controller. 
 *
 * @section Circuit
 * Connect Arduino to ST7735R Module (Arduino ==> HY-1.8 SPI):
 * GND ==> GND(1), VCC(5V) ==> VCC(2), RST ==> RESET(6),
 * D9 ==> A0(7), MOSI/D11 ==> SDA(8), SCK/D13 ==> SCK(9),
 * SS/D10 ==> CS(10), VCC(5V) ==> LED+(15), GND ==> LED-(16)    
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/SPI/Driver/ST7735R.hh"

ST7735R tft;

void setup()
{
  Watchdog::begin();
  tft.begin();
  tft.set_canvas_color(tft.BLACK);
  tft.fill_screen();
}

void draw_shade(uint16_t color)
{
  uint16_t base = color;
  for (uint8_t x = 75; x > 0; x -= 25) {
    tft.set_pen_color(tft.WHITE);
    tft.draw_rect(9, 9, tft.WIDTH - 19, tft.HEIGHT - 19);
    for (uint8_t y = 10; y < tft.HEIGHT - 10; y += 4) {
      uint8_t level = ((y - 10) * 100L) / (tft.HEIGHT - 10);
      tft.set_pen_color(tft.shade(color, level));
      tft.fill_rect(10, y, tft.WIDTH - 20, 4);
    }
    SLEEP(1);
    color = tft.shade(base, x);
  }
}

void loop()
{
  static uint8_t direction = Canvas::PORTRAIT;
  draw_shade(tft.WHITE);
  draw_shade(tft.RED);
  draw_shade(tft.GREEN);
  draw_shade(tft.BLUE);
  draw_shade(tft.YELLOW);
  draw_shade(tft.CYAN);
  draw_shade(tft.MAGENTA);
  tft.set_orientation(direction);
  direction = !direction;
}
