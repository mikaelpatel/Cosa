/**
 * @file Base64.hh
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

#ifndef COSA_BASE64_HH
#define COSA_BASE64_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Base64 encoder/decoder. Maps 3 bytes (24-bits) binary data to
 * 4 printable characters, 32-bits. Allows encoding directly to an
 * IOStream::Device such as the UART. Long string to an device is
 * broken into multiple lines with a max length of 64 characters.
 *
 * @section Acknowledgements
 * Inspired by implementation method by Bob Trower and Arduino Forum
 * discussions.
 *
 * @section References
 * 1. http://en.wikipedia.org/wiki/Base64
 * 2. https://tools.ietf.org/html/rfc4648
 * 3. http://base64.sourceforge.net
 */
class Base64 {
public:
  /**
   * Encode the size number of bytes in the source buffer to a
   * null-terminated printable string in the given destination
   * buffer. The destination buffer must be able to hold the
   * encoded data and the terminating null.
   * @param[in] dest destination buffer pointer (string).
   * @param[in] src source buffer pointer (binary).
   * @param[in] size number of bytes to encode.
   * @return length of string or negative error code.
   */
  static int encode(char* dest, const void* src, size_t size);

  /**
   * Encode the size number of bytes in the source buffer to a
   * null-terminated printable string in the given destination
   * buffer. The destination buffer must be able to hold the
   * encoded data and the terminating null. A new-line is emitted
   * every 64 characters.
   * @param[in] dest destination buffer pointer (string).
   * @param[in] src source buffer pointer (binary, in program memory).
   * @param[in] size number of bytes to encode.
   * @return length of string or negative error code.
   */
  static int encode_P(char* dest, const void* src, size_t size);

  /**
   * Encode the size number of bytes in the source buffer to given
   * iostream device. A new-line is emitted every 64 characters.
   * @param[in] dest output stream device.
   * @param[in] src source buffer pointer (binary).
   * @param[in] size number of bytes to encode.
   * @return length of string or negative error code.
   */
  static int encode(IOStream::Device* dest, const void* src, size_t size);

  /**
   * Encode the size number of bytes in the source buffer to given
   * iostream device.
   * @param[in] dest output stream device.
   * @param[in] src source buffer pointer (binary, in program memory).
   * @param[in] size number of bytes to encode.
   * @return length of string or negative error code.
   */
  static int encode_P(IOStream::Device* dest, const void* src, size_t size);

  /**
   * Decode the size number of bytes in the source buffer to binary
   * representation in the given destination buffer. The destination
   * buffer must be able to hold the decoded data. The source buffer
   * size must be even 4 character blocks (EINVAL is returned if not).
   * @param[in] dest destination buffer pointer (binary).
   * @param[in] src source buffer pointer (string).
   * @param[in] size number of bytes to decode.
   * @return number of decoded bytes or negative error code.
   */
  static int decode(void* dest, const char* src, size_t size);

private:
  /** Padding character for last encoded block */
  static const char PAD = '=';

  /** Mapping between 3-characters and 4-bitfields(6 bits) */
  union base64_t {
    uint8_t d[3];
    struct {
      uint8_t c0:6;
      uint8_t c1:6;
      uint8_t c2:6;
      uint8_t c3:6;
    };
  };

  /** Encoding table in program memory */
  static const char ENCODE[] PROGMEM;

  /**
   * Encode given 6-bit number to a character.
   * @param[in] bits to encode.
   * @return character.
   */
  static char encode(uint8_t bits)
    __attribute__((always_inline))
  {
    return (pgm_read_byte(&ENCODE[bits]));
  }

  /** Decoding table in program memory */
  static const char DECODE[] PROGMEM;

  /**
   * Decode given character to 6-bit number.
   * @param[in] c character to decode.
   * @return 6-bit representation.
   */
  static uint8_t decode(char c)
    __attribute__((always_inline))
  {
    if (UNLIKELY(c < 43 || c > 122)) return (0);
    uint8_t bits = pgm_read_byte(&DECODE[c - 43]);
    return (bits == '$' ? 0 : bits - 62);
  }
};

#endif
