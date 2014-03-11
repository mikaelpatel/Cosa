/**
 * @file Cosa/Cipher/RC4.hh
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

#ifndef __COSA_CIPHER_RC4_HH__
#define __COSA_CIPHER_RC4_HH__

#include "Cosa/Types.h"

/**
 * RC4 cipher.
 * @section See Also
 * 1. http://en.wikipedia.org/wiki/RC4
 * 2. http://cypherpunks.venona.com/archive/1994/09/msg00304.html
 */
class RC4 {
public:
  /**
   * Construct RC4 cipher for given key and length.
   * @param[in] key pointer to key.
   * @param[in] len length of key in bytes.
   */
  RC4(const void* key, size_t len)
  {
    restart(key, len);
  }

  /**
   * Restart the given key and length.
   * @param[in] key pointer to key.
   * @param[in] len length of key in bytes.
   */
  void restart(const void* key, size_t len);

  /**
   * Encrypt the given character.
   * @param[in] c character to encode.
   * @return encoded character.
   */
  char encrypt(char c) 
  {
    m_x += 1;
    uint8_t sx = m_state[m_x];
    m_y += sx;
    uint8_t sy = m_state[m_y];
    m_state[m_x] = sy;
    m_state[m_y] = sx;
    uint8_t ix = sx + sy;
    return (c ^ m_state[ix]);
  }
  
  /**
   * Encrypt the given buffer.
   * @param[in] buf buffer pointer.
   * @param[in] n number of bytes.
   */
  void encrypt(void* buf, size_t n)
  {
    for (char* bp = (char*) buf; n--; bp++)
      *bp = encrypt(*bp);
  }

  /**
   * Encrypt the given src buffer to the dest buffer.
   * @param[in] dest buffer pointer.
   * @param[in] src buffer pointer.
   * @param[in] n number of bytes.
   */
  void encrypt(void* dest, const void* src, size_t n) 
  {
    char* dp = (char*) dest;
    const char* sp = (const char*) src;
    while (n--) *dp++ = encrypt(*sp++);
  }

  /**
   * Decrypt the given character.
   * @param[in] c character to decode.
   * @return decoded character.
   */
  char decrypt(char c)
  {
    return (encrypt(c));
  }
  
  /**
   * Decrypt the given buffer.
   * @param[in] buf buffer pointer.
   * @param[in] n number of bytes.
   */
  void decrypt(void* buf, size_t n) 
  {
    for (char* bp = (char*) buf; n--; bp++)
      *bp = decrypt(*bp);
  }

  /**
   * Decrypt the given src buffer to the dest buffer.
   * @param[in] dest buffer pointer.
   * @param[in] src buffer pointer.
   * @param[in] n number of bytes.
   */
  void decrypt(void* dest, const void* src, size_t n) 
  {
    char* dp = (char*) dest;
    const char* sp = (const char*) src;
    while (n--) *dp++ = decrypt(*sp++);
  }

private:
  uint8_t m_state[256];
  uint8_t m_x;
  uint8_t m_y;
};

#endif
