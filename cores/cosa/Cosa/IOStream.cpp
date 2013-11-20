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
#include "Cosa/Power.hh"

IOStream::Filter::Filter(Device* dev) : 
  m_dev(dev != NULL ? dev : &Device::null) 
{}

IOStream::Filter::Filter() : 
  m_dev(&Device::null) 
{}

IOStream::IOStream(Device* dev) : 
  m_dev(dev != NULL ? dev : &Device::null), 
  m_base(dec) 
{}

IOStream::IOStream() : 
  m_dev(&Device::null), 
  m_base(dec) 
{}

IOStream::Device* 
IOStream::set_device(Device* dev) 
{ 
  Device* previous = m_dev;
  if (dev == NULL) dev = &Device::null;
  m_dev = dev;
  return (previous);
}

void 
IOStream::print(int n, Base base) 
{
  if (base != bcd) {
    if (base != dec) print_prefix(base);
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
  if (base != dec) print_prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ltoa(n, buf, base));
}

void 
IOStream::print(unsigned int n, Base base) 
{
  if (base != dec) print_prefix(base);
  char buf[sizeof(int) * CHARBITS + 1];
  print(utoa(n, buf, base));
}

void 
IOStream::print(unsigned long int n, Base base)
{
  if (base != dec) print_prefix(base);
  char buf[sizeof(long int) * CHARBITS + 1];
  print(ultoa(n, buf, base));
}

void 
IOStream::print(IOStream::Device* buffer)
{
  int c;
  while ((c = buffer->getchar()) != EOF)
    print((char) c);
}

void 
IOStream::print_prefix(Base base)
{
  if (base == hex)
    print_P(PSTR("0x"));
  else if (base == bin)
    print_P(PSTR("0b"));
  else if (base == oct)
    print_P(PSTR("0"));
}

void 
IOStream::print(const void *ptr, size_t size, Base base, uint8_t max)
{
  uint8_t* p = (uint8_t*) ptr;
  unsigned int v_adj = (base == dec ? 0 : (base == oct ? 01000 : 0x100));
  uint8_t adj = (v_adj != 0);
  uint8_t n = 0;
  print(p);
  print_P(PSTR(": "));
  while (size--) {
    char buf[sizeof(int) * CHARBITS + 1];
    unsigned int v = (*p++) + v_adj;
    print(utoa(v, buf, base) + adj);
    if (++n < max) {
      print_P(PSTR(" "));
    }
    else {
      println();
      n = 0;
      if (size > 0) {
	print(p);
	print_P(PSTR(": "));
      }
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

IOStream::Device IOStream::Device::null;

int 
IOStream::Device::available()
{
  return (0);
}

int 
IOStream::Device::room()
{
  return (0);
}

int 
IOStream::Device::putchar(char c) 
{ 
  return (EOF); 
}
    
int 
IOStream::Device::puts(const char* s) 
{ 
  return (write((void*) s, strlen(s)));
}

int 
IOStream::Device::puts_P(const char* s)
{ 
  char c;
  int n = 0;
  while ((c = pgm_read_byte(s++)) != 0)
    if (putchar(c) < 0) 
      return (EOF);
    else
      n += 1;
  return (n); 
}

int 
IOStream::Device::write(const void* buf, size_t size) 
{ 
  char* ptr = (char*) buf;
  size_t n = 0;
  for(; n < size; n++)
    if (putchar(*ptr++) < 0)
      break;
  return (n);
}

int 
IOStream::Device::write(const iovec_t* vec)
{
  size_t size = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++) {
    size_t res = (size_t) write(vp->buf, vp->size);
    if (res == 0) break;
    size += res;
  }
  return (size);
}

int 
IOStream::Device::peekchar() 
{ 
  return (EOF); 
}

int 
IOStream::Device::peekchar(char c) 
{ 
  return (EOF); 
}

int 
IOStream::Device::getchar() 
{ 
  return (EOF); 
}

char* 
IOStream::Device::gets(char *s, size_t count) 
{ 
  char* res = s;
  while (count--) {
    int c = getchar();
    if (c == EOF && m_mode != NON_BLOCKING) {
      while (c == EOF) {
	Power::sleep(m_mode);
	c = getchar();
      }
    }
    if (c == '\n' || c == IOStream::EOF) break;
    *s++ = c;
  }
  *s = 0;
  return (res);
}

int 
IOStream::Device::read(void* buf, size_t size) 
{ 
  char* ptr = (char*) buf;
  size_t n = 0; 
  for (; n < size; n++)
    if ((*ptr++ = getchar()) < 0)
      break;
  return (n);
}

int 
IOStream::Device::read(iovec_t* vec) 
{
  size_t size = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++) {
    size_t res = (size_t) read(vp->buf, vp->size);
    if (res == 0) break;
    size += res;
  }
  return (size);
}

int 
IOStream::Device::flush() 
{ 
  return (EOF); 
}
