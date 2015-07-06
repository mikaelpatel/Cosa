/**
 * @file Font.cpp
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

#include "Font.hh"

void
Font::draw(Canvas* canvas, char c, uint16_t x, uint16_t y,
           uint8_t scale)
{
  Glyph glyph(this, c);

  if (scale == 1) {
    for (uint16_t i = 0; i < HEIGHT; i += 8) {
      for (uint16_t j = 0; j < WIDTH; j++) {
        uint8_t bits = glyph.next();
        if (bits == 0xff) {
          canvas->draw_vertical_line(x + j, y + i, CHARBITS);
        }
        else {
          for (uint8_t k = 0; k < 8; k++) {
            if (bits == 0) break;
            if (bits & 1) canvas->draw_pixel(x + j, y + k + i);
            bits >>= 1;
          }
        }
      }
    }
  }
  else {
    for (uint16_t i = 0; i < HEIGHT; i += 8) {
      for (uint16_t j = 0; j < WIDTH; j++) {
        uint8_t bits = glyph.next();
        for (uint8_t k = 0; k < 8; k++) {
          if (bits == 0) break;
          if (bits & 1) canvas->fill_rect(x + j*scale, y + (k+i)*scale, scale, scale);
          bits >>= 1;
        }
      }
    }
  }
}

#define ESCAPED_BITSET 0x1

void
Font::Glyph::begin(char c)
{
  unsigned char chr = c;

  if (chr < m_font->FIRST || chr > m_font->LAST) {
    m_bitmap = NULL;
    return;
  }

  m_offset = 0;
  m_flags = 0;

  uint8_t uncompressed_size = m_font->WIDTH * ((m_font->HEIGHT + (CHARBITS-1)) / CHARBITS);

  switch (m_font->m_compression_type)
    {
    case 0:  // uncompressed
      m_bitmap = (uint8_t*)&m_font->m_bitmap[(chr - m_font->FIRST)*uncompressed_size];
      break;

    case 1:  // non-zero "present" bitset
      uint16_t bitset_offset;
      uint8_t bitset_size;

      bitset_offset = pgm_read_byte(&m_font->m_bitmap[(chr - m_font->FIRST)*2]) << 8;
      bitset_offset |= pgm_read_byte(&m_font->m_bitmap[(chr - m_font->FIRST)*2 + 1]);
      // indicates an escaped bitset (twice as wide)
      if (bitset_offset & 0x8000) {
	m_flags |= ESCAPED_BITSET;
	bitset_offset &= 0x7FFF;
      }
      m_bitset = (uint8_t*)&m_font->m_bitmap[bitset_offset];

      bitset_size = (uncompressed_size + (CHARBITS-1)) / CHARBITS;
      if (m_flags & ESCAPED_BITSET)
        bitset_size *=2 ;

      m_bitmap = m_bitset + bitset_size;
      m_next = 0;
      break;

    default:
      m_bitmap = NULL;
    }
}

uint8_t
Font::Glyph::next()
{
  // unrecognized compression type or
  // character not available in font shows streaks
  if (!m_bitmap)
    return (0x55);

  uint8_t result = 0;

  switch (m_font->m_compression_type)
    {
    case 0:  // uncompressed
      result = pgm_read_byte(&m_bitmap[m_offset++]);
      break;

    case 1:  // non-zero "present" bitset
      uint8_t bitset_offset = m_offset >> 3;
      if (m_flags & ESCAPED_BITSET)
        bitset_offset = (bitset_offset*2)+1;
      uint8_t bitset_bit = 1 << (7 - (m_offset % 8));

      m_offset++;

      // If byte is present
      if (pgm_read_byte(&m_bitset[bitset_offset]) & bitset_bit)
        result = pgm_read_byte(&m_bitmap[m_next++]);
      break;
    }

  return(result);
}
