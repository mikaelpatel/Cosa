/**
 * @file Cosa/Canvas/CompressedFont.cpp
 * @version 1.0
 *
 * @author contributed by jediunix
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

#include "Cosa/Canvas/CompressedFont.hh"

Font::display_iterator_t
CompressedFont::display_begin(char c)
{
  uint16_t chr = c;

  if (chr < FIRST || chr > LAST)
    return (NULL);

  uint16_t offset = pgm_read_word(&m_offsets[chr - FIRST]);

  return ((Font::display_iterator_t)&m_bitmap[offset]);
}

uint8_t
CompressedFont::display_next(display_iterator_t* iterator)
{
  if (*iterator)
    switch (m_compression_type)
      {
      case 1:
        if (m_zeros)
          {
            m_zeros--;
            return (0);
          }

        uint8_t byte = pgm_read_byte((*iterator)++);
        if (byte == 0)
          m_zeros = pgm_read_byte((*iterator)++);

        return (byte);
      }

  return (0x55);  // unrecognized compression shows streaks
}
