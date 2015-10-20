/**
 * @file ST7565.cpp
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

#include "ST7565.hh"

// Initialization script
const uint8_t ST7565::script[] __PROGMEM = {
  LCD_BIAS_9,
  ADC_NORMAL,
  COM_OUTPUT_REVERSE,
  SET_DISPLAY_START 	| 0,
  SET_POWER_CONTROL 	| 0x04,
  SCRIPT_PAUSE		, 50,
  SET_POWER_CONTROL 	| 0x06,
  SCRIPT_PAUSE		, 50,
  SET_POWER_CONTROL 	| 0x07,
  SCRIPT_PAUSE		, 10,
  SET_RESISTOR_RATIO 	| 0x06,
  SET_CONSTRAST		, 0x08,
  DISPLAY_ON,
  DISPLAY_NORMAL,
  DISPLAY_64X128_POINTS,
  SCRIPT_END
};

ST7565::ST7565(LCD::IO* io, Board::DigitalPin dc, Font* font) :
  LCD::Device(),
  m_io(io),
  m_dc(dc, 1),
  m_font(font)
{
}

void
ST7565::set(uint8_t cmd)
{
  m_io->begin();
  asserted(m_dc) {
    m_io->write(cmd);
  }
  m_io->end();
}

void
ST7565::set(uint8_t x, uint8_t y)
{
  m_io->begin();
  asserted(m_dc) {
    m_io->write(SET_X_ADDR | ((x >> 4) & X_ADDR_MASK));
    m_io->write(x & X_ADDR_MASK);
    m_io->write(SET_Y_ADDR | (y & Y_ADDR_MASK));
  }
  m_io->end();
}

void
ST7565::fill(uint8_t data, uint16_t count)
{
  m_io->begin();
  while (count--) m_io->write(data);
  m_io->end();
}

bool
ST7565::begin()
{
  const uint8_t* bp = script;
  uint8_t cmd;
  m_io->begin();
  asserted(m_dc) {
    while ((cmd = pgm_read_byte(bp++)) != SCRIPT_END) {
      if (cmd == SCRIPT_PAUSE) {
	uint8_t ms = pgm_read_byte(bp++);
	delay(ms);
      }
      else m_io->write(cmd);
    }
  }
  m_io->end();
  display_clear();
  return (true);
}

bool
ST7565::end()
{
  set(DISPLAY_OFF);
  return (true);
}

void
ST7565::display_contrast(uint8_t level)
{
  m_io->begin();
  asserted(m_dc) {
    m_io->write(SET_CONSTRAST);
    m_io->write(CONSTRAST_MASK & level);
  }
  m_io->end();
}

void
ST7565::display_on()
{
  set(DISPLAY_ON);
}

void
ST7565::display_off()
{
  set(DISPLAY_OFF);
}

void
ST7565::display_normal()
{
  set(DISPLAY_NORMAL);
}

void
ST7565::display_inverse()
{
  set(DISPLAY_REVERSE);
}

void
ST7565::display_clear()
{
  for (uint8_t y = 0; y < LINES; y++) {
    set(0, y);
    fill(m_mode, WIDTH);
  }
  set_cursor(0, 0);
}

void
ST7565::set_cursor(uint8_t x, uint8_t y)
{
  set(x, y);
  m_x = (x & (WIDTH - 1));
  m_y = (y & (LINES - 1));
  if (UNLIKELY((m_x != 0) || (m_y != 0))) return;
  m_line = 0;
  set(SET_DISPLAY_START | m_line);
}

void
ST7565::draw_icon(const uint8_t* bp)
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
ST7565::draw_bitmap(uint8_t* bp, uint8_t width, uint8_t height)
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
  set_cursor(m_x, m_y + 1);
}

void
ST7565::draw_bar(uint8_t percent, uint8_t width, uint8_t pattern)
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
ST7565::putchar(char c)
{
  // Check for special characters
  if (c < ' ') {

    // Carriage-return: move to start of line
    if (c == '\r') {
      set_cursor(0, m_y);
      return (c);
    }

    // Check line-feed: clear new line, Use display start line scroll
    if (c == '\n') {
      if (m_y == (LINES - 1)) {
	m_line = (m_line + CHARBITS) & DISPLAY_START_MASK;
	set(SET_DISPLAY_START | m_line);
	uint8_t y = m_line / CHARBITS;
	if (y == 0) y = 7; else y = y - 1;
	set(0, y);
	fill(m_mode, WIDTH);
	set(0, y);
	m_x = 0;
      }
      else {
	set_cursor(0, m_y + 1);
	fill(m_mode, WIDTH);
	set(m_x, m_y);
      }
      return (c);
    }

    // Check for special character: horizontal tab
    if (c == '\t') {
      uint8_t tab = m_tab * (m_font->WIDTH + m_font->SPACING);
      uint8_t x = m_x + tab - (m_x % tab);
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

  // Write character to the display with an extra space
  uint8_t width = m_font->WIDTH + m_font->SPACING;
  Font::Glyph glyph(m_font, c);
  m_x += width;
  if (m_x > WIDTH) {
    putchar('\n');
    m_x = width;
  }
  m_io->begin();
  while (--width)
    m_io->write(m_mode ^ glyph.next());
  m_io->write(m_mode);
  m_io->end();

  return (c & 0xff);
}

