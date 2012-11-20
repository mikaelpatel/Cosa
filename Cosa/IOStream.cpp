/**
 * @file Cosa/IOStream.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * @section Description
 * Basic in-/output stream support class. Requires implementation of
 * Stream::Device. See UART for an example.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream.h"
#include <stdarg.h>

IOStream::Device IOStream::Device::null;

void 
IOStream::print(int n, uint8_t base) 
{
  if (base != 10) print_prefix(base);
  char buf[sizeof(int) * CHARBITS + 1];
  print(itoa(n, buf, base));
}

void 
IOStream::print(long int n, uint8_t base)
{
  if (base != 10) print_prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ltoa(n, buf, base));
}

void 
IOStream::print(unsigned int n, uint8_t base) 
{
  if (base != 10) print_prefix(base);
  char buf[sizeof(int) * CHARBITS + 1];
  print(utoa(n, buf, base));
}

void 
IOStream::print(unsigned long int n, uint8_t base)
{
  if (base != 10) print_prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ultoa(n, buf, base));
}

void 
IOStream::vprintf(const char* format, va_list args)
{
  const char* s = format;
  uint8_t is_signed;
  uint8_t base;
  char c;
  while ((c = pgm_read_byte(s++)) != 0) {
    if (c == '%') {
      is_signed = 1;
      base = 10;
    next:
      c = pgm_read_byte(s++);
      if (c == 0) s--;
      switch (c) {
      case 'b': 
	base = 2; 
	goto next;
      case 'o': 
	base = 8; 
	goto next;
      case 'h': 
	base = 16; 
	goto next;
      case 'u': 
	is_signed = 0; 
	goto next;
      case 'p': 
	print(va_arg(args, void*)); 
	continue;
      case 'P': 
	print(va_arg(args, const void*)); 
	continue;
      case 's': 
	print(va_arg(args, char*)); 
	continue;
      case 'S': 
	print(va_arg(args, const char*)); 
	continue;
      case 'd': 
	if (is_signed) 
	  print(va_arg(args, int), base); 
	else
	  print(va_arg(args, unsigned int), base); 
	continue;
      case 'l': 
	if (is_signed) 
	  print(va_arg(args, long int), base);
	else
	  print(va_arg(args, unsigned long int), base);
	continue;
      };
    }
    print(c);
  }
}

void 
IOStream::print_prefix(uint8_t base)
{
  if (base == 2)
    print_P(PSTR("0b"));
  else if (base == 8)
    print_P(PSTR("0"));
  else if (base == 16)
    print_P(PSTR("0x"));
}

