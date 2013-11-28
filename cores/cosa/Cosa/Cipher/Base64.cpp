/**
 * @file Cosa/Cipher/Base64.cpp
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

#include "Cosa/Cipher/Base64.hh"

int 
Base64::encode(char* dest, const void* src, size_t size)
{
  const uint8_t* sp = (const uint8_t*) src;
  char* dp = dest;
  base64_t temp;
  int res = 0;

  // Encode three bytes block
  while (size > 2) {
    temp.d[0] = *sp++; 
    temp.d[1] = *sp++; 
    temp.d[2] = *sp++; 
    size = size - 3;
    *dp++ = temp.c0 + BASE;
    *dp++ = temp.c1 + BASE;
    *dp++ = temp.c2 + BASE;
    *dp++ = temp.c3 + BASE;
    res = res + 4;
  }

  // Pad and encode any remaining bytes
  if (size != 0) {
    for (uint8_t i = 0; i < size; i++) temp.d[i] = *sp++; 
    for (uint8_t i = size; i < 3; i++) temp.d[i] = 0;
    *dp++ = temp.c0 + BASE;
    *dp++ = temp.c1 + BASE;
    *dp++ = size > 1 ? temp.c2 + BASE : PAD;
    *dp++ = size > 2 ? temp.c3 + BASE : PAD;
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

  // Encode three byte blocks
  while (size > 2) {
    temp.d[0] = *sp++;
    temp.d[1] = *sp++;
    temp.d[2] = *sp++;
    size = size - 3;
    dest->putchar(temp.c0 + BASE);
    dest->putchar(temp.c1 + BASE);
    dest->putchar(temp.c2 + BASE);
    dest->putchar(temp.c3 + BASE);
    res = res + 4;
    if ((res & 0x3f) == 0) dest->putchar('\n');
  }

  // Pad and encode any remaining bytes
  if (size != 0) {
    for (uint8_t i = 0; i < size; i++) temp.d[i] = *sp++; 
    for (uint8_t i = size; i < 3; i++) temp.d[i] = 0;
    dest->putchar(temp.c0 + BASE);
    dest->putchar(temp.c1 + BASE);
    dest->putchar(size > 1 ? temp.c2 + BASE : PAD);
    dest->putchar(size > 2 ? temp.c3 + BASE : PAD);
    res = res + 4;
    if ((res & 0x3f) == 0) dest->putchar('\n');
  }

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
    temp.d[0] = pgm_read_byte(sp++);
    temp.d[1] = pgm_read_byte(sp++);
    temp.d[2] = pgm_read_byte(sp++);
    size = size - 3;
    *dp++ = temp.c0 + BASE;
    *dp++ = temp.c1 + BASE;
    *dp++ = temp.c2 + BASE;
    *dp++ = temp.c3 + BASE;
    res = res + 4;
  }

  // Pad and encode any remaining bytes
  if (size != 0) {
    for (uint8_t i = 0; i < size; i++) temp.d[i] = pgm_read_byte(sp++); 
    for (uint8_t i = size; i < 3; i++) temp.d[i] = 0;
    *dp++ = temp.c0 + BASE;
    *dp++ = temp.c1 + BASE;
    *dp++ = size > 1 ? temp.c2 + BASE : PAD;
    *dp++ = size > 2 ? temp.c3 + BASE : PAD;
    res = res + 4;
  }

  // Zero terminate string and return length
  *dp = 0;
  return (res);
}

int 
Base64::encode_P(IOStream::Device* dest, const void* src, size_t size)
{
  const uint8_t* sp = (const uint8_t*) src;
  base64_t temp;
  int res = 0;

  // Encode three byte blocks
  while (size > 2) {
    temp.d[0] = pgm_read_byte(sp++);
    temp.d[1] = pgm_read_byte(sp++);
    temp.d[2] = pgm_read_byte(sp++);
    size = size - 3;
    dest->putchar(temp.c0 + BASE);
    dest->putchar(temp.c1 + BASE);
    dest->putchar(temp.c2 + BASE);
    dest->putchar(temp.c3 + BASE);
    res = res + 4;
    if ((res & 0x3f) == 0) dest->putchar('\n');
  }

  // Pad and encode any remaining bytes
  if (size != 0) {
    for (uint8_t i = 0; i < size; i++) temp.d[i] = pgm_read_byte(sp++); 
    for (uint8_t i = size; i < 3; i++) temp.d[i] = 0;
    dest->putchar(temp.c0 + BASE);
    dest->putchar(temp.c1 + BASE);
    dest->putchar(size > 1 ? temp.c2 + BASE : PAD);
    dest->putchar(size > 2 ? temp.c3 + BASE : PAD);
    res = res + 4;
    if ((res & 0x3f) == 0) dest->putchar('\n');
  }

  return (res);
}

int 
Base64::decode(void* dest, const char* src, size_t size)
{
  // Check for illegal string length
  if (size & 0x3) return (-1);
  
  uint8_t* dp = (uint8_t*) dest;
  const char* sp = src;
  int res = 0;

  // Decode four character to three bytes
  while (size != 0) {
    base64_t temp;
    temp.c0 = (*sp++) - BASE;
    temp.c1 = (*sp++) - BASE;
    temp.c2 = (*sp++) - BASE;
    temp.c3 = (*sp++) - BASE;
    size = size - 4;
    for (uint8_t i = 0; i < 3; i++) 
      *dp++ = temp.d[i];
    res = res + 3;
  }

  // Return number of bytes
  return (res);
}
