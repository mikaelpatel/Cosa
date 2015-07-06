/**
 * @file PCD8544.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#include "PCD8544.hh"

// Startup script
const uint8_t PCD8544::script[] __PROGMEM = {
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

PCD8544::PCD8544(LCD::IO* io, Board::DigitalPin dc, Font* font) :
  LCD::Device(),
  m_io(io),
  m_dc(dc, 1),
  m_font(font)
{
}

void
PCD8544::set(uint8_t cmd)
{
  m_io->begin();
  asserted(m_dc) {
    m_io->write(cmd);
  }
  m_io->end();
}

void
PCD8544::set(uint8_t x, uint8_t y)
{
  m_io->begin();
  asserted(m_dc) {
    m_io->write(SET_X_ADDR | (x & X_ADDR_MASK));
    m_io->write(SET_Y_ADDR | (y & Y_ADDR_MASK));
  }
  m_io->end();
}

void
PCD8544::fill(uint8_t data, uint16_t count)
{
  m_io->begin();
  while (count--) m_io->write(data);
  m_io->end();
}

bool
PCD8544::begin()
{
  const uint8_t* bp = script;
  uint8_t cmd;
  m_io->begin();
  asserted(m_dc) {
    while ((cmd = pgm_read_byte(bp++)) != SCRIPT_END)
      m_io->write(cmd);
  }
  m_io->end();
  display_clear();
  return (true);
}

bool
PCD8544::end()
{
  set(SET_FUNC | BASIC_INST | POWER_DOWN_MODE);
  return (true);
}

void
PCD8544::display_on()
{
  set(DISPLAY_CNTL | DISPLAY_ON);
}

void
PCD8544::display_off()
{
  set(DISPLAY_CNTL | DISPLAY_OFF);
}

void
PCD8544::display_normal()
{
  set(DISPLAY_CNTL | NORMAL_MODE);
}

void
PCD8544::display_inverse()
{
  set(DISPLAY_CNTL | INVERSE_MODE);
}

void
PCD8544::display_clear()
{
  set_cursor(0, 0);
  fill(m_mode, LINES * WIDTH);
  set(m_x, m_y);
}

void
PCD8544::display_contrast(uint8_t level)
{
  m_io->begin();
  asserted(m_dc) {
    m_io->write(SET_FUNC | EXTENDED_INST);
    m_io->write(SET_VOP  | (level & VOP_MASK));
    m_io->write(SET_FUNC | BASIC_INST | HORIZONTAL_ADDR);
  }
  m_io->end();
}

void
PCD8544::set_cursor(uint8_t x, uint8_t y)
{
  // For x on character boundary (and not pixel)
  // x *= m_font->WIDTH + m_font->SPACING;
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
    m_io->begin();
    for (uint8_t x = 0; x < width; x++) {
      m_io->write(m_mode ^ pgm_read_byte(bp++));
    }
    m_io->end();
    set_cursor(m_x, m_y + 1);
  }
  set_cursor(m_x, m_y + 1);
}

void
PCD8544::draw_bitmap(uint8_t* bp, uint8_t width, uint8_t height)
{
  uint8_t lines = (height >> 3);
  for (uint8_t y = 0; y < lines; y++) {
    m_io->begin();
    for (uint8_t x = 0; x < width; x++) {
      m_io->write(m_mode ^ (*bp++));
    }
    m_io->end();
    set_cursor(m_x, m_y + 1);
  }
  m_y += 1;
  set_cursor(m_x, m_y + 1);
}

void
PCD8544::draw_bar(uint8_t percent, uint8_t width, uint8_t pattern)
{
  if (UNLIKELY(percent > 100)) percent = 100;
  uint8_t filled = (percent * (width - 2U)) / 100;
  uint8_t boarder = (m_y == 0 ? 0x81 : 0x80);
  width -= (filled + 1);
  m_io->begin();
  m_io->write(m_mode ^ 0xff);
  while (filled--) {
    m_io->write(m_mode ^ (pattern | boarder));
    pattern = ~pattern;
  }
  m_io->write(m_mode ^ 0xff);
  width -= 1;
  if (width > 0) {
    while (width--)
      m_io->write(m_mode ^ boarder);
  }
  m_io->write(m_mode ^ 0xff);
  m_io->end();
}

int
PCD8544::putchar(char c)
{
  // Check for special characters
  if (c < ' ') {

    // Carriage-return: move to start of line
    if (c == '\r') {
      set_cursor(0, m_y);
      return (c);
    }

    // Check for line-feed: clear new line
    if (c == '\n') {
      m_y += 1;
      if (m_y == LINES) m_y = 0;
      set_cursor(0, m_y);
      fill(m_mode, WIDTH);
      set(m_x, m_y);
      return (c);
    }

    // Check for horizontal tab
    if (c == '\t') {
      uint8_t x = m_x + m_tab - (m_x % m_tab);
      uint8_t y = m_y + (x >= WIDTH);
      set_cursor(x, y);
      return (c);
    }

    // Check for special character: form-feed
    if (c == '\f') {
      display_clear();
      return (c);
    }

    // Check for special character: back-space
    if (c == '\b') {
      uint8_t width = m_font->WIDTH + m_font->SPACING;
      if (m_x < width) width = m_x;
      set_cursor(m_x - width, m_y);
      return (c);
    }

    // Check for special character: alert
    if (c == '\a') {
      m_mode = ~m_mode;
      return (c);
    }
  }

  // Access font for character width and bitmap
  uint8_t width = m_font->WIDTH + m_font->SPACING;
  Font::Glyph glyph(m_font, c);
  m_x += width;

  // Check that the character is not clipped
  if (m_x > WIDTH) {
    putchar('\n');
    m_x = width;
  }

  // Write character to the display memory and an extra byte
  m_io->begin();
  while (--width)
    m_io->write(m_mode ^ glyph.next());
  m_io->write(m_mode);
  m_io->end();

  return (c);
}
