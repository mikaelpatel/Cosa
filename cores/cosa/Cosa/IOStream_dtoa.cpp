/**
 * @file IOStream_dtoa.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2011, Dmitry E. Oboukhov
 * Copyright (C) 2015, Mikael Patel (C++ port and bin/hex optimization).
 *
 * A high performance implementation of standard C functions for
 * conversion from signed and unsigned numbers to character string.
 *
 * @section References
 * 1. https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=627899
 */

#include "Cosa/IOStream.hh"

#if !defined(COSA_IOSTREAM_STDLIB_DTOA)
#include <avr/pgmspace.h>

static const unsigned long digits8[] __PROGMEM = {
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

static const unsigned long digits10[] __PROGMEM = {
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

static const char letters[] __PROGMEM = "0123456789abcdef";

char*
IOStream::ultoa(unsigned long __val, char *__s, int base)
{
  unsigned char i, j, k, l, first;
  const uint8_t* p;
  first = 1;
  j = 0;

  if (__val != 0UL) {
    if (base == 2) {
      // Optimize for base(2)
      first = 0;
      p = ((uint8_t*) &__val) + sizeof(__val) - 1;
      for (i = 0; i < 4 && *p == 0; i++, p--)
	;
      k = *p--;
      for (l = 0; l < 8; l++) {
	if (k & 0x80) break;
	k <<= 1;
      }
      for (; i < 4; i++, l = 0, k = *p--) {
	for (; l < 8; l++) {
	  __s[j++] = (k & 0x80) ? '1' : '0';
	  k <<= 1;
	}
      }
    }
    else if (base == 16) {
      // Optimize for base(16)
      first = 0;
      p = ((uint8_t*) &__val) + sizeof(__val) - 1;
      for (i = 0; i < 4 && *p == 0; i++, p--)
	;
      k = *p--;
      if (k & 0xf0) __s[j++] = pgm_read_byte(letters + (k >> 4));
      __s[j++] = pgm_read_byte(letters + (k & 0xf));
      for (i++; i < 4; i++) {
	k = *p--;
	__s[j++] = pgm_read_byte(letters + (k >> 4));
	__s[j++] = pgm_read_byte(letters + (k & 0xf));
      }
    }
    else {
      const unsigned long *d;
      unsigned char max;
      if (base == 8) {
	max = sizeof(digits8) / sizeof(unsigned long);
	d = digits8;
      }
      else {
	max = sizeof(digits10) / sizeof(unsigned long);
	d = digits10;
      }
      for (i = 0; i < max; i++) {
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
    }
  }

  if (first)
    __s[j++] = '0';
  __s[j] = 0;

  return (__s);
}

char*
IOStream::ltoa(long __val, char *__s, int base)
{
  if (__val >= 0 || base != 10)
    return (ultoa((unsigned long)__val, __s, base));
  __s[0] = '-';
  return (ultoa((unsigned long)(-__val), __s + 1, base) - 1);
}

char*
IOStream::utoa(unsigned int __val, char *__s, int base)
{
  return (ultoa(((unsigned long) __val) & 0xffff, __s, base));
}

char*
IOStream::itoa(int __val, char *__s, int base)
{
  return (ltoa((long)__val, __s, base));
}

#endif
