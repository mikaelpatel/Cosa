/**
 * @file Cosa/IOStream/Driver/PCD8544.cpp
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream/Driver/PCD8544.hh"

const uint8_t PCD8544::script[] PROGMEM = {
  SET_FUNC       | EXTENDED_INST,
  SET_VOP 	 | 0x38,
  SET_TEMP_COEFF | 0x00,
  SET_BIAS_SYS   | 0x04,
  SET_FUNC       | BASIC_INST	| HORIZONTAL_ADDR,
  DISPLAY_CNTL   | NORMAL_MODE,
  SET_X_ADDR     | 0,
  SET_Y_ADDR     | 0,
  SCRIPT_END
};

void 
PCD8544::set(uint8_t x, uint8_t y)
{
  inverted(m_sce) {
    inverted(m_dc) {
      write(SET_X_ADDR | (x & X_ADDR_MASK));
      write(SET_Y_ADDR | (y & Y_ADDR_MASK));
    }
  }
}

bool 
PCD8544::begin(uint8_t level)
{
  const uint8_t* bp = script;
  uint8_t cmd;
  inverted(m_sce) {
    inverted(m_dc) {
      while ((cmd = pgm_read_byte(bp++)) != SCRIPT_END)
	write(cmd);
    }
  }
  set_display_contrast(level);
  set_cursor(0, 0);
  return (true);
}

bool 
PCD8544::end()
{
  inverted(m_sce) {
    inverted(m_dc) {
      write(SET_FUNC | BASIC_INST | POWER_DOWN_MODE);
    }
  }
  return (true);
}

void 
PCD8544::set_display_mode(DisplayMode mode)
{
  inverted(m_sce) {
    inverted(m_dc) {
      write(DISPLAY_CNTL | mode);
    }
  }
}

void 
PCD8544::set_display_contrast(uint8_t level)
{
  inverted(m_sce) {
    inverted(m_dc) {
      write(SET_FUNC | EXTENDED_INST);
      write(SET_VOP  | (level & VOP_MASK));
      write(SET_FUNC | BASIC_INST | HORIZONTAL_ADDR);
    }
  }
}

void 
PCD8544::set_cursor(uint8_t x, uint8_t y)
{
  set(x, y);
  m_x = x;
  m_y = y;
}

void 
PCD8544::draw_icon(const uint8_t* bp)
{
  uint8_t width = pgm_read_byte(bp++);
  uint8_t height = pgm_read_byte(bp++);
  uint8_t lines = (height >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    inverted(m_sce) {
      for (uint8_t x = 0; x < width; x++) {
	write(m_mode ^ pgm_read_byte(bp++));
      }
    }
    set_cursor(m_x, m_y + 1);
  }
  set_cursor(m_x, m_y + 1);
}

void 
PCD8544::draw_bitmap(uint8_t* bp, uint8_t width, uint8_t height)
{
  uint8_t lines = (height >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    inverted(m_sce) {
      for (uint8_t x = 0; x < width; x++) {
	write(m_mode ^ (*bp++));
      }
    }
    set_cursor(m_x, m_y + 1);
  }
  m_y += 1;
  set_cursor(m_x, m_y + 1);
}

void 
PCD8544::draw_bar(uint8_t procent, uint8_t width, uint8_t pattern)
{
  if (procent > 100) procent = 100;
  uint8_t filled = (procent * (width - 2U)) / 100;
  uint8_t boarder = (m_y == 0 ? 0x81 : 0x80);
  width -= (filled + 1);
  inverted(m_sce) {
    write(m_mode ^ 0xff);
    while (filled--) {
      write(m_mode ^ (pattern | boarder));
      pattern = ~pattern;
    }
    write(m_mode ^ 0xff);
    width -= 1;
    if (width > 0) {
      while (width--)
	write(m_mode ^ boarder);
    }
    write(m_mode ^ 0xff);
  }
}

int 
PCD8544::putchar(char c)
{
  // Check for special characters; carriage-return-line-feed
  if (c == '\n') {
    m_y += 1;
    if (m_y == LINES) m_y = 0;
    set_cursor(0, m_y);
    fill(m_mode, WIDTH);
    set(m_x, m_y);
    return (c);
  }
  
  // Check for special character: form-feed
  if (c == '\f') {
    set_cursor(0, 0);
    fill(m_mode, LINES * WIDTH);
    set(m_x, m_y);
    return (c);
  }

  // Check for special character: back-space
  if (c == '\b') {
    uint8_t width = m_font->get_width(' ');
    if (m_x < width) width = m_x;
    set_cursor(m_x - width, m_y);
    return (c);
  }

  // Check for special character: alert
  if (c == '\a') {
    m_mode = ~m_mode;
    return (c);
  }

  // Access font for character width and bitmap
  uint8_t width = m_font->get_width(c);
  const uint8_t* bp = m_font->get_bitmap(c);
  m_x += width;

  // Check that the character is not clipped
  if (m_x > WIDTH) {
    putchar('\n');
    m_x = width;
  }

  // Write character to the display memory and an extra byte
  inverted(m_sce) {
    while (--width) 
      write(m_mode ^ pgm_read_byte(bp++));
    write(m_mode);
  }

  return (c);
}
