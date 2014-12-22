/**
 * @file Cosa/Canvas/Glyph.cpp
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

#include "Cosa/Canvas/Glyph.hh"

Glyph::Glyph(uint8_t width, uint8_t height) :
  WIDTH(width),
  HEIGHT(height),
  m_offset(0)
{
  m_image = (uint8_t*) malloc(width * GLYPH_BITS_TO_BYTES(height));
  // malloc failure is handled by always checking m_image before referencing
}

Glyph::~Glyph()
{
  if (m_image)
    {
      free(m_image);
      m_image = NULL;
    }
}

uint8_t
Glyph::next()
{
  if (m_image && !eog())
    return (*(m_image + m_offset++));

  return (0xFF);
}

const uint8_t*
Glyph::next_stripe()
{
  if (m_image && !eog())
    {
      uint8_t* stripe = m_image + m_offset;
      m_offset += WIDTH;
      return (stripe);
    }

  return (NULL);
}

IOStream&
operator<<(IOStream& outs, Glyph& glyph)
{
  IOStream::Device* dev = outs.get_device();

  dev->putchar('+');
  for (uint8_t x = 0; x < glyph.WIDTH; x++)
    dev->putchar('-');
  dev->putchar('+');
  dev->putchar('\n');

  for (uint8_t y = 0; y < glyph.HEIGHT; y++)
    {
      dev->putchar('|');
      for (uint8_t x = 0; x < glyph.WIDTH; x++)
        if (glyph.pixel(x, y))
          dev->putchar('@');
        else
          dev->putchar('.');
      dev->putchar('|');
      dev->putchar('\n');
    }

  dev->putchar('+');
  for (uint8_t x = 0; x < glyph.WIDTH; x++)
    dev->putchar('-');
  dev->putchar('+');
  dev->putchar('\n');

  return (outs);
}
