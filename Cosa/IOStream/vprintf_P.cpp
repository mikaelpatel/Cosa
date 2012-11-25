/**
 * @file Cosa/IOStream/vprintf_P.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Variable argument formated output.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOStream.h"
#include <stdarg.h>

void 
IOStream::vprintf_P(const char* format, va_list args)
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
      case 's': 
	print(va_arg(args, char*)); 
	continue;
      case 'S': 
	print_P(va_arg(args, const char*)); 
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
