/**
 * @file Cosa/LCD/Driver/MAX72XXMATRIX.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/LCD/Driver/MAX72XXMATRIX.hh"
#include "Cosa/Trace.hh"

/** 
 * @code
 * @endcode
 */

void 
MAX72XXMATRIX::setall(Register reg, uint8_t value)
{
  m_io->begin();
  for (uint8_t i = 1; i <= WIDTH * HEIGHT; i++) {
    m_io->write(reg);
    m_io->write(value);
  }
  m_io->end();
}

void 
MAX72XXMATRIX::setone(uint8_t display, Register reg, uint8_t value)
{
  m_io->begin();

  for (uint8_t i = 0; i < HEIGHT * WIDTH; i++) {
    if (i == display) {
      m_io->write(reg);
      m_io->write(value);
    } else {
      m_io->write(NOP);
      m_io->write(0);
    }
  }
  m_io->end();
}

bool 
MAX72XXMATRIX::begin()
{
  setall(SCAN_LIMIT, 7);
  setall(DECODE_MODE, NO_DECODE);
  setall(DISPLAY_TEST, 0);
  display_contrast(7);
  display_clear();
  display_on();
  return (true);
}

bool 
MAX72XXMATRIX::end()
{
  setall(DISPLAY_MODE, SHUTDOWN_MODE);
  return (true);
}

void 
MAX72XXMATRIX::display_on() 
{ 
  setall(DISPLAY_MODE, NORMAL_OPERATION);
}

void 
MAX72XXMATRIX::display_off() 
{ 
  setall(DISPLAY_MODE, SHUTDOWN_MODE);
}

void 
MAX72XXMATRIX::display_clear()
{
  for (uint8_t reg = COLUMN0; reg <= COLUMN7; reg++) {
    setall((Register) reg, 0x00);
  }
  set_cursor(0, 0);
}

void 
MAX72XXMATRIX::display_contrast(uint8_t level)
{
  setall(INTENSITY, level);
}

void 
MAX72XXMATRIX::set_cursor(uint8_t x, uint8_t y)
{
  m_x = x;
  m_y = y;
}

int 
MAX72XXMATRIX::putchar(char c)
{
  // Check for illegal characters
  if (c < 0) return (-1);

  // Check for special characters
  if (c < ' ') {

    // Alert: blink the backlight
    if (c == '\a') {
      display_off();
      delay(32);
      display_on();
      return (c);
    }

    // Back-space: move cursor back one step (if possible)
    if (c == '\b') {
      set_cursor(m_x - 1, m_y);
      return (c);
    }

    // Form-feed: clear the display or Carriage-return-line-feed: clear line
    if ((c == '\f') || (c == '\n')) {
      display_clear();
      return (c);
    }
    
    // Horizontal tab
    if (c == '\t') {
      uint8_t x = m_x + m_tab - (m_x % m_tab);
      uint8_t y = m_y + (x >= WIDTH);
      set_cursor(x, y);
      return (c);
    }
  }

  uint8_t width = m_font->get_width(c);
  const uint8_t* bp = m_font->get_bitmap(c);
  // Write character
  uint8_t xc = 0;
  for (uint8_t d = 0; d < HEIGHT * WIDTH; d++) {
    for (uint8_t reg = COLUMN0; reg <= COLUMN7; reg++) {
      if ((xc >= m_x) and (xc < m_x + (width - 1)))
        setone(d,(Register) reg, pgm_read_byte(bp++));
      xc++;
    }
  }
  m_x += width;

  return (c);
}

void MAX72XXMATRIX::draw_bitmap(uint8_t* bp, uint8_t width, uint8_t height)
{
  uint8_t lines = (height >> 3);
  uint8_t displays = (width >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    for (uint8_t d = 0; d < displays; d++) {
      for (uint8_t reg = COLUMN0; reg <= COLUMN7; reg++) {
        setone(d, (Register) reg, (*bp++));
      }
    }
  }
  m_y += 1;
  set_cursor(m_x, m_y + 1);
}
