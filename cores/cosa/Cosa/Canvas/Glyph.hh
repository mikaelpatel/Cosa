/**
 * @file Cosa/Canvas/Glyph.hh
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

/**
 * Common usage - one byte at a time.
 *
 * // position 0, 0
 *
 * glyph.begin();
 *
 * do
 *   {
 *     do
 *       {
 *
 *         uint8_t byte = glyph.next();
 *
 *         // send byte to display
 *         // increment column
 *
 *       } while (!glyph.eos());
 *
 *     // increment-display-stripe, reset column
 *
 *   } while (!glyph.eog());
 *
 *  glyph.end();
 */

/**
 * Common usage - one stripe at a time.
 *
 * // position 0, 0
 *
 * glyph.begin();
 *
 * do
 *   {
 *     do
 *       {
 *
 *         uint8_t* stripe = glyph.next_stripe();
 *
 *         // send stripe (<width> bytes) to display
 *
 *       } while (!glyph.eos());
 *
 *     // increment-display-stripe, reset column
 *
 *   } while (!glyph.eog());
 *
 *  glyph.end();
 */

#ifndef COSA_CANVAS_GLYPH_HH
#define COSA_CANVAS_GLYPH_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

#define GLYPH_BITS_TO_BYTES(x) ((x + (CHARBITS-1))/CHARBITS)

class Glyph {
public:
  /** Width */
  const uint8_t WIDTH;

  /** Height */
  const uint8_t HEIGHT;

  /**
   * Construct glyph.
   * @param[in] width.
   * @param[in] height.
   */
  Glyph(uint8_t width, uint8_t height);

  /**
   * Destroy glyph.
   */
  ~Glyph();

  /**
   * @override Glyph
   * Begin iterator for access glyph image.
   */
  void begin();
  
  /**
   * @override Glyph
   * End iterator.
   */
  void end();
  
  /**
   * Get value of a pixel.
   * @param[in] x.
   * @param[in] y.
   * @return true if set.
   */
  bool pixel(uint8_t x, uint8_t y)
    __attribute__((always_inline))
  {
    if (!m_image)
      return (true);
    if (x > WIDTH || y > HEIGHT)
      return (false);
    return ((m_image[((y >> 3) * WIDTH) + x] >> (y - ((y >> 3)*8))) & 0x1);
  }

  
  /**
   * @override Glyph
   * Determine if iteration has reached end of glyph.
   * @return true if end has been reached.
   */
  bool eog();
  
  /**
   * @override Glyph
   * Determine if iteration has reached end of stripe.
   * @return true if end of band has been reached.
   */
  bool eos();
  
  /**
   * @override Glyph
   * Get next element.
   * @return element.
   */
  uint8_t next();

  /**
   * @override Glyph
   * Get next stripe.
   * @return array of bytes in stripe of length width().
   */
  const uint8_t* next_stripe();

  /**
   * Print glyph image to the given output stream.
   * @param[in] outs output stream.
   * @param[in] glyph to print.
   * @return output stream.
   */
  friend IOStream& operator<<(IOStream& outs, Glyph& glyph);

protected:
  /** Image storage */
  uint8_t* m_image;

  /** Iterator offset */
  uint8_t m_offset;
};
#endif
