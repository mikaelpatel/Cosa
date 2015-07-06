/**
 * @file Vigenere.hh
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

#ifndef COSA_VIGENERE_HH
#define COSA_VIGENERE_HH

#include "Cosa/Types.h"

/**
 * Vigenere auto-key cipher.
 * @param[in] N number of bytes in key.
 *
 * @section References
 * http://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher
 */
template<uint8_t N>
class Vigenere {
public:
  /**
   * Construct Vigenere crypto with key generated from given seed.
   * An autokey is generated when key length (n) is less than max key
   * length (N).
   * @param[in] seed for random key generation.
   * @param[in] n length of random key.
   */
  Vigenere(uint32_t seed, uint8_t n = N)
  {
    if (UNLIKELY(n > N)) n = N;
    srandom(seed);
    for (uint8_t i = 0; i < n; i++)
      m_key[i] = random();
    m_nr = 0;
    m_max = n;
    m_pos = n;
  }

  /**
   * Construct Vigenere crypto with given password. An autokey is
   * generated when the password length is less than max key length (N).
   * @param[in] password.
   */
  Vigenere(const char* password)
  {
    uint8_t i;
    for (i = 0; i < N && password[i]; i++)
      m_key[i] = password[i];
    m_nr = 0;
    m_max = i;
    m_pos = i;
  }

  /**
   * Restart the crypto. Generate a new auto-key.
   */
  void restart()
  {
    m_max = m_pos;
    m_nr = 0;
  }

  /**
   * Encrypt the given character.
   * @param[in] c character to encode.
   * @return encoded character.
   */
  char encrypt(char c)
  {
    char res = c + m_key[m_nr++];
    if (m_max != N) m_key[m_max++] = c;
    if (m_nr == N) m_nr = 0;
    return (res);
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
    char res = c - m_key[m_nr++];
    if (m_max != N) m_key[m_max++] = res;
    if (m_nr == N) m_nr = 0;
    return (res);
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
  char m_key[N];
  uint8_t m_nr;
  uint8_t m_max;
  uint8_t m_pos;
};

#endif
