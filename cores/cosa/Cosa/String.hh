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

#ifndef COSA_STRING_HH
#define COSA_STRING_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * String add operator handler.
 */
class __StringSumHelper;

/**
 * The String class; dynamic, resizable strings.
 */
class String {

  /**
   * Use a function pointer to allow for "if (s)" without the
   * complications of an operator bool(). for more information, see:
   * http://www.artima.com/cppsource/safebool.html
   */
  typedef void (String::*StringIfHelperType)() const;
  void StringIfHelper() const {}

public:
  /**
   * Creates a copy of the initial value. If the initial value is null
   * or invalid, or if memory allocation fails, the string will be
   * marked as invalid (i.e. "if (s)" will be false).
   */
  String(const char *cstr = "");
  String(const String &str);
  String(str_P str);
#if (ARDUINO >= 150)
  String(String &&rval);
  String(__StringSumHelper &&rval);
#endif

  explicit String(char c);
  explicit String(unsigned char, unsigned char base=10);
  explicit String(int, unsigned char base=10);
  explicit String(unsigned int, unsigned char base=10);
  explicit String(long, unsigned char base=10);
  explicit String(unsigned long, unsigned char base=10);
  explicit String(float, unsigned char decimalPlaces=2);
  explicit String(double, unsigned char decimalPlaces=2);

  ~String(void);

  /**
   * Memory Management: Return true on success, false on failure (in
   * which case, the string is left unchanged).  reserve(0), if
   * successful, will validate an invalid string (i.e., "if (s)" will
   * be true afterwards)
   */
  unsigned char reserve(unsigned int size);

  /**
   * Return string length.
   */
  inline unsigned int length(void) const
  {
    return (m_length);
  }

  /**
   * Creates a copy of the assigned value.  if the value is null or
   * invalid, or if the memory allocation fails, the string will be
   * marked as invalid ("if (s)" will be false).
   */
  String& operator=(const String &rhs);
  String& operator=(const char *cstr);
  String& operator=(str_P str);

#if (ARDUINO >= 150)
  String& operator=(String &&rval);
  String& operator=(__StringSumHelper &&rval);
#endif

  /**
   * Concatenate value. Returns true on success, false on failure (in
   * which case, the string is left unchanged). If the argument is
   * null or invalid, the concatenation is considered unsucessful.
   */
  unsigned char concat(const String &str);
  unsigned char concat(const char *cstr);
  unsigned char concat(char c);
  unsigned char concat(unsigned char c);
  unsigned char concat(int num);
  unsigned char concat(unsigned int num);
  unsigned char concat(long num);
  unsigned char concat(unsigned long num);
  unsigned char concat(float num);
  unsigned char concat(double num);
  unsigned char concat(str_P str);

  /**
   * If there's not enough memory for the concatenated value, the
   * string will be left unchanged (but this isn't signalled in any
   * way)
   */
  String& operator+=(const String &rhs) { concat(rhs); return (*this); }
  String& operator+=(const char *cstr)	{ concat(cstr); return (*this); }
  String& operator+=(char c) { concat(c); return (*this); }
  String& operator+=(unsigned char num) { concat(num); return (*this); }
  String& operator+=(int num) { concat(num); return (*this); }
  String& operator+=(unsigned int num) {concat(num); return (*this); }
  String& operator+=(long num)	{ concat(num); return (*this); }
  String& operator+=(unsigned long num) { concat(num); return (*this); }
  String& operator+=(float num) { concat(num); return (*this); }
  String& operator+=(double num) { concat(num); return (*this); }
  String& operator+=(str_P str) { concat(str); return (*this);}

  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, const String &rhs);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, const char *cstr);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, char c);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, unsigned char num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, int num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, unsigned int num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, long num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, unsigned long num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, float num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, double num);
  friend __StringSumHelper& operator+(const __StringSumHelper &lhs, str_P rhs);

  /**
   * Comparison (only works w/ Strings and "strings").
   */
  operator StringIfHelperType() const
  {
    return m_buffer ? &String::StringIfHelper : 0;
  }

  int compareTo(const String &s) const;
  unsigned char equals(const String &s) const;
  unsigned char equals(const char *cstr) const;
  unsigned char operator==(const String &rhs) const { return equals(rhs); }
  unsigned char operator==(const char *cstr) const { return equals(cstr); }
  unsigned char operator!=(const String &rhs) const { return !equals(rhs); }
  unsigned char operator!=(const char *cstr) const { return !equals(cstr); }
  unsigned char operator<(const String &rhs) const;
  unsigned char operator>(const String &rhs) const;
  unsigned char operator<=(const String &rhs) const;
  unsigned char operator>=(const String &rhs) const;
  unsigned char equalsIgnoreCase(const String &s) const;
  unsigned char startsWith( const String &prefix) const;
  unsigned char startsWith(const String &prefix, unsigned int offset) const;
  unsigned char endsWith(const String &suffix) const;

  /**
   * Character acccess.
   */
  char charAt(unsigned int index) const;
  void setCharAt(unsigned int index, char c);
  char operator[](unsigned int index) const;
  char& operator[](unsigned int index);
  void getBytes(unsigned char* buf, unsigned int bufsize, unsigned int index = 0) const;

  void toCharArray(char* buf, unsigned int bufsize, unsigned int index = 0) const
  {
    getBytes((unsigned char*) buf, bufsize, index);
  }

  const char* c_str() const { return (m_buffer); }

  /**
   * Search.
   */
  int indexOf(char ch) const;
  int indexOf(char ch, unsigned int fromIndex) const;
  int indexOf(const String &str) const;
  int indexOf(const String &str, unsigned int fromIndex) const;
  int lastIndexOf(char ch) const;
  int lastIndexOf(char ch, unsigned int fromIndex) const;
  int lastIndexOf(const String &str) const;
  int lastIndexOf(const String &str, unsigned int fromIndex) const;

  /**
   * Sub-string.
   */
  String substring(unsigned int beginIndex, unsigned int endIndex) const;
  String substring(unsigned int beginIndex) const
  {
    return substring(beginIndex, m_length);
  };

  /**
   * Modification.
   */
  void replace(char find, char replace);
  void replace(const String& find, const String& replace);
  void remove(unsigned int index);
  void remove(unsigned int index, unsigned int count);
  void toLowerCase(void);
  void toUpperCase(void);
  void trim(void);

  /**
   * Parsing/Conversion.
   */
  long toInt(void) const;
  float toFloat(void) const;

protected:
  char* m_buffer;	    //!< the actual char array
  unsigned int m_capacity;  //!< the array length minus one (for the '\0')
  unsigned int m_length;    //!< the String length (not counting the '\0')

  void init(void);
  void invalidate(void);
  unsigned char changeBuffer(unsigned int maxStrLen);
  unsigned char concat(const char *cstr, unsigned int length);

  /**
   * Copy and move.
   */
  String& copy(const char *cstr, unsigned int length);
  String& copy(str_P pstr, unsigned int length);
  void move(String &rhs);
};

/**
 * An inherited String class for holding the result of a
 * concatenation. These result objects are assumed to be writable by
 * subsequent concatenations.
 */
class __StringSumHelper : public String
{
public:
  __StringSumHelper(const String& s) : String(s) {}
  __StringSumHelper(const char* p) : String(p) {}
  __StringSumHelper(char c) : String(c) {}
  __StringSumHelper(unsigned char num) : String(num) {}
  __StringSumHelper(int num) : String(num) {}
  __StringSumHelper(unsigned int num) : String(num) {}
  __StringSumHelper(long num) : String(num) {}
  __StringSumHelper(unsigned long num) : String(num) {}
  __StringSumHelper(float num) : String(num) {}
  __StringSumHelper(double num) : String(num) {}
};

/**
 * Print String to given stream.
 * @param[in] s String to print.
 * @return iostream.
 */
inline IOStream& operator<<(IOStream& outs, String& s)
{
  outs.print((char*) s.c_str());
  return (outs);
}
#endif
