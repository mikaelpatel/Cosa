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
 * @section Description
 * PCD8544 48x84 pixels matrix LCD controller/driver, device driver 
 * for IOStream access. Binding to trace, etc. For furter details
 * see Product Specification, Philips Semiconductors, 1999 Apr 12.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream/Driver/PCD8544.hh"

const uint8_t PCD8544::script[] PROGMEM = {
  SET_FUNC       | EXTENDED_INST,
  SET_VOP 	 | 0x2a,
  SET_TEMP_COEFF | 0x00,
  SET_BIAS_SYS   | 0x04,
  SET_FUNC       | BASIC_INST	| HORIZONTAL_ADDR,
  DISPLAY_CNTL   | NORMAL_MODE,
  SET_X_ADDR     | 0,
  SET_Y_ADDR     | 0,
  NOP
};

void 
PCD8544::set_address(uint8_t x, uint8_t y)
{
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    m_sdin.write(SET_X_ADDR | (x & X_ADDR_MASK), m_sclk);
    m_sdin.write(SET_Y_ADDR | (y & Y_ADDR_MASK), m_sclk);
    m_dc.set();
  }
}

void 
PCD8544::fill(uint8_t data, uint16_t count)
{
  PCD8544_transaction(m_sce) {
    for (uint16_t i = 0; i < count; i++)
      m_sdin.write(data, m_sclk);
  }
}

bool 
PCD8544::begin()
{
  const uint8_t* bp = script;
  uint8_t cmd;
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    while ((cmd = pgm_read_byte(bp++)) != NOP)
      m_sdin.write(cmd, m_sclk);
    m_dc.set();
  }
  m_x = 0;
  m_y = 0;
  return (1);
}

bool 
PCD8544::end()
{
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    m_sdin.write(SET_FUNC | BASIC_INST | POWER_DOWN_MODE);
    m_dc.set();
  }
  return (1);
}

void 
PCD8544::set_display_mode(DisplayMode mode)
{
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    m_sdin.write(DISPLAY_CNTL | mode, m_sclk);
    m_dc.set();
  }
}

void 
PCD8544::set_cursor(uint8_t x, uint8_t y)
{
  set_address(x, y);
  m_x = (x & X_ADDR_MASK);
  m_y = (y & Y_ADDR_MASK);
}

void 
PCD8544::draw_icon(const uint8_t* bp)
{
  uint8_t width = pgm_read_byte(bp++);
  uint8_t height = pgm_read_byte(bp++);
  uint8_t lines = (height >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    PCD8544_transaction(m_sce) {
      for (uint8_t x = 0; x < width; x++) {
	m_sdin.write(m_mode ^ pgm_read_byte(bp++), m_sclk);
      }
    }
    m_y += 1;
    set_address(m_x, m_y);
  }
  m_y += 1;
  if (m_y == LINES) m_y = 0;
  set_address(m_x, m_y);
}

void 
PCD8544::draw_bar(uint8_t procent, uint8_t width, uint8_t pattern)
{
  if (procent > 100) procent = 100;
  uint8_t filled = (procent * (width - 2U)) / 100;
  uint8_t boarder = (m_y == 0 ? 0x81 : 0x80);
  width -= (filled + 1);
  PCD8544_transaction(m_sce) {
    m_sdin.write(m_mode ^ 0xff, m_sclk);
    while (filled--) {
      m_sdin.write(m_mode ^ (pattern | boarder), m_sclk);
      pattern = ~pattern;
    }
    m_sdin.write(m_mode ^ 0xff, m_sclk);
    width -= 1;
    if (width > 0) {
      while (width--)
	m_sdin.write(m_mode ^ boarder, m_sclk);
      m_sdin.write(m_mode ^ 0xff, m_sclk);
    }
  }
}

int 
PCD8544::putchar(char c)
{
  // Check for special characters; carriage-return-line-feed
  if (c == '\n') {
    m_x = 0;
    m_y += 1;
    if (m_y == LINES) m_y = 0;
    set_address(m_x, m_y);
    fill(m_mode, WIDTH);
    set_address(m_x, m_y);
    return (c);
  }
  
  // Check for special character: form-feed
  if (c == '\f') {
    m_x = 0;
    m_y = 0;
    fill(m_mode, LINES * WIDTH);
    set_address(m_x, m_y);
    return (c);
  }

  // Check for special character: back-space
  if (c == '\b') {
    uint8_t width = m_font->get_width(' ');
    if (m_x < width) width = m_x;
    m_x -= width;
    set_address(m_x, m_y);
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
  PCD8544_transaction(m_sce) {
    while (--width) 
      m_sdin.write(m_mode ^ pgm_read_byte(bp++), m_sclk);
    m_sdin.write(m_mode, m_sclk);
  }

  return (c);
}
