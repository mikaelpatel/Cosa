/**
 * @file Cosa/IOStream/Driver/ST7565P.cpp
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

#include "Cosa/IOStream/Driver/ST7565P.hh"
#include "Cosa/Watchdog.hh"

#define ST7565P_transaction(cs)					\
  for (uint8_t i = (cs.clear(), 1); i != 0; i--, cs.set())

const uint8_t ST7565P::script[] PROGMEM = {
  LCD_BIAS_9,
  ADC_REVERSE,
  COM_OUTPUT_NORMAL,
  SET_DISPLAY_START 	| 0,
  SET_POWER_CONTROL 	| 0x04,
  WAIT			, 50,
  SET_POWER_CONTROL 	| 0x06,
  WAIT			, 50,
  SET_POWER_CONTROL 	| 0x07,
  WAIT			, 10,
  SET_RESISTOR_RATIO 	| 0x06,
  DISPLAY_ON,
  DISPLAY_NORMAL,
  DISPLAY_NORMAL_POINTS,
  STOP
};

void 
ST7565P::set_address(uint8_t x, uint8_t y)
{
  x += 4;
  ST7565P_transaction(m_cs) {
    m_dc.clear();
    m_si.write(SET_X_ADDR | ((x >> 4) & X_ADDR_MASK), m_scl);
    m_si.write(x & X_ADDR_MASK, m_scl);
    m_si.write(SET_Y_ADDR | (y & Y_ADDR_MASK), m_scl);
    m_dc.set();
  }
}

void 
ST7565P::fill(uint8_t data, uint16_t count)
{
  ST7565P_transaction(m_cs) {
    for (uint16_t i = 0; i < count; i++) 
      m_si.write(data, m_scl);
  }
}

bool 
ST7565P::begin(uint8_t level)
{
  const uint8_t* bp = script;
  uint8_t cmd;
  ST7565P_transaction(m_cs) {
    m_dc.clear();
    while ((cmd = pgm_read_byte(bp++)) != STOP) {
      if (cmd != WAIT) {
	m_si.write(cmd, m_scl);
      }
      else {
	uint8_t ms = pgm_read_byte(bp++);
	Watchdog::delay(ms);
      }
    }
    m_dc.set();
  }
  set_display_contrast(level);
  m_x = 0;
  m_y = 0;
  set_address(m_x, m_y);
  return (true);
}

bool 
ST7565P::end()
{
  ST7565P_transaction(m_cs) {
    m_dc.clear();
    m_si.write(DISPLAY_OFF, m_scl);
    m_dc.set();
  }
  return (true);
}

void 
ST7565P::set_display_mode(DisplayMode mode)
{
  ST7565P_transaction(m_cs) {
    m_dc.clear();
    m_si.write(DISPLAY_NORMAL | mode, m_scl);
    m_dc.set();
  }
}

void 
ST7565P::set_display_contrast(uint8_t level)
{
  ST7565P_transaction(m_cs) {
    m_dc.clear();
    m_si.write(SET_CONSTRAST, m_scl);
    m_si.write(CONSTRAST_MASK & level, m_scl);
    m_dc.set();
  }
}

void 
ST7565P::set_cursor(uint8_t x, uint8_t y)
{
  set_address(x, y);
  m_x = x;
  m_y = (y & Y_ADDR_MASK);
}

void 
ST7565P::draw_icon(const uint8_t* bp)
{
  uint8_t width = pgm_read_byte(bp++);
  uint8_t height = pgm_read_byte(bp++);
  uint8_t lines = (height >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    ST7565P_transaction(m_cs) {
      for (uint8_t x = 0; x < width; x++) {
	m_si.write(m_mode ^ pgm_read_byte(bp++), m_scl);
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
ST7565P::draw_bitmap(uint8_t* bp, uint8_t width, uint8_t height)
{
  uint8_t lines = (height >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    ST7565P_transaction(m_cs) {
      for (uint8_t x = 0; x < width; x++) {
	m_si.write(m_mode ^ (*bp++), m_scl);
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
ST7565P::draw_bar(uint8_t procent, uint8_t width, uint8_t pattern)
{
  if (procent > 100) procent = 100;
  uint8_t filled = (procent * (width - 2U)) / 100;
  uint8_t boarder = (m_y == 0 ? 0x81 : 0x80);
  width -= (filled + 1);
  ST7565P_transaction(m_cs) {
    m_si.write(m_mode ^ 0xff, m_scl);
    while (filled--) {
      m_si.write(m_mode ^ (pattern | boarder), m_scl);
      pattern = ~pattern;
    }
    m_si.write(m_mode ^ 0xff, m_scl);
    width -= 1;
    if (width > 0) {
      while (width--)
	m_si.write(m_mode ^ boarder, m_scl);
    }
    m_si.write(m_mode ^ 0xff, m_scl);
  }
}

int 
ST7565P::putchar(char c)
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
    for (uint8_t y = 0; y < LINES; y++) {
      set_address(0, y);
      fill(m_mode, WIDTH);
    }
    m_x = 0;
    m_y = 0;
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
  ST7565P_transaction(m_cs) {
    while (--width) 
      m_si.write(m_mode ^ pgm_read_byte(bp++), m_scl);
    m_si.write(m_mode, m_scl);
  }

  return (c);
}

