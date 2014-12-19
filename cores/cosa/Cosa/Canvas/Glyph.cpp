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

void
Glyph::begin()
{
  m_offset = 0;
}

void
Glyph::end()
{
  m_offset = 0;
}

bool
Glyph::eog()
{
  return (!((int16_t)m_offset < (int16_t)(WIDTH * GLYPH_BITS_TO_BYTES(HEIGHT))));
}

bool
Glyph::eos()
{
  if (m_offset == 0)
    return (false);
  else
    return (!(m_offset % WIDTH));
}

uint8_t
Glyph::next()
{
  uint8_t element;


  if (!m_image)
    return (0);

  if (!m_image)
    element = 0xFF;
  else
    element = *(m_image + m_offset);

  if (!eog())
    {
      m_offset++;
      return (element);
    }
  else
    return (0);
}

const uint8_t*
Glyph::next_stripe()
{
  if (!m_image)
    return (NULL);

  uint8_t* stripe = m_image + m_offset;

  if (!eog())
    {
      m_offset += WIDTH;
      return (stripe);
    }
  else
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
