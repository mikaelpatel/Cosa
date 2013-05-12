/**
 * @file Cosa/IOStream.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/IOStream.hh"
#include <stdarg.h>

IOStream::Device* 
IOStream::set_device(Device* dev) 
{ 
  Device* previous = m_dev;
  if (dev == 0) dev = &Device::null;
  m_dev = dev;
  return (previous);
}

void 
IOStream::print(int n, Base base) 
{
  if (base != bcd) {
    print_prefix(base);
    char buf[sizeof(int) * CHARBITS + 1];
    print(itoa(n, buf, base));
  }
  else {
    print((char) ('0' + ((n >> 4) & 0xf)));
    print((char) ('0' + (n & 0xf)));
  }
}

void 
IOStream::print(long int n, Base base)
{
  print_prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ltoa(n, buf, base));
}

void 
IOStream::print(unsigned int n, Base base) 
{
  print_prefix(base);
  char buf[sizeof(int) * CHARBITS + 1];
  print(utoa(n, buf, base));
}

void 
IOStream::print(unsigned long int n, Base base)
{
  print_prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ultoa(n, buf, base));
}

void 
IOStream::print(IOStream::Device* buffer)
{
  int c;
  while ((c = buffer->getchar()) != -1)
    print((char) c);
}

void 
IOStream::print_prefix(Base base)
{
  if (base == dec)
    return;
  else if (base == hex)
    print_P(PSTR("0x"));
  else if (base == bin)
    print_P(PSTR("0b"));
  else if (base == oct)
    print_P(PSTR("0"));
}

void 
IOStream::print(void *ptr, size_t size, Base base, uint8_t max)
{
  uint8_t* p = (uint8_t*) ptr;
  unsigned int v_adj = (base == dec ? 0 : (base == oct ? 01000 : 0x100));
  uint8_t adj = (v_adj != 0);
  uint8_t n = 0;
  while (size--) {
    if (n == 0) {
      print(p);
      print_P(PSTR(": "));
    }
    char buf[sizeof(int) * CHARBITS + 1];
    unsigned int v = (*p++) + v_adj;
    print(utoa(v, buf, base) + adj);
    if (++n < max) {
      print_P(PSTR(" "));
    }
    else {
      println();
      n = 0;
    }
  }
  if (n != 0) println();
}

void 
IOStream::vprintf_P(const char* format, va_list args)
{
  const char* s = format;
  uint8_t is_signed;
  Base base;
  char c;
  while ((c = pgm_read_byte(s++)) != 0) {
    if (c == '%') {
      is_signed = 1;
      base = dec;
    next:
      c = pgm_read_byte(s++);
      if (c == 0) s--;
      switch (c) {
      case 'b': 
	base = bin; 
	goto next;
      case 'B': 
	base = bcd; 
	goto next;
      case 'o': 
	base = oct; 
	goto next;
      case 'h':
      case 'x': 
	base = hex; 
	goto next;
      case 'u': 
	is_signed = 0; 
	goto next;
      case 'c': 
	print((char) va_arg(args, int)); 
	continue;
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

IOStream& 
bcd(IOStream& outs)
{
  outs.m_base = IOStream::bcd;
  return (outs);
}

IOStream& 
bin(IOStream& outs)
{
  outs.m_base = IOStream::bin;
  return (outs);
}

IOStream& 
oct(IOStream& outs)
{
  outs.m_base = IOStream::oct;
  return (outs);
}

IOStream& 
dec(IOStream& outs)
{
  outs.m_base = IOStream::dec;
  return (outs);
}

IOStream& 
hex(IOStream& outs)
{
  outs.m_base = IOStream::hex;
  return (outs);
}

IOStream& 
endl(IOStream& outs)
{
  outs.print('\n');
  return (outs);
}

IOStream& 
ends(IOStream& outs)
{
  outs.print('\0');
  return (outs);
}

IOStream& 
clear(IOStream& outs)
{
  outs.print('\f');
  return (outs);
}

