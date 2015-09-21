/**
 * @file Cosa/IOStream_Device.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "Cosa/IOStream.hh"

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
  int res = write(&c, sizeof(c));
  return (res == sizeof(c) ? c & 0xff : res);
}

int
IOStream::Device::puts(const char* s)
{
  const char* bp = (const char*) s;
  int n = 0;
  for (char c; (c = *bp++) != 0; n++)
    if (UNLIKELY(putchar(c) < 0))
      break;
  return (n);
}

int
IOStream::Device::puts(str_P s)
{
  const char* bp = (const char*) s;
  int n = 0;
  for (char c; (c = pgm_read_byte(bp++)) != 0; n++)
    if (UNLIKELY(putchar(c) < 0))
      break;
  return (n);
}

int
IOStream::Device::write(const void* buf, size_t size)
{
  char* bp = (char*) buf;
  size_t n = 0;
  for(; n < size; n++)
    if (UNLIKELY(putchar(*bp++) < 0))
      break;
  return (n);
}

int
IOStream::Device::write_P(const void* buf, size_t size)
{
  char* bp = (char*) buf;
  size_t n = 0;
  for(; n < size; n++)
    if (UNLIKELY(putchar(pgm_read_byte(bp++)) < 0))
      break;
  return (n);
}

int
IOStream::Device::write(const iovec_t* vec)
{
  size_t size = 0;
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++) {
    size_t res = (size_t) write(vp->buf, vp->size);
    if (UNLIKELY(res == 0)) break;
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
  UNUSED(c);
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
  while (--count) {
    int c = getchar();
    if (UNLIKELY(c == EOF)) {
      if (m_blocking) {
	while (c == EOF) {
	  yield();
	  c = getchar();
	}
      }
      else break;
    }
    if (c == '\r') {
      if (m_eol == CRLF_MODE) continue;
      c = '\n';
    }
    *s++ = c;
    if (c == '\n') break;
  }
  *s = 0;
  return (s == res ? NULL : res);
}

int
IOStream::Device::read(void* buf, size_t size)
{
  char* bp = (char*) buf;
  size_t n = 0;
  for (; n < size; n++) {
    int c = getchar();
    if (UNLIKELY(c < 0)) break;
    *bp++ = c;
  }
  return (n);
}

int
IOStream::Device::read(iovec_t* vec)
{
  size_t size = 0;
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++) {
    size_t res = (size_t) read(vp->buf, vp->size);
    if (UNLIKELY(res == 0)) break;
    size += res;
  }
  return (size);
}

int
IOStream::Device::flush()
{
  return (EOF);
}

void
IOStream::Device::empty()
{
}
