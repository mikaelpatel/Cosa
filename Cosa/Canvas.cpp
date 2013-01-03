/**
 * @file Cosa/Canvas.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Virtual Canvas device/IOStream; abstraction of small screens,
 * LCD/TFT. See Cosa/SPI/ST7735R.hh for an example of usage.
 *
 * @section Limitations
 * Color model is 16-bit RBG<5,6,5>.
 *
 * @section Acknowledgement
 * Inspired by graphics library by ladyada/adafruit.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas.hh"

void
Canvas::draw_bitmap(uint8_t x, uint8_t y, const uint8_t* bp, 
		    uint8_t width, uint8_t height)
{
  for (uint8_t i = 0; i < width; i++) {
    uint8_t line = 0;
    for (uint8_t j = 0; j < height; j++) {
      if ((j & 0x7) == 0)
	line = pgm_read_byte(bp++);
      if (line & 0x1) {
	if (m_scale == 1)
	  draw_pixel(x + i, y + j);
	else {
	  fill_rect(x + i*m_scale, y + j*m_scale, m_scale, m_scale);
	} 
      }
      line >>= 1;
    }
  }
}

void
Canvas::draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  draw_horizontal_line(x, y, width);
  draw_vertical_line(x + width, y, height);
  draw_vertical_line(x, y, height);
  draw_horizontal_line(x, y + height, width);
}

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

void 
Canvas::draw_line(uint8_t _x0, uint8_t _y0, uint8_t _x1, uint8_t _y1)
{
  int16_t x0 = _x0;
  int16_t y0 = _y0;
  int16_t x1 = _x1;
  int16_t y1 = _y1; 

  uint16_t steep = (abs(y1 - y0) > abs(x1 - x0));
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      draw_pixel(y0, x0);
    } else {
      draw_pixel(x0, y0);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void 
Canvas::draw_circle(uint8_t x, uint8_t y, uint8_t radius)
{
  int16_t f = 1 - radius;
  int16_t dx = 1;
  int16_t dy = -2 * radius;
  int16_t rx = 0;
  int16_t ry = radius;

  draw_pixel(x, y + radius);
  draw_pixel(x, y - radius);
  draw_pixel(x + radius, y);
  draw_pixel(x - radius, y);
  while (rx < ry) {
    if (f >= 0) {
      ry--;
      dy += 2;
      f += dy;
    }
    rx++;
    dx += 2;
    f += dx;
    draw_pixel(x + rx, y + ry);
    draw_pixel(x - rx, y + ry);
    draw_pixel(x + rx, y - ry);
    draw_pixel(x - rx, y - ry);
    draw_pixel(x + ry, y + rx);
    draw_pixel(x - ry, y + rx);
    draw_pixel(x + ry, y - rx);
    draw_pixel(x - ry, y - rx);
  }
}

void 
Canvas::fill_circle(uint8_t x, uint8_t y, uint8_t radius)
{
  int16_t f = 1 - radius;
  int16_t dx = 1;
  int16_t dy = -2 * radius;
  int16_t rx = 0;
  int16_t ry = radius;

  draw_vertical_line(x, y - radius, 2*radius + 1);
  while (rx < ry) {
    if (f >= 0) {
      ry--;
      dy += 2;
      f += dy;
    }
    rx++;
    dx += 2;
    f += dx;
    draw_vertical_line(x + rx, y - ry, 2*ry + 1);
    draw_vertical_line(x - rx, y - ry, 2*ry + 1);
    draw_vertical_line(x + ry, y - rx, 2*rx + 1);
    draw_vertical_line(x - ry, y - rx, 2*rx + 1);
  }
}

void 
Canvas::draw_char(char c)
{
  uint16_t color = m_pen_color;
  m_pen_color = m_text_color;
  draw_bitmap(m_x, m_y, m_font->get_bitmap(c), m_font->WIDTH, m_font->HEIGHT);
  m_x += m_scale * (m_font->WIDTH + CHAR_SPACING);
  if (m_x > SCREEN_WIDTH) {
    m_x = 0;
    m_y += m_scale * (m_font->HEIGHT + LINE_SPACING);
    if (m_y > SCREEN_HEIGHT) m_y = 0;
  }
  m_pen_color = color;
}

int 
Canvas::putchar(char c) 
{ 
  if (c >= ' ') 
    draw_char(c);
  if (c == '\n') {
    m_x = 0;
    m_y += m_scale * (m_font->HEIGHT + LINE_SPACING);
    if (m_y > SCREEN_HEIGHT) m_y = 0;
  }
  return (1);
}
