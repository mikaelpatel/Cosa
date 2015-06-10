/**
 * @file Canvas.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Font.hh"
#include "System5x7.hh"
#include "Canvas.hh"

Canvas::Context Canvas::context;

Canvas::color16_t
Canvas::shade(color16_t color, uint8_t scale)
{
  color16_t res;
  if (UNLIKELY(scale > 100)) scale = 100;
  res.red = (scale * color.red) / 100;
  res.green = (scale * color.green) / 100;
  res.blue = (scale * color.blue) / 100;
  return (res);
}

Canvas::color16_t
Canvas::blend(color16_t c1, color16_t c2)
{
  color16_t res;
  res.red = (c1.red + c2.red) / 2;
  res.green = (c1.green + c2.green) / 2;
  res.blue = (c1.blue + c2.blue) / 2;
  return (res);
}

uint8_t
Canvas::get_orientation()
{
  return (m_direction);
}

uint8_t
Canvas::set_orientation(uint8_t direction)
{
  uint8_t previous = m_direction;
  m_direction = (direction & 1);
  return (previous);
}

void
Canvas::draw_pixel(uint16_t x, uint16_t y)
{
  fill_rect(x, y, 1, 1);
}

void
Canvas::draw_bitmap(uint16_t x, uint16_t y, const uint8_t* bp,
		    uint16_t width, uint16_t height,
		    uint8_t scale)
{
  if (scale == 1) {
    for (uint16_t i = 0; i < height; i += 8) {
      for (uint16_t j = 0; j < width; j++) {
	uint8_t bits = pgm_read_byte(bp++);
	if (bits == 0xff) {
	  draw_vertical_line(x + j, y + i, CHARBITS);
	}
	else {
	  for (uint8_t k = 0; k < 8; k++) {
	    if (bits == 0) break;
	    if (bits & 1) draw_pixel(x + j, y + k + i);
	    bits >>= 1;
	  }
	}
      }
    }
  }
  else {
    for (uint16_t i = 0; i < height; i += 8) {
      for (uint16_t j = 0; j < width; j++) {
	uint8_t bits = pgm_read_byte(bp++);
	for (uint8_t k = 0; k < 8; k++) {
	  if (bits == 0) break;
	  if (bits & 1) fill_rect(x + j*scale, y + (k+i)*scale, scale, scale);
	  bits >>= 1;
	}
      }
    }
  }
}

void
Canvas::draw_icon(uint16_t x, uint16_t y, const uint8_t* bp,
		  uint16_t width, uint16_t height,
		  uint8_t scale)
{
  draw_bitmap(x, y, bp, width, height, scale);
}

void
Canvas::draw_icon(uint16_t x, uint16_t y, const uint8_t* bp, uint8_t scale)
{
  uint16_t width = pgm_read_byte(bp++);
  uint16_t height = pgm_read_byte(bp++);
  draw_icon(x, y, bp, width, height, scale);
}

void
Canvas::draw_image(uint16_t x, uint16_t y, Image* image)
{
  color16_t saved = set_pen_color(0);
  uint16_t width = image->WIDTH;
  uint16_t height = image->HEIGHT;
  for (uint16_t i = 0; i < height; i++) {
    color16_t buf[Image::BUFFER_MAX];
    size_t count;
    for (uint16_t j = 0; j < width; j += count) {
      count = (width - j > Image::BUFFER_MAX) ? Image::BUFFER_MAX : width - j;
      image->read(buf, Image::BUFFER_MAX);
      for (uint8_t k = 0; k < count; k++) {
	set_pen_color(buf[k]);
	draw_pixel(x + j, y + i);
      }
    }
  }
  set_pen_color(saved);
}

void
Canvas::draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  draw_horizontal_line(x, y, width);
  draw_vertical_line(x + width, y, height);
  draw_vertical_line(x, y, height);
  draw_horizontal_line(x, y + height, width);
}

void
Canvas::fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  if (width > height) {
    for (uint16_t h = 0; h < height; h++)
      draw_horizontal_line(x, y + h, width);
  }
  else {
    for (uint16_t w = 0; w < width; w++)
      draw_vertical_line(x + w, y, height);
  }
}

#define dist(x, y) ((x > y) ? (x - y) : (y - x))
#define swap(a, b) { uint16_t t = a; a = b; b = t; }

void
Canvas::draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  bool steep = (dist(y0, y1) > dist(x0, x1));
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
Canvas::draw_vertical_line(uint16_t x, uint16_t y, uint16_t length)
{
  draw_line(x, y, x, y + length);
}

void
Canvas::draw_horizontal_line(uint16_t x, uint16_t y, uint16_t length)
{
  draw_line(x, y, x + length, y);
}

void
Canvas::draw_poly_P(const int8_t* poly, uint8_t scale)
{
  if (UNLIKELY(scale == 0)) return;
  for (;;) {
    int8_t dx = pgm_read_byte(poly++);
    int8_t dy = pgm_read_byte(poly++);
    if (UNLIKELY(dx == 0 && dy == 0)) return;
    uint16_t x, y;
    get_cursor(x, y);
    x += dx*scale;
    y += dy*scale;
    draw_line(x, y);
  }
}

void
Canvas::draw_stroke_P(const int8_t* stroke, uint8_t scale)
{
  if (UNLIKELY(scale == 0)) return;
  for (;;) {
    int8_t dx = pgm_read_byte(stroke++);
    int8_t dy = pgm_read_byte(stroke++);
    if (UNLIKELY(dx == 0 && dy == 0)) return;
    if (dx <= 0 && dy <= 0) {
      move_cursor(dx*scale, dy*scale);
    }
    else {
      uint16_t x, y;
      get_cursor(x, y);
      x += dx*scale;
      y += dy*scale;
      draw_line(x, y);
    }
  }
}

void
Canvas::draw_circle(uint16_t x, uint16_t y, uint16_t radius)
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
Canvas::fill_circle(uint16_t x, uint16_t y, uint16_t radius)
{
  int16_t dx = 0, dy = radius;
  int16_t p = 1 - radius;

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
Canvas::draw_roundrect(uint16_t x, uint16_t y,
		       uint16_t width, uint16_t height,
		       uint16_t radius)
{
  uint16_t diameter = 2 * radius;
  int16_t f = 1 - radius;
  int16_t dx = 1;
  int16_t dy = -diameter;
  int8_t rx = 0;
  int8_t ry = radius;

  // Adjust position, width and height
  x += radius;
  y += radius;
  width -= diameter;
  height -= diameter;

  // Draw the boundary rectangle
  draw_horizontal_line(x, y - radius, width + 1);
  draw_vertical_line(x + width + radius, y, height + 1);
  draw_vertical_line(x - radius, y, height + 1);
  draw_horizontal_line(x, y + height + radius, width + 1);

  // Draw the round corners
  while (rx < ry) {
    if (f >= 0) {
      ry--;
      dy += 2;
      f += dy;
    }
    rx++;
    dx += 2;
    f += dx;
    draw_pixel(x + rx + width, y - ry);
    draw_pixel(x + ry + width, y - rx);
    draw_pixel(x + rx + width, y + ry + height);
    draw_pixel(x + ry + width, y + rx + height);
    draw_pixel(x - rx, y + ry + height);
    draw_pixel(x - ry, y + rx + height);
    draw_pixel(x - rx, y - ry);
    draw_pixel(x - ry, y - rx);
  }
}

void
Canvas::fill_roundrect(uint16_t x, uint16_t y,
		       uint16_t width, uint16_t height,
		       uint16_t radius)
{
  int16_t dx = 0, dy = radius;
  int16_t p = 1 - radius;
  uint16_t diameter = 2 * radius;

  // Adjust the position and fill the inner rectangle
  x += radius;
  width -= diameter;
  fill_rect(x, y, width, height + 1);
  height -= diameter;
  y += radius;

  // Draw the outer rectangle and corners
  while (dx <= dy) {
    draw_vertical_line(x + dx + width, y - dy, dy + dy + height);
    draw_vertical_line(x - dx, y - dy, dy + dy + height);
    draw_vertical_line(x + dy + width, y - dx, dx + dx + height);
    draw_vertical_line(x - dy, y - dx, dx + dx + height);
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
Canvas::draw_char(uint16_t x, uint16_t y, char c)
{
  uint8_t scale = get_text_scale();
  color16_t saved = set_pen_color(get_text_color());
  Font* font = get_text_font();
  font->draw(this, c, x, y, scale);
  set_cursor(x + scale * (font->WIDTH + font->SPACING), y);
  set_pen_color(saved);
}

void
Canvas::draw_string(char* s)
{
  char c;
  while ((c = *s++) != 0)
    draw_char(c);
}

void
Canvas::draw_string(str_P s)
{
  const char* sp = (const char*) s;
  char c;
  while ((c = pgm_read_byte(sp++)) != 0)
    draw_char(c);
}

void
Canvas::fill_screen()
{
  color16_t saved = set_pen_color(get_canvas_color());
  fill_rect(0, 0, WIDTH, HEIGHT);
  set_pen_color(saved);
}

void
Canvas::run(uint8_t ix, const void_P* tab, uint8_t max)
{
  if (UNLIKELY(ix >= max)) return;
  const uint8_t* ip = (const uint8_t*) pgm_read_word(tab + ix);
  uint8_t x, y, r, g, b, w, h, s;
  int8_t dx, dy;
  char c;
  while (1) {
    switch (pgm_read_byte(ip++)) {
    case END_SCRIPT:
      return;
    case CALL_SCRIPT:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      run(ix, tab, max);
      break;
    case SET_CANVAS_COLOR:
      r = pgm_read_byte(ip++);
      g = pgm_read_byte(ip++);
      b = pgm_read_byte(ip++);
      set_canvas_color(color(r, b, g));
      break;
    case SET_PEN_COLOR:
      r = pgm_read_byte(ip++);
      g = pgm_read_byte(ip++);
      b = pgm_read_byte(ip++);
      set_pen_color(color(r, g, b));
      break;
    case SET_TEXT_COLOR:
      r = pgm_read_byte(ip++);
      g = pgm_read_byte(ip++);
      b = pgm_read_byte(ip++);
      set_text_color(color(r, g, b));
      break;
    case SET_TEXT_SCALE:
      set_text_scale(pgm_read_byte(ip++));
      break;
    case SET_TEXT_FONT:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      set_text_font((Font*) pgm_read_word(tab + ix));
      break;
    case SET_CURSOR:
      x = pgm_read_byte(ip++);
      y = pgm_read_byte(ip++);
      set_cursor(x, y);
      break;
    case MOVE_CURSOR:
      dx = pgm_read_byte(ip++);
      dy = pgm_read_byte(ip++);
      move_cursor(dx, dy);
      break;
    case DRAW_BITMAP:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      w = pgm_read_byte(ip++);
      h = pgm_read_byte(ip++);
      s = pgm_read_byte(ip++);
      draw_bitmap((const uint8_t*) pgm_read_word(tab + ix), w, h, s);
      break;
    case DRAW_ICON:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      s = pgm_read_byte(ip++);
      draw_icon((const uint8_t*) pgm_read_word(tab + ix), s);
      break;
    case DRAW_PIXEL:
      draw_pixel();
      break;
    case DRAW_LINE:
      x = pgm_read_byte(ip++);
      y = pgm_read_byte(ip++);
      draw_line(x, y);
      break;
    case DRAW_POLY:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      s = pgm_read_byte(ip++);
      draw_poly_P((const int8_t*) pgm_read_word(tab + ix), s);
      break;
    case DRAW_STROKE:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      s = pgm_read_byte(ip++);
      draw_stroke_P((const int8_t*) pgm_read_word(tab + ix), s);
      break;
    case DRAW_RECT:
      w = pgm_read_byte(ip++);
      h = pgm_read_byte(ip++);
      draw_rect(w, h);
      break;
    case FILL_RECT:
      w = pgm_read_byte(ip++);
      h = pgm_read_byte(ip++);
      fill_rect(w, h);
      break;
    case DRAW_ROUNDRECT:
      w = pgm_read_byte(ip++);
      h = pgm_read_byte(ip++);
      r = pgm_read_byte(ip++);
      draw_roundrect(w, h, r);
      break;
    case FILL_ROUNDRECT:
      w = pgm_read_byte(ip++);
      h = pgm_read_byte(ip++);
      r = pgm_read_byte(ip++);
      fill_roundrect(w, h, r);
      break;
    case DRAW_CIRCLE:
      r = pgm_read_byte(ip++);
      draw_circle(r);
      break;
    case FILL_CIRCLE:
      r = pgm_read_byte(ip++);
      fill_circle(r);
      break;
    case DRAW_CHAR:
      c = pgm_read_byte(ip++);
      draw_char(c);
      break;
    case DRAW_STRING:
      ix = pgm_read_byte(ip++);
      if (UNLIKELY(ix >= max)) return;
      draw_string((str_P) pgm_read_word(tab + ix));
      break;
    case FILL_SCREEN:
      fill_screen();
      break;
    default:
      return;
    }
  }
}

