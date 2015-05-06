/**
 * @file MAX72XX.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "MAX72XX.hh"

/**
 * No-Decode Mode Data Bits and Corresponding Segment Lines (Table 6, pp 8.).
 * @code
 *      A
 *    +---+
 *   F| G |B
 *    +---+	Standard 7-Segment LED
 *   E|   |C
 *    +---+ DP
 *      D
 *
 *  D7 D6 D5 D4 D3 D2 D1 D0
 *  DP  A  B  C  D  E  F  G
 * @endcode
 */
const uint8_t MAX72XX::font[] __PROGMEM = {
  0b00000000, // (space)
  0b10000000, // !
  0b00100010, // "
  0b10000000, // #
  0b10000000, // $
  0b10000000, // %
  0b10000000, // &
  0b00000010, // '
  0b01001110, // (
  0b01111000, // )
  0b10000000, // *
  0b10000000, // +
  0b10000000, // ,
  0b00000001, // -
  0b10000000, // .
  0b00100100, // /
  0b01111110, // 0
  0b00110000, // 1
  0b01101101, // 2
  0b01111001, // 3
  0b00110011, // 4
  0b01011011, // 5
  0b01011111, // 6
  0b01110000, // 7
  0b01111111, // 8
  0b01111011, // 9
  0b10000000, // :
  0b10000000, // ;
  0b10000000, // <
  0b10000000, // =
  0b10000000, // >
  0b10000000, // ?
  0b10000000, // @
  0b01110111, // A
  0b10000000, // B
  0b01001110, // C
  0b10000000, // D
  0b01001111, // E
  0b01000111, // F
  0b01011110, // G
  0b00110111, // H
  0b00110000, // I
  0b00111000, // J
  0b10000000, // K
  0b00001110, // L
  0b10000000, // M
  0b11110110, // N
  0b01111110, // O
  0b01100111, // P
  0b10000000, // Q
  0b10000000, // R
  0b01011011, // S
  0b10000000, // T
  0b00111110, // U
  0b10000000, // V
  0b10000000, // W
  0b00110111, // X
  0b00110011, // Y
  0b01101101, // Z
  0b01001110, // [
  0b00010011, // "\"
  0b01111000, // ]
  0b11100010, // ^
  0b00001000, // _
  0b10000000, // `
  0b10000000, // a
  0b00011111, // b
  0b00001101, // c
  0b00111101, // d
  0b10000000, // e
  0b01000111, // f
  0b01111011, // g
  0b00010111, // h
  0b00010000, // i
  0b00111000, // j
  0b10000000, // k
  0b00110000, // l
  0b10000000, // m
  0b00010101, // n
  0b00011101, // o
  0b01100111, // p
  0b01110011, // q
  0b00000101, // r
  0b10000000, // s
  0b10000000, // t
  0b00011100, // u
  0b10000000, // v
  0b10000000, // w
  0b10000000, // x
  0b10000000, // y
  0b10000000, // z
  0b00110001, // {
  0b00110000, // |
  0b00000111, // }
  0b00100101, // ~
  0b00010011  // DEL
};

MAX72XX::MAX72XX(LCD::IO* io, const uint8_t* font) :
  LCD::Device(),
  m_io(io),
  m_font(font != NULL ? font : MAX72XX::font)
{
}

void
MAX72XX::set(Register reg, uint8_t value)
{
  m_io->begin();
  m_io->write(reg);
  m_io->write(value);
  m_io->end();
}

bool
MAX72XX::begin()
{
  set(DECODE_MODE, NO_DECODE);
  set(SCAN_LIMIT, 7);
  display_contrast(7);
  display_clear();
  display_on();
  return (true);
}

bool
MAX72XX::end()
{
  set(DISPLAY_MODE, SHUTDOWN_MODE);
  return (true);
}

void
MAX72XX::display_on()
{
  set(DISPLAY_MODE, NORMAL_OPERATION);
}

void
MAX72XX::display_off()
{
  set(DISPLAY_MODE, SHUTDOWN_MODE);
}

void
MAX72XX::display_clear()
{
  for (uint8_t reg = DIGIT0; reg <= DIGIT7; reg++)
    set((Register) reg, 0x00);
  set_cursor(0, 0);
}

void
MAX72XX::display_contrast(uint8_t level)
{
  set(INTENSITY, level);
}

void
MAX72XX::set_cursor(uint8_t x, uint8_t y)
{
  m_x = x;
  m_y = y;
}

int
MAX72XX::putchar(char c)
{
  // Check for illegal characters
  if (c < 0) return (-1);

  // Check for special characters
  if (c < ' ') {

    // Carriage-return: move to start of line
    if (c == '\r') {
      set_cursor(0, m_y);
      return (c);
    }
    // Form-feed: clear the display or line-feed: clear line
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

    // Back-space: move cursor back one step (if possible)
    if (c == '\b') {
      set_cursor(m_x - 1, m_y);
      return (c);
    }

    // Alert: blink the backlight
    if (c == '\a') {
      display_off();
      delay(32);
      display_on();
      return (c);
    }
  }

  // Write character
  uint8_t segments;
  if (c == '.') {
    c = m_latest;
    segments = pgm_read_byte(m_font + c - ' ') | 0x80;
  }
  else {
    if (m_x == WIDTH) putchar('\n');
    m_x += 1;
    m_latest = c;
    segments = pgm_read_byte(m_font + c - ' ');
  }
  set((Register) m_x, segments);

  return (c & 0xff);
}
