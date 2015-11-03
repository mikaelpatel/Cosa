/**
 * @file HammingCodec_8_4.hh
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

#ifndef COSA_HAMMINGCODEC_8_4_HH
#define COSA_HAMMINGCODEC_8_4_HH

#include <VWI.h>

/**
 * Hamming(8,4) Codec for the Cosa VWI (Virtual Wire Interface). This
 * is a linear error-correcting code that encodes 4 bits of data into
 * 8 bits by adding 4 parity bits. Hamming's (8,4) algorithm can
 * correct any single-bit error, or detect all single-bit and two-bit
 * errors. This implementation uses inverted parity bits to improve DC
 * balance in transmission (e.g. avoid coding long sequences of zero
 * or one).
 *
 * @section References
 * 1. http://en.wikipedia.org/wiki/Hamming_code#.5B7.2C4.5D_Hamming_code_with_an_additional_parity_bit
 */
class HammingCodec_8_4 : public VWI::Codec {
public:
  /**
   * Construct Hamming(8,4) codec with given bits per symbol, start
   * symbol, and preamble size.
   */
  HammingCodec_8_4() :
    VWI::Codec(8, 0x5a55, 8)
  {
  }

  /**
   * @override{VWI::Codec}
   * Returns pointer to Hamming frame preamble in program memory.
   * @return pointer.
   */
  virtual const uint8_t* preamble()
  {
    return (s_preamble);
  }

  /**
   * @override{VWI::Codec}
   * Returns symbol for given 4-bit data.
   * @param[in] nibble data.
   * @return 8-bit symbol code.
   */
  virtual uint8_t encode4(uint8_t nibble)
  {
    return (pgm_read_byte(&s_symbols[nibble & 0xf]));
  };

  /**
   * @override{VWI::Codec}
   * Returns 4-bit data for given symbol.
   * @param[in] symbol code.
   * @return 4-bit data.
   */
  virtual uint8_t decode4(uint8_t symbol)
  {
    uint8_t code = pgm_read_byte(&s_codes[symbol >> 1]);
    return ((symbol & 0x01) ? (code & 0x0f) : (code >> 4));
  }

private:
  /** Symbol mapping table: 4 to 8 bits. */
  static const uint8_t s_symbols[] PROGMEM;

  /** Code mapping table: 8 to 4-bit code (packed table). */
  static const uint8_t s_codes[] PROGMEM;

  /** Message preamble with start symbol. */
  static const uint8_t s_preamble[] PROGMEM;
};

#endif
