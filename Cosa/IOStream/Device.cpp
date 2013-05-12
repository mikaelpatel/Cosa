/**
 * @file Cosa/IOStream/Device.cpp
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
  return (-1); 
}
    
int 
IOStream::Device::puts(char* s) 
{ 
  char c;
  while ((c = *s++) != 0) 
    if (putchar(c) < 0) return (-1);
  return (0); 
}

int 
IOStream::Device::puts_P(const char* s)
{ 
  char c;
  while ((c = pgm_read_byte(s++)) != 0)
    if (putchar(c) < 0) return (-1);
  return (0); 
}

int 
IOStream::Device::write(void* buf, size_t size) 
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
  return (-1); 
}

int 
IOStream::Device::getchar() 
{ 
  return (-1); 
}

char* 
IOStream::Device::gets(char *s, size_t count) 
{ 
  char* res = s;
  while (count--) {
    int c = getchar();
    if (c < 0) {
      *s = 0;
      return (0);
    }
    if (c == '\n') {
      *s = 0;
      return (res);
    }
    *s++ = c;
  }
  *--s = 0;
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
IOStream::Device::flush(uint8_t mode) 
{ 
  return (-1); 
}
