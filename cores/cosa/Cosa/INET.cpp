/**
 * @file Cosa/INET.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "Cosa/INET.hh"
#include <ctype.h>

bool
INET::is_illegal(uint8_t addr[4], uint16_t port)
{
  return
    (((addr[0] == 0xff)
      && (addr[1] == 0xff)
      && (addr[2] == 0xff)
      && (addr[3] == 0xff))
     ||
     ((addr[0] == 0x00)
      && (addr[1] == 0x00)
      && (addr[2] == 0x00)
      && (addr[3] == 0x00))
     ||
     (port == 0x0000));
}

int
INET::aton(const char* addr, uint8_t ip[4], bool progmem)
{
  const char* ap = addr;
  char c;
  for (uint8_t i = 0; i < IP_MAX; i++) {
    uint16_t r = 0;
    while (isdigit(c = (progmem ? pgm_read_byte(ap++) : *ap++)))
      r = (r * 10) + (c - '0');
    if (c != 0) {
      if (c != '.') return (EINVAL);
    }
    else {
      if (i < IP_MAX - 1) return (E2BIG);
    }
    if (r > 255) return (EOVERFLOW);
    ip[i] = r;
  }
  return (0);
}

int
INET::nametopath(const char* hostname, char* path, bool progmem)
{
  const char* hp = hostname;
  char* sp = path;
  char* np = path + 1;
  uint8_t n = 0;
  int res = 0;
  while (res < PATH_MAX) {
    char c = (progmem ? pgm_read_byte(hp++) : *hp++);
    if (c == 0 || c == '.') {
      if (UNLIKELY(n == 0)) return (EINVAL);
      *sp = n;
      res += n + 1;
      sp = np++;
      n = 0;
      if (c == 0) {
	*sp = 0;
	return (res + 1);
      };
    }
    else {
      *np++ = c;
      n++;
    }
  }
  return (EINVAL);
}

void
INET::print_path(IOStream& outs, const char* path)
{
  const char* p = path;
  uint8_t n = *p++;
  int i = 0;
  char c;
  while (i < PATH_MAX) {
    if (UNLIKELY(n == 0)) return;
    i += n;
    while (n--) {
      c = *p++;
      outs << c;
    }
    n = *p++;
    if (n != 0) outs << '.';
  }
}

void
INET::print_mac(IOStream& outs, const uint8_t* mac)
{
  outs << toHEX(mac[0] >> 4) << toHEX(mac[0]);
  for (uint8_t i = 1; i < MAC_MAX; i++)
    outs << ':' << toHEX(mac[i] >> 4) << toHEX(mac[i]);
}

void
INET::print_addr(IOStream& outs, const uint8_t* addr, uint16_t port)
{
  outs << addr[0];
  for (uint8_t i = 1; i < IP_MAX; i++) outs << '.' << addr[i];
  if (UNLIKELY(port == 0)) return;
  outs << ':' << port;
}

uint16_t
INET::checksum(const void* buf, size_t count)
{
  // Based on the C-code given in RFC 1071 (Computing the Internet
  // Checksum by R. Braden, D. Borman, and C. Partridge, 1988).
  const uint16_t* bp = (const uint16_t*) buf;
  uint32_t sum = 0L;

  // Sum up the buffer as 16-bit numbers
  while (count > 1) {
    sum += ntoh(*bp++);
    count -= 2;
  }

  // Add last byte if odd number of bytes
  if (count > 0)
    sum += *(const uint8_t*) bp;

  // Add carry bits
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  // And return the one-complement of the sum
  return (~sum);
}
