/**
 * @file Cosa/Canvas/CompressedFont.hh
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

#ifndef COSA_CANVAS_COMPRESSEDFONT_HH
#define COSA_CANVAS_COMPRESSEDFONT_HH

#include "Cosa/Types.h"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Font.hh"

/*
 * Compressed bitmap font library handler.
 */
class CompressedFont : public Font {
public:
  /**
   * Construct font descriptor.
   * @param[in] width character width.
   * @param[in] height character height.
   * @param[in] first character available.
   * @param[in] last character available.
   * @param[in] compressed_bitmap font storage.
   * @param[in] offsets character offsets in bitmap.
   * @param[in] compression_type.
   * @param[in] spacing recommended character spacing.
   * @param[in] line_spacing recommended line spacing.
   */
  CompressedFont(uint8_t width, uint8_t height,
                 uint8_t first, uint8_t last,
                 const uint8_t* compressed_bitmap,
                 const uint16_t* offsets,
                 const uint8_t compression_type,
                 uint8_t spacing = 1, uint8_t line_spacing = 1) :
    Font(width, height, first, last, compressed_bitmap, spacing, line_spacing),
    m_offsets(offsets),
    m_compression_type(compression_type),
    m_zeros(0)
  {
  }

  /**
   * @override Font
   * Begin character display.
   * @param[in] c character.
   * @return iterator.
   */
  virtual display_iterator_t display_begin(char c);

  /**
   * @override Font
   * Get next byte for display.
   * @param[in,out] iterator.
   * @return byte for display.
   */
  virtual uint8_t display_next(display_iterator_t* iterator);
  
protected:
  /** Compressed font offsets. */
  const uint16_t* m_offsets;

  /** Compression type. */
  const uint8_t m_compression_type;

  /** Zeros left to supply. */
  uint8_t m_zeros;
};

#endif
