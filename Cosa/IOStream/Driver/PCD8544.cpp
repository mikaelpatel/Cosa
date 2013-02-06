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
  SET_VOP 	 | 0x31,
  SET_TEMP_COEFF | 0x00,
  SET_BIAS_SYS   | 0x04,
  SET_FUNC       | BASIC_INST	| HORIZONTAL_ADDR,
  DISPLAY_CNTL   | NORMAL_MODE,
  SET_X_ADDR     | 0,
  SET_Y_ADDR     | 0,
  NOP
};

bool 
PCD8544::begin()
{
  const uint8_t* bp = script;
  uint8_t cmd;
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    while ((cmd = pgm_read_byte(bp++)) != NOP)
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, cmd);
    m_dc.set();
  }
  return (1);
}

bool 
PCD8544::end()
{
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    m_sdin.write(&m_sclk, Pin::MSB_FIRST, BASIC_INST | DISPLAY_OFF);
    m_dc.set();
  }
  return (1);
}

void 
PCD8544::set_cursor(uint8_t x, uint8_t y)
{
  x = (x & X_ADDR_MASK);
  y = (y & Y_ADDR_MASK);
  PCD8544_transaction(m_sce) {
    m_dc.clear();
    m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_X_ADDR | x);
    m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_Y_ADDR | y);
    m_dc.set();
  }
  m_x = x;
  m_y = y;
}

int 
PCD8544::putchar(char c)
{
  // Check for special characters; carriage-return-line-feed
  if (c == '\n') {
    m_x = 0;
    m_y += 1;
    if (m_y == LINES) m_y = 0;
    PCD8544_transaction(m_sce) {
      m_dc.clear();
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_X_ADDR | m_x);
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_Y_ADDR | m_y);
      m_dc.set();
      for (uint8_t i = 0; i < WIDTH; i++) 
	m_sdin.write(&m_sclk, Pin::MSB_FIRST, 0);
      m_dc.clear();
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_X_ADDR | m_x);
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_Y_ADDR | m_y);
      m_dc.set();
    }
    return (c);
  }
  
  // Check for special character: form-feed
  if (c == '\f') {
    m_x = 0;
    m_y = 0;
    PCD8544_transaction(m_sce) {
      for (uint8_t i = 0; i < LINES; i++)
	for (uint8_t j = 0; j < WIDTH; j++) 
	  m_sdin.write(&m_sclk, Pin::MSB_FIRST, 0);
      m_dc.clear();
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_X_ADDR | m_x);
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, SET_Y_ADDR | m_y);
      m_dc.set();
    }
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
      m_sdin.write(&m_sclk, Pin::MSB_FIRST, m_mode ^ pgm_read_byte(bp++));
    m_sdin.write(&m_sclk, Pin::MSB_FIRST, m_mode);
  }

  return (c);
}
