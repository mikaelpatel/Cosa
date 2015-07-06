/**
 * @file Base64.cpp
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

#include "Base64.hh"

const char Base64::ENCODE[] __PROGMEM =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char Base64::DECODE[] __PROGMEM =
  "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`"
  "abcdefghijklmnopq";

int
Base64::encode(char* dest, const void* src, size_t size)
{
  const uint8_t* sp = (const uint8_t*) src;
  char* dp = dest;
  base64_t temp;
  int res = 0;

  // Encode three bytes block
  while (size > 2) {
    temp.d[2] = *sp++;
    temp.d[1] = *sp++;
    temp.d[0] = *sp++;
    size = size - 3;
    *dp++ = encode(temp.c3);
    *dp++ = encode(temp.c2);
    *dp++ = encode(temp.c1);
    *dp++ = encode(temp.c0);
    res = res + 4;
  }

  // Pad and encode any remaining bytes
  if (size != 0) {
    temp.d[2] = *sp++;
    temp.d[1] = (size > 1 ? *sp : 0);
    temp.d[0] = 0;
    *dp++ = encode(temp.c3);
    *dp++ = encode(temp.c2);
    *dp++ = size > 1 ? encode(temp.c1) : PAD;
    *dp++ = PAD;
    res = res + 4;
  }

  // Zero terminate string and return length
  *dp = 0;
  return (res);
}

int
Base64::encode_P(char* dest, const void* src, size_t size)
{
  const uint8_t* sp = (const uint8_t*) src;
  char* dp = dest;
  base64_t temp;
  int res = 0;

  // Encode three byte blocks
  while (size > 2) {
    temp.d[2] = pgm_read_byte(sp++);
    temp.d[1] = pgm_read_byte(sp++);
    temp.d[0] = pgm_read_byte(sp++);
    size = size - 3;
    *dp++ = encode(temp.c3);
    *dp++ = encode(temp.c2);
    *dp++ = encode(temp.c1);
    *dp++ = encode(temp.c0);
    res = res + 4;
  }

  // Pad and encode any remaining bytes
  if (size != 0) {
    temp.d[2] = pgm_read_byte(sp++);
    temp.d[1] = size > 1 ? pgm_read_byte(sp) : 0;
    temp.d[0] = 0;
    *dp++ = encode(temp.c3);
    *dp++ = encode(temp.c2);
    *dp++ = size > 1 ? encode(temp.c1) : PAD;
    *dp++ = PAD;
    res = res + 4;
  }

  // Zero terminate string and return length
  *dp = 0;
  return (res);
}

int
Base64::encode(IOStream::Device* dest, const void* src, size_t size)
{
  const uint8_t* sp = (const uint8_t*) src;
  base64_t temp;
  int res = 0;

  // Encode three byte blocks with line break every 64 characters
  while (size > 2) {
    temp.d[2] = *sp++;
    temp.d[1] = *sp++;
    temp.d[0] = *sp++;
    size = size - 3;
    dest->putchar(encode(temp.c3));
    dest->putchar(encode(temp.c2));
    dest->putchar(encode(temp.c1));
    dest->putchar(encode(temp.c0));
    res = res + 4;
    if ((res & 0x3f) == 0) dest->puts((str_P) IOStream::CRLF);
  }

  // Pad and encode any remaining bytes with possible line break
  if (size != 0) {
    temp.d[2] = *sp++;
    temp.d[1] = size > 1 ? *sp : 0;
    temp.d[0] = 0;
    dest->putchar(encode(temp.c3));
    dest->putchar(encode(temp.c2));
    dest->putchar(size > 1 ? encode(temp.c1) : PAD);
    dest->putchar(PAD);
    res = res + 4;
    if ((res & 0x3f) == 0) dest->puts((str_P) IOStream::CRLF);
  }

  return (res);
}

int
Base64::encode_P(IOStream::Device* dest, const void* src, size_t size)
{
  const uint8_t* sp = (const uint8_t*) src;
  base64_t temp;
  int res = 0;

  // Encode three byte blocks with line break every 64 characters
  while (size > 2) {
    temp.d[2] = pgm_read_byte(sp++);
    temp.d[1] = pgm_read_byte(sp++);
    temp.d[0] = pgm_read_byte(sp++);
    size = size - 3;
    dest->putchar(encode(temp.c3));
    dest->putchar(encode(temp.c2));
    dest->putchar(encode(temp.c1));
    dest->putchar(encode(temp.c0));
    res = res + 4;
    if ((res & 0x3f) == 0) dest->puts((str_P) IOStream::CRLF);
  }

  // Pad and encode any remaining bytes with possible line break
  if (size != 0) {
    temp.d[2] = pgm_read_byte(sp++);
    temp.d[1] = size > 1 ? pgm_read_byte(sp) : 0;
    temp.d[0] = 0;
    dest->putchar(encode(temp.c3));
    dest->putchar(encode(temp.c2));
    dest->putchar(size > 1 ? encode(temp.c1) : PAD);
    dest->putchar(PAD);
    res = res + 4;
    if ((res & 0x3f) == 0) dest->puts((str_P) IOStream::CRLF);
  }

  return (res);
}

int
Base64::decode(void* dest, const char* src, size_t size)
{
  // Check for illegal length (even 4 character blocks)
  if (UNLIKELY((size & 0x3) != 0)) return (EINVAL);

  uint8_t* dp = (uint8_t*) dest;
  const char* sp = src;
  int res = 0;

  // Decode four character to three bytes
  while (size != 0) {
    base64_t temp;
    temp.c3 = decode(*sp++);
    temp.c2 = decode(*sp++);
    temp.c1 = decode(*sp++);
    temp.c0 = decode(*sp++);
    size = size - 4;
    *dp++ = temp.d[2];
    *dp++ = temp.d[1];
    *dp++ = temp.d[0];
    res = res + 3;
  }

  // Return number of bytes
  return (res);
}
