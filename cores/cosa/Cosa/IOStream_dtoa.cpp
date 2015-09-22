/**
 * @file IOStream_dtoa.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2011, Dmitry E. Oboukhov
 *
 * A high performance implementation of standard C functions for
 * conversion from signed and unsigned numbers to character string.
 *
 * @section References
 * 1. https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=627899
 */

#include "Cosa/IOStream.hh"

#if !defined(COSA_IOSTREAM_STDLIB)
#include <avr/pgmspace.h>

static const unsigned long digits8[] PROGMEM = {
  1073741824,
  134217728,
  16777216,
  2097152,
  262144,
  32768,
  4096,
  512,
  64,
  8,
  1,
};

static const unsigned long digits10[] PROGMEM = {
  1000000000,
  100000000,
  10000000,
  1000000,
  100000,
  10000,
  1000,
  100,
  10,
  1,
};

static const unsigned long digits16[] PROGMEM = {
  268435456,
  16777216,
  1048576,
  65536,
  4096,
  256,
  16,
  1,
};

static const unsigned long digits32[] PROGMEM = {
  1073741824,
  33554432,
  1048576,
  32768,
  1024,
  32,
  1,
};

static const unsigned long digits36[] PROGMEM = {
  2176782336,
  60466176,
  1679616,
  46656,
  1296,
  36,
  1,
};

static const char letters[] PROGMEM = "0123456789abcdefghijklmnopqrstuvwxyz";

char*
IOStream::ultoa(unsigned long __val, char *__s, int base)
{
  unsigned char max;
  const unsigned long *d;
  unsigned char i, j, k, first = 1;

  switch(base) {
  case 8:
    max = sizeof(digits8) / sizeof(unsigned long);
    d = digits8;
    break;

  case 16:
    max = sizeof(digits16) / sizeof(unsigned long);
    d = digits16;
    break;

  case 32:
    max = sizeof(digits32) / sizeof(unsigned long);
    d = digits32;
    break;

  case 36:
    max = sizeof(digits36) / sizeof(unsigned long);
    d = digits36;
    break;

  default:
    max = sizeof(digits10) / sizeof(unsigned long);
    d = digits10;
    break;
  }

  for (i = j = 0; i < max; i++) {
    unsigned long check = pgm_read_dword(d + i);
    if (check > __val) {
      if (first)
	continue;
      __s[j++] = '0';
      continue;
    }
    first = k = 0;
    while(check <= __val) {
      __val -= check;
      k++;
    }

    __s[j++] = pgm_read_byte(letters + k);
  }

  if (first)
    __s[j++] = '0';
  __s[j] = 0;

  return (__s);
}

char*
IOStream::ltoa(long __val, char *__s, int base)
{
  if (__val >= 0)
    return (ultoa((unsigned long)__val, __s, base));
  __s[0] = '-';
  return (ultoa((unsigned long)(-__val), __s + 1, base) - 1);
}
#endif
