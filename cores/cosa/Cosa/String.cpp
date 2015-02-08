/**
 * @file Cosa/String.hh
 * @version 1.0
 *
 * @section License
 * Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
 * Copyright 2011, Paul Stoffregen, paul@pjrc.com, rewriting.
 * Copyright (C) 2014-2015, Mikael Patel, refactoring.
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

#include "Cosa/String.hh"
#include <ctype.h>

String::String(const char* cstr)
{
  init();
  if (cstr != NULL) copy(cstr, strlen(cstr));
}

String::String(const String& value)
{
  init();
  *this = value;
}

String::String(str_P pstr)
{
  init();
  *this = pstr;
}

#if (ARDUINO >= 150)
String::String(String&& rval)
{
  init();
  move(rval);
}

String::String(__StringSumHelper&& rval)
{
  init();
  move(rval);
}
#endif

String::String(char c)
{
  init();
  char buf[2];
  buf[0] = c;
  buf[1] = 0;
  *this = buf;
}

String::String(unsigned char value, unsigned char base)
{
  init();
  char buf[1 + 8 * sizeof(unsigned char)];
  utoa(value, buf, base);
  *this = buf;
}

String::String(int value, unsigned char base)
{
  init();
  char buf[2 + 8 * sizeof(int)];
  itoa(value, buf, base);
  *this = buf;
}

String::String(unsigned int value, unsigned char base)
{
  init();
  char buf[1 + 8 * sizeof(unsigned int)];
  utoa(value, buf, base);
  *this = buf;
}

String::String(long value, unsigned char base)
{
  init();
  char buf[2 + 8 * sizeof(long)];
  ltoa(value, buf, base);
  *this = buf;
}

String::String(unsigned long value, unsigned char base)
{
  init();
  char buf[1 + 8 * sizeof(unsigned long)];
  ultoa(value, buf, base);
  *this = buf;
}

String::String(float value, unsigned char decimalPlaces)
{
  init();
  char buf[33];
  *this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
}

String::String(double value, unsigned char decimalPlaces)
{
  init();
  char buf[33];
  *this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
}

String::~String()
{
  if (m_buffer != NULL) free(m_buffer);
}

inline void
String::init(void)
{
  m_buffer = NULL;
  m_capacity = 0;
  m_length = 0;
}

void
String::invalidate(void)
{
  if (m_buffer != NULL) free(m_buffer);
  m_buffer = NULL;
  m_capacity = 0;
  m_length = 0;
}

unsigned char
String::reserve(unsigned int size)
{
  if ((m_buffer != NULL) && (m_capacity >= size)) return (true);
  if (!changeBuffer(size)) return (false);
  if (m_length == 0) m_buffer[0] = 0;
  return (true);
}

unsigned char
String::changeBuffer(unsigned int maxStrLen)
{
  char* newbuffer = (char*) realloc(m_buffer, maxStrLen + 1);
  if (newbuffer == NULL) return (false);
  m_buffer = newbuffer;
  m_capacity = maxStrLen;
  return (true);
}

String&
String::copy(const char* cstr, unsigned int length)
{
  if (!reserve(length)) {
    invalidate();
  }
  else {
    m_length = length;
    strcpy(m_buffer, cstr);
  }
  return (*this);
}

String&
String::copy(str_P pstr, unsigned int length)
{
  if (!reserve(length)) {
    invalidate();
  }
  else {
    m_length = length;
    strcpy_P(m_buffer, (PGM_P) pstr);
  }
  return *this;
}

void String::move(String& rhs)
{
  if (m_buffer != NULL) {
    if (m_capacity >= rhs.m_length) {
      strcpy(m_buffer, rhs.m_buffer);
      m_length = rhs.m_length;
      rhs.m_length = 0;
      return;
    } else {
      free(m_buffer);
    }
  }
  m_buffer = rhs.m_buffer;
  m_capacity = rhs.m_capacity;
  m_length = rhs.m_length;
  rhs.m_buffer = NULL;
  rhs.m_capacity = 0;
  rhs.m_length = 0;
}

String&
String::operator=(const String& rhs)
{
  if (this == &rhs) return (*this);
  if (rhs.m_buffer != NULL)
    copy(rhs.m_buffer, rhs.m_length);
  else
    invalidate();
  return (*this);
}

#if (ARDUINO >= 150)
String&
String::operator=(String&& rval)
{
  if (this != &rval) move(rval);
  return (*this);
}

String&
String::operator=(__StringSumHelper&& rval)
{
  if (this != &rval) move(rval);
  return (*this);
}
#endif

String&
String::operator=(const char* cstr)
{
  if (cstr != NULL) copy(cstr, strlen(cstr));
  else invalidate();
  return (*this);
}

String&
String::operator=(str_P pstr)
{
  if (pstr != NULL) copy(pstr, strlen_P((PGM_P) pstr));
  else invalidate();
  return (*this);
}

unsigned char
String::concat(const String& s)
{
  return (concat(s.m_buffer, s.m_length));
}

unsigned char
String::concat(const char* cstr, unsigned int length)
{
  unsigned int newlen = m_length + length;
  if (cstr == NULL) return (false);
  if (length == 0) return (true);
  if (!reserve(newlen)) return (false);
  strcpy(m_buffer + m_length, cstr);
  m_length = newlen;
  return (true);
}

unsigned char
String::concat(const char* cstr)
{
  if (cstr == NULL) return (false);
  return (concat(cstr, strlen(cstr)));
}

unsigned char
String::concat(char c)
{
  char buf[2];
  buf[0] = c;
  buf[1] = 0;
  return (concat(buf, 1));
}

unsigned char
String::concat(unsigned char num)
{
  char buf[1 + 3 * sizeof(unsigned char)];
  itoa(num, buf, 10);
  return (concat(buf, strlen(buf)));
}

unsigned char
String::concat(int num)
{
  char buf[2 + 3 * sizeof(int)];
  itoa(num, buf, 10);
  return (concat(buf, strlen(buf)));
}

unsigned char
String::concat(unsigned int num)
{
  char buf[1 + 3 * sizeof(unsigned int)];
  utoa(num, buf, 10);
  return (concat(buf, strlen(buf)));
}

unsigned char
String::concat(long num)
{
  char buf[2 + 3 * sizeof(long)];
  ltoa(num, buf, 10);
  return (concat(buf, strlen(buf)));
}

unsigned char
String::concat(unsigned long num)
{
  char buf[1 + 3 * sizeof(unsigned long)];
  ultoa(num, buf, 10);
  return (concat(buf, strlen(buf)));
}

unsigned char
String::concat(float num)
{
  char buf[20];
  char* string = dtostrf(num, 4, 2, buf);
  return (concat(string, strlen(string)));
}

unsigned char
String::concat(double num)
{
  char buf[20];
  char* string = dtostrf(num, 4, 2, buf);
  return (concat(string, strlen(string)));
}

unsigned char
String::concat(str_P str)
{
  if (str == NULL) return (false);
  int length = strlen_P((const char*) str);
  if (length == 0) return (true);
  unsigned int newlen = m_length + length;
  if (!reserve(newlen)) return (false);
  strcpy_P(m_buffer + m_length, (const char*) str);
  m_length = newlen;
  return (true);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, const String& rhs)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(rhs.m_buffer, rhs.m_length)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, const char* cstr)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if ((cstr == NULL) || !res.concat(cstr, strlen(cstr))) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, char c)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(c)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, unsigned char num)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(num)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, int num)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(num)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, unsigned int num)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(num)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, long num)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(num)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, unsigned long num)
{
  __StringSumHelper& a = const_cast<__StringSumHelper&>(lhs);
  if (!a.concat(num)) a.invalidate();
  return a;
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, float num)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(num)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, double num)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(num)) res.invalidate();
  return (res);
}

__StringSumHelper&
operator+(const __StringSumHelper& lhs, str_P rhs)
{
  __StringSumHelper& res = const_cast<__StringSumHelper&>(lhs);
  if (!res.concat(rhs)) res.invalidate();
  return (res);
}

int
String::compareTo(const String& s) const
{
  if ((m_buffer == NULL) || (s.m_buffer == NULL)) {
    if ((s.m_buffer != NULL) && (s.m_length > 0))
      return (0 - *(unsigned char*) s.m_buffer);
    if ((m_buffer != NULL) && (m_length > 0))
      return (*(unsigned char*) m_buffer);
    return (0);
  }
  return (strcmp(m_buffer, s.m_buffer));
}

unsigned char
String::equals(const String& s2) const
{
  return ((m_length == s2.m_length) && (compareTo(s2) == 0));
}

unsigned char
String::equals(const char* cstr) const
{
  if (m_length == 0)
    return ((cstr == NULL) || (*cstr == 0));
  if (cstr == NULL)
    return ((m_buffer == NULL) || (m_buffer[0] == 0));
  return (strcmp(m_buffer, cstr) == 0);
}

unsigned char
String::operator<(const String& rhs) const
{
  return (compareTo(rhs) < 0);
}

unsigned char
String::operator>(const String& rhs) const
{
  return (compareTo(rhs) > 0);
}

unsigned char
String::operator<=(const String& rhs) const
{
  return (compareTo(rhs) <= 0);
}

unsigned char
String::operator>=(const String& rhs) const
{
  return (compareTo(rhs) >= 0);
}

unsigned char
String::equalsIgnoreCase( const String& s2 ) const
{
  if (this == &s2) return (true);
  if (m_length != s2.m_length) return (false);
  if (m_length == 0) return (true);
  const char* p1 = m_buffer;
  const char* p2 = s2.m_buffer;
  while (*p1 != 0) {
    if (tolower(*p1++) != tolower(*p2++)) return (false);
  }
  return (true);
}

unsigned char
String::startsWith( const String& s2 ) const
{
  if (m_length < s2.m_length) return (false);
  return (startsWith(s2, 0));
}

unsigned char
String::startsWith( const String& s2, unsigned int offset ) const
{
  if ((offset > (m_length - s2.m_length))
      || (m_buffer == NULL)
      || (s2.m_buffer == NULL)) return (false);
  return (strncmp( &m_buffer[offset], s2.m_buffer, s2.m_length) == 0);
}

unsigned char
String::endsWith( const String& s2 ) const
{
  if ((m_length < s2.m_length)
      || (m_buffer == NULL)
      || (s2.m_buffer == NULL)) return (false);
  return (strcmp(&m_buffer[m_length - s2.m_length], s2.m_buffer) == 0);
}

char
String::charAt(unsigned int loc) const
{
  return (operator[](loc));
}

void
String::setCharAt(unsigned int loc, char c)
{
  if (loc < m_length) m_buffer[loc] = c;
}

char&
String::operator[](unsigned int index)
{
  static char dummy_writable_char;
  if ((index >= m_length) || (m_buffer == NULL)) {
    dummy_writable_char = 0;
    return (dummy_writable_char);
  }
  return (m_buffer[index]);
}

char
String::operator[]( unsigned int index ) const
{
  if ((index >= m_length) || (m_buffer == NULL)) return (0);
  return (m_buffer[index]);
}

void
String::getBytes(unsigned char* buf, unsigned int bufsize, unsigned int index) const
{
  if ((bufsize == 0) || (buf == NULL)) return;
  if (index >= m_length) {
    buf[0] = 0;
    return;
  }
  unsigned int n = bufsize - 1;
  if (n > (m_length - index)) n = m_length - index;
  strncpy((char*) buf, m_buffer + index, n);
  buf[n] = 0;
}

int
String::indexOf(char c) const
{
  return (indexOf(c, 0));
}

int
String::indexOf(char ch, unsigned int fromIndex) const
{
  if (fromIndex >= m_length) return -1;
  const char* temp = strchr(m_buffer + fromIndex, ch);
  if (temp == NULL) return (-1);
  return (temp - m_buffer);
}

int
String::indexOf(const String& s2) const
{
  return (indexOf(s2, 0));
}

int
String::indexOf(const String& s2, unsigned int fromIndex) const
{
  if (fromIndex >= m_length) return (-1);
  const char* found = strstr(m_buffer + fromIndex, s2.m_buffer);
  if (found == NULL) return (-1);
  return (found - m_buffer);
}

int
String::lastIndexOf(char theChar) const
{
  return (lastIndexOf(theChar, m_length - 1));
}

int
String::lastIndexOf(char ch, unsigned int fromIndex) const
{
  if (fromIndex >= m_length) return (-1);
  char tempchar = m_buffer[fromIndex + 1];
  m_buffer[fromIndex + 1] = '\0';
  char* temp = strrchr( m_buffer, ch );
  m_buffer[fromIndex + 1] = tempchar;
  if (temp == NULL) return (-1);
  return (temp - m_buffer);
}

int
String::lastIndexOf(const String& s2) const
{
  return (lastIndexOf(s2, m_length - s2.m_length));
}

int
String::lastIndexOf(const String& s2, unsigned int fromIndex) const
{
  if ((s2.m_length == 0)
      || (m_length == 0)
      || (s2.m_length > m_length)) return (-1);
  if (fromIndex >= m_length) fromIndex = m_length - 1;
  int found = -1;
  for (char* p = m_buffer; p <= m_buffer + fromIndex; p++) {
    p = strstr(p, s2.m_buffer);
    if (p == NULL) break;
    if ((unsigned int)(p - m_buffer) <= fromIndex) found = p - m_buffer;
  }
  return (found);
}

String
String::substring(unsigned int left, unsigned int right) const
{
  if (left > right) {
    unsigned int temp = right;
    right = left;
    left = temp;
  }
  String res;
  if (left > m_length) return (res);
  if (right > m_length) right = m_length;
  char temp = m_buffer[right];
  m_buffer[right] = '\0';
  res = m_buffer + left;
  m_buffer[right] = temp;
  return (res);
}

void
String::replace(char find, char replace)
{
  if (m_buffer == NULL) return;
  for (char* p = m_buffer; *p != 0; p++) {
    if (*p == find) *p = replace;
  }
}

void
String::replace(const String& find, const String& replace)
{
  if ((m_length == 0) || (find.m_length == 0)) return;
  int diff = replace.m_length - find.m_length;
  char* readFrom = m_buffer;
  char* foundAt;
  if (diff == 0) {
    while ((foundAt = strstr(readFrom, find.m_buffer)) != NULL) {
      memcpy(foundAt, replace.m_buffer, replace.m_length);
      readFrom = foundAt + replace.m_length;
    }
  } else if (diff < 0) {
    char* writeTo = m_buffer;
    while ((foundAt = strstr(readFrom, find.m_buffer)) != NULL) {
      unsigned int n = foundAt - readFrom;
      memcpy(writeTo, readFrom, n);
      writeTo += n;
      memcpy(writeTo, replace.m_buffer, replace.m_length);
      writeTo += replace.m_length;
      readFrom = foundAt + find.m_length;
      m_length += diff;
    }
    strcpy(writeTo, readFrom);
  } else {
    unsigned int size = m_length;
    while ((foundAt = strstr(readFrom, find.m_buffer)) != NULL) {
      readFrom = foundAt + find.m_length;
      size += diff;
    }
    if (size == m_length) return;
    if ((size > m_capacity) && !changeBuffer(size)) return;
    int index = m_length - 1;
    while ((index >= 0) && ((index = lastIndexOf(find, index)) >= 0)) {
      readFrom = m_buffer + index + find.m_length;
      memmove(readFrom + diff, readFrom, m_length - (readFrom - m_buffer));
      m_length += diff;
      m_buffer[m_length] = 0;
      memcpy(m_buffer + index, replace.m_buffer, replace.m_length);
      index--;
    }
  }
}

void
String::remove(unsigned int index){
  if (index >= m_length) return;
  int count = m_length - index;
  remove(index, count);
}

void
String::remove(unsigned int index, unsigned int count)
{
  if (index >= m_length) return;
  if (count == 0) return;
  if (index + count > m_length) { count = m_length - index; }
  char* writeTo = m_buffer + index;
  m_length = m_length - count;
  strncpy(writeTo, m_buffer + index + count,m_length - index);
  m_buffer[m_length] = 0;
}

void
String::toLowerCase(void)
{
  if (m_buffer == NULL) return;
  for (char* p = m_buffer; *p; p++) {
    *p = tolower(*p);
  }
}

void
String::toUpperCase(void)
{
  if (m_buffer == NULL) return;
  for (char* p = m_buffer; *p; p++) {
    *p = toupper(*p);
  }
}

void
String::trim(void)
{
  if ((m_buffer == NULL) || (m_length == 0)) return;
  char* begin = m_buffer;
  while (isspace(*begin)) begin++;
  char* end = m_buffer + m_length - 1;
  while (isspace(*end) && end >= begin) end--;
  m_length = end + 1 - begin;
  if (begin > m_buffer) memcpy(m_buffer, begin, m_length);
  m_buffer[m_length] = 0;
}

long
String::toInt(void) const
{
  if (m_buffer != NULL) return (atol(m_buffer));
  return (0);
}

float
String::toFloat(void) const
{
  if (m_buffer != NULL) return (float(atof(m_buffer)));
  return (0.0);
}
