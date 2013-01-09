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
#include "Cosa/Font.hh"

uint16_t 
Canvas::color(uint8_t red, uint8_t green, uint8_t blue)
{
  return ((((red >> 3) & 0x1f) << 11)  | 
	  (((green >> 2) & 0x3f) << 5) | 
	  ((blue >> 3) & 0x1f));
}

uint16_t 
Canvas::shade(uint16_t color, uint8_t scale)
{
  if (scale > 100) scale = 100;
  uint8_t blue = (scale * (color & 0x1fU)) / 100;
  color >>= 5;
  uint8_t green = (scale * (color & 0x3fU)) / 100;
  color >>= 6;
  uint8_t red = (scale * (color & 0x1fU)) / 100;
  return (((red & 0x1f) << 11)  | ((green & 0x3f) << 5) | (blue & 0x1f));
}

uint16_t 
Canvas::blend(uint16_t c1, uint16_t c2)
{
  uint8_t blue = (c1 + c2)/2;
  c1 >>= 5; c2 >>= 5; 
  uint8_t green = (c1 + c2)/2;
  c1 >>= 6; c2 >>= 6; 
  uint8_t red = (c1 + c2)/2;
  return (((red & 0x1f) << 11)  | ((green & 0x3f) << 5) | (blue & 0x1f));
}

void
Canvas::draw_bitmap(uint8_t x, uint8_t y, const uint8_t* bp, 
		    uint8_t width, uint8_t height,
		    uint8_t scale)
{
  for (uint8_t i = 0; i < width; i++) {
    uint8_t line = 0;
    for (uint8_t j = 0; j < height; j++) {
      if ((j & 0x7) == 0)
	line = pgm_read_byte(bp++);
      if (line & 0x1) {
	if (scale == 1)
	  draw_pixel(x + i, y + j);
	else {
	  fill_rect(x + i*scale, y + j*scale, scale, scale);
	} 
      }
      line >>= 1;
    }
  }
}

void
Canvas::draw_icon(uint8_t x, uint8_t y, const uint8_t* bp,
		  uint8_t width, uint8_t height,
		  uint8_t scale)
{
  for (uint8_t i = 0; i < height; i += 8) {
    for (uint8_t j = 0; j < width; j++) {
      uint8_t line = pgm_read_byte(bp++);
      for (uint8_t k = 0; k < 8; k++) {
	if (line & 1) {
	  if (scale == 1)
	    draw_pixel(x + j, y + k + i);
	  else {
	    fill_rect(x + j*scale, y + (k+i)*scale, scale, scale);
	  } 
	}
	line >>= 1;
      }
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

#define dist(x, y) ((x > y) ? (x - y) : (y - x))
#define swap(a, b) { uint8_t t = a; a = b; b = t; }

void 
Canvas::draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  uint8_t steep = (dist(y0, y1) > dist(x0, x1));
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  uint8_t dx, dy;
  dx = x1 - x0;
  dy = dist(y0, y1);
  int16_t err = dx / 2;
  int8_t ystep = (y0 < y1) ? 1 : -1;
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
Canvas::draw_poly(int dx, int dy, ...)
{
  va_list args;
  va_start(args, dy);
  while (dx != 0 || dy != 0) {
    uint8_t x = m_cursor.x + dx;
    uint8_t y = m_cursor.y + dy;
    draw_line(x, y);
    dx = va_arg(args, int);
    dy = va_arg(args, int);
  }
  va_end(args);
}

void 
Canvas::draw_poly_P(const int8_t* p)
{
  for (;;) {
    int8_t dx = pgm_read_byte(p++);
    int8_t dy = pgm_read_byte(p++);
    if (dx == 0 && dy == 0) return;
    uint8_t x = m_cursor.x + dx;
    uint8_t y = m_cursor.y + dy;
    draw_line(x, y);
  }
}

void 
Canvas::draw_circle(uint8_t x, uint8_t y, uint8_t radius)
{
  int16_t f = 1 - radius;
  int16_t dx = 1;
  int16_t dy = -2 * radius;
  int8_t rx = 0;
  int8_t ry = radius;

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
Canvas::fill_circle(uint8_t x, uint8_t y, uint8_t r)
{
  int16_t dx = 0, dy = r;
  int16_t p = 1 - r;

  while (dx <= dy) {
    draw_vertical_line(x + dx, y - dy, dy + dy);
    draw_vertical_line(x - dx, y - dy, dy + dy);
    draw_vertical_line(x + dy, y - dx, dx + dx);
    draw_vertical_line(x - dy, y - dx, dx + dx);
    dx++;
    if (p < 0)
      p = p + (dx << 1) + 1;
    else {
      dy--;
      p = p + ((dx - dy) << 1) + 1;
    }
  }
}

void 
Canvas::draw_char(char c)
{
  uint16_t color = m_pen_color;
  m_pen_color = m_text_color;
  m_font->draw(this, c, m_cursor.x, m_cursor.y, m_text_scale);
  m_cursor.x += m_text_scale * (m_font->WIDTH + CHAR_SPACING);
  if (m_cursor.x > m_text_port.width) {
    m_cursor.x = m_text_port.x;
    m_cursor.y += m_text_scale * (m_font->HEIGHT + LINE_SPACING);
    if (m_cursor.y > m_text_port.width) 
      m_cursor.y = m_text_port.y;
  }
  m_pen_color = color;
}

int 
Canvas::putchar(char c) 
{ 
  if (c >= ' ') 
    draw_char(c);
  if (c == '\n') {
    m_cursor.x = m_text_port.x;
    m_cursor.y += m_text_scale * (m_font->HEIGHT + LINE_SPACING);
    if (m_cursor.y > m_text_port.width) 
      m_cursor.y = m_text_port.y;
  }
  return (c);
}

void 
Canvas::run(uint8_t ix, PGM_VOID_P* tab, uint8_t max)
{
  if (ix >= max) return;
  const uint8_t* ip = (const uint8_t*) pgm_read_word(tab + ix);
  while (1) {
    switch (pgm_read_byte(ip++)) {
    case END_SCRIPT:
      return;
    case CALL_SCRIPT:
      ix = pgm_read_byte(ip++);
      if (ix >= max) return;
      run(ix, tab, max);
      break;
    case SET_CANVAS_COLOR:
      set_canvas_color(color(pgm_read_byte(ip++), 
			     pgm_read_byte(ip++), 
			     pgm_read_byte(ip++)));
      break;
    case SET_PEN_COLOR:
      set_pen_color(color(pgm_read_byte(ip++), 
			  pgm_read_byte(ip++), 
			  pgm_read_byte(ip++)));
      break;
    case SET_TEXT_COLOR:
      set_text_color(color(pgm_read_byte(ip++), 
			   pgm_read_byte(ip++), 
			   pgm_read_byte(ip++)));
      break;
    case SET_TEXT_SCALE:
      set_text_scale(pgm_read_byte(ip++));
      break;
    case SET_TEXT_PORT:
      set_text_port(pgm_read_byte(ip++),
		    pgm_read_byte(ip++),
		    pgm_read_byte(ip++),
		    pgm_read_byte(ip++));
      break;
    case SET_TEXT_FONT:
      ix = pgm_read_byte(ip++);
      if (ix >= max) return;
      set_text_font((Font*) pgm_read_word(tab + ix));
      break;
    case SET_CURSOR:
      set_cursor(pgm_read_byte(ip++), pgm_read_byte(ip++));
      break;
    case MOVE_CURSOR:
      move_cursor(pgm_read_byte(ip++), pgm_read_byte(ip++));
      break;
    case DRAW_BITMAP:
      ix = pgm_read_byte(ip++);
      if (ix >= max) return;
      draw_bitmap((const uint8_t*) pgm_read_word(tab + ix),
		  pgm_read_byte(ip++), 
		  pgm_read_byte(ip++),
		  pgm_read_byte(ip++));
      break;
    case DRAW_ICON:
      ix = pgm_read_byte(ip++);
      if (ix >= max) return;
      draw_icon((const uint8_t*) pgm_read_word(tab + ix), 
		pgm_read_byte(ip++));
      break;
    case DRAW_PIXEL:
      draw_pixel();
      break;
    case DRAW_LINE:
      draw_line(pgm_read_byte(ip++), pgm_read_byte(ip++));
      break;
    case DRAW_POLY:
      ix = pgm_read_byte(ip++);
      if (ix >= max) return;
      draw_poly_P((const int8_t*) pgm_read_word(tab + ix));
      break;
    case DRAW_RECT:
      draw_rect(pgm_read_byte(ip++), pgm_read_byte(ip++));
      break;
    case FILL_RECT:
      fill_rect(pgm_read_byte(ip++), pgm_read_byte(ip++));
      break;
    case FILL_SCREEN:
      fill_screen();
      break;
    case DRAW_CIRCLE:
      draw_circle(pgm_read_byte(ip++));
      break;
    case FILL_CIRCLE:
      fill_circle(pgm_read_byte(ip++));
      break;
    case DRAW_CHAR:
      draw_char(pgm_read_byte(ip++));
      break;
    case DRAW_STRING:
      ix = pgm_read_byte(ip++);
      if (ix >= max) return;
      draw_string_P((const char*) pgm_read_word(tab + ix));
      break;
    default:
      return;
    }
  }
}

