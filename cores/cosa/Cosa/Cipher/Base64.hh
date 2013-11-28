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
 * Non-standard ultra fast Base64 encoder/decoder. Maps 24-bits to
 * printable 32-bits (4 characters), 3x8 => 4X6. Each 6-bit binary
 * value is encoded by added the BASE character, '0'. The mapping
 * gives the character range '0'..'_'. Three character blocks are
 * padded with the PAD character.
 */
class Base64 {
private:
  /** Base character for mapping binary value (0..63) to character */
  static const char BASE = '0';

  /** Padding character for last encoded block */
  static const char PAD = BASE + 64;

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
   * Encode the size number of bytes in the source buffer to given 
   * iostream device. 
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
   * encoded data and the terminating null.
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
