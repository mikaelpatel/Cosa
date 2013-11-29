/**
 * @file Cosa/Cipher/Base64.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CIPHER_BASE64_HH__
#define __COSA_CIPHER_BASE64_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Base64 encoder/decoder. Maps 24-bits to printable 32-bits (4
 * characters), 3x8 => 4X6. Allows encoding directly to an
 * IOStream::Device such as the UART. Long string to an device is
 * broken into multiple lines with a max length of 64 characters.
 */
class Base64 {
private:
  /** Padding character for last encoded block */
  static const char PAD = '=';

  /** Mapping between 3-characters and 4X6-bits */ 
  union base64_t {
    uint8_t d[3];
    struct {
      uint8_t c0:6;
      uint8_t c1:6;
      uint8_t c2:6;
      uint8_t c3:6;
    };
  };

  /** Encode table */
  static const char ENCODE[] PROGMEM;
  static char encode(uint8_t bits)
  {
    return (pgm_read_byte(&ENCODE[bits]));
  }

public:
  /** Decode table */
  static const char DECODE[] PROGMEM;
  static uint8_t decode(char c)
  {
    if (c < 43 || c > 122) return (0);
    uint8_t bits = pgm_read_byte(&DECODE[c - 43]);
    return (bits == '$' ? 0 : bits - 62);
  }

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
   * Encode the size number of bytes in the source buffer to given 
   * iostream device. New-line is emitted every 64 characters (16 code
   * blocks).
   * @param[in] dest output stream device.
   * @param[in] src source buffer pointer (binary).
   * @param[in] size number of bytes to encode.
   * @return length of string or negative error code.
   */
  static int encode(IOStream::Device* dest, const void* src, size_t size);

  /**
   * Encode the size number of bytes in the source buffer to a
   * null-terminated printable string in the given destination
   * buffer. The destination buffer must be able to hold the 
   * encoded data and the terminating null. New-line is emitted every
   * 64 characters (16 code 
   * blocks).

   * @param[in] dest destination buffer pointer (string).
   * @param[in] src source buffer pointer (binary, in program memory).
   * @param[in] size number of bytes to encode.
   * @return length of string or negative error code.
   */
  static int encode_P(char* dest, const void* src, size_t size);

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
   * buffer must be able to hold the decoded data.
   * @param[in] dest destination buffer pointer (binary).
   * @param[in] src source buffer pointer (string).
   * @param[in] size number of bytes to decode.
   * @return number of decoded bytes or negative error code.
   */
  static int decode(void* dest, const char* src, size_t size);
};

#endif
