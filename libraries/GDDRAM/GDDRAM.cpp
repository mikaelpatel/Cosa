/**
 * @file GDDRAM.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "GDDRAM.hh"

GDDRAM::GDDRAM(uint16_t width,
	       uint16_t height,
	       Board::DigitalPin cs,
	       Board::DigitalPin dc) :
  Canvas(width, height),
  SPI::Driver(cs, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 3, SPI::MSB_ORDER, NULL),
  m_dc(dc, 1),
  m_initiated(false)
{
}

bool
GDDRAM::begin()
{
  if (m_initiated) return (false);
  const uint8_t* bp = script();
  uint8_t count;
  uint8_t cmd;
  spi.acquire(this);
    spi.begin();
      while ((cmd = pgm_read_byte(bp++)) != SCRIPTEND) {
	count = pgm_read_byte(bp++);
	if (cmd == SWDELAY) {
	  DELAY(count);
	}
	else {
	  asserted(m_dc) {
	    spi.transfer(cmd);
	  }
	  while (count--) spi.transfer(pgm_read_byte(bp++));
	}
      }
    spi.end();
  spi.release();
  m_initiated = true;
  return (true);
}

uint8_t
GDDRAM::set_orientation(uint8_t direction)
{
  uint8_t previous = m_direction;
  uint8_t setting = 0;
  m_direction = direction;
  uint16_t temp = WIDTH;
  WIDTH  = HEIGHT;
  HEIGHT = temp;
  if (direction == LANDSCAPE) {
    setting = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
  }
  else {
    setting = (MADCTL_MX | MADCTL_BGR);
  }
  spi.acquire(this);
    spi.begin();
      write(MADCTL, setting);
    spi.end();
  spi.release();
  return (previous);
}

void
GDDRAM::draw_pixel(uint16_t x, uint16_t y)
{
  const color16_t color = get_pen_color();
  spi.acquire(this);
    spi.begin();
      write(CASET, x, x + 1);
      write(PASET, y, y + 1);
      write(RAMWR);
      write(color.rgb);
    spi.end();
  spi.release();
}

void
GDDRAM::draw_image(uint16_t x, uint16_t y, Image* image)
{
  uint16_t width = image->WIDTH;
  uint16_t height = image->HEIGHT;
  spi.acquire(this);
    spi.begin();
      write(CASET, x, x + width - 1);
      write(PASET, y, y + height - 1);
      write(RAMWR);
    spi.end();
  spi.release();
  for (uint16_t i = 0; i < height; i++) {
    color16_t buf[Image::BUFFER_MAX];
    size_t count;
    for (uint16_t j = 0; j < width; j += count) {
      count = (width - j > Image::BUFFER_MAX) ? Image::BUFFER_MAX : width - j;
      image->read(buf, count);
      spi.acquire(this);
        spi.begin();
	for (uint16_t k = 0; k < count; k++) write(buf[k]);
	spi.end();
      spi.release();
    }
  }
}

void
GDDRAM::draw_vertical_line(uint16_t x, uint16_t y, uint16_t length)
{
  if (UNLIKELY((x >= WIDTH) || (length == 0))) return;
  if (y >= HEIGHT) {
    uint16_t h = y + length;
    if (h >= HEIGHT) return;
    length = h;
    y = 0;
  }
  if ((y + length - 1) >= HEIGHT) length = HEIGHT - y;
  const color16_t color = get_pen_color();
  spi.acquire(this);
    spi.begin();
      write(CASET, x, x);
      write(PASET, y, y + length - 1);
      write(RAMWR);
      write(color.rgb, length);
    spi.end();
  spi.release();
}

void
GDDRAM::draw_horizontal_line(uint16_t x, uint16_t y, uint16_t length)
{
  if (UNLIKELY((y >= HEIGHT) || (length == 0))) return;
  if (x >= WIDTH) {
    uint16_t w = x + length;
    if (w >= WIDTH) return;
    length = w;
    x = 0;
  }
  if ((x + length - 1) >= WIDTH) length = WIDTH - x;
  const color16_t color = get_pen_color();
  spi.acquire(this);
    spi.begin();
      write(CASET, x, x + length - 1);
      write(PASET, y, y);
      write(RAMWR);
      write(color.rgb, length);
    spi.end();
  spi.release();
}

void
GDDRAM::fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  if (UNLIKELY((width == 0) || (height == 0))) return;
  if (UNLIKELY((x >= WIDTH) || (y >= HEIGHT))) return;
  if ((x + width - 1) >= WIDTH) width = WIDTH - x;
  if ((y + height - 1) >= HEIGHT) height = HEIGHT - y;
  const color16_t color = get_pen_color();
  spi.acquire(this);
    spi.begin();
      write(CASET, x, x + width - 1);
      write(PASET, y, y + height - 1);
      write(RAMWR);
      if (width > height) {
	for (y = 0; y < height; y++)
	  write(color.rgb, width);
      }
      else {
	for (x = 0; x < width; x++)
	  write(color.rgb, height);
      }
    spi.end();
  spi.release();
}

bool
GDDRAM::end()
{
  return (true);
}
