/**
 * @file Cosa/IOStream.h
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
 * Stream::Device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_H__
#define __COSA_IOSTREAM_H__

#include "Cosa/Types.h"
#include <stdarg.h>

class IOStream {

public:
  /**
   * Device for output/input of character or string. Null device.
   */
  class Device {
  public:
    virtual int putchar(char c) { return (0); }
    virtual int puts(char* s) { return (0); }
    virtual int puts_P(const char* s) { return (0); }
    virtual int getchar() { return (-1); }
    virtual char* gets(char *s) { *s = 0; return (s); }
    virtual int sync() { return (0); }
    static Device null;
  };

  /**
   * Construct stream with given device.
   * @param[in] dev stream device.
   */
  IOStream(Device* dev = &Device::null) : _dev(dev) {}
  
  /**
   * Get current device.
   * @return device
   */
  Device* get_device() 
  { 
    return (_dev); 
  }

  /**
   * Set io stream device.
   * @param[in] dev stream device.
   */
  void set(Device* dev) 
  { 
    _dev = dev;
  }

  /**
   * Print integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(int value, uint8_t base = 10);

  /**
   * Print long integer 32-bit value in given base torace stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(long int value, uint8_t base = 10);

  /**
   * Print unsigned integer as string with given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned int value, uint8_t base = 10);

  /**
   * Print unsigned long integer 32-bit value in given base to stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  void print(unsigned long int value, uint8_t base = 10);

  /**
   * Print pointer as a hexidecimal number to stream.
   * @param[in] ptr pointer to data memory.
   */
  void print(void *ptr) { print((unsigned int) ptr, 16); }

  /**
   * Print pointer to program memory as a hexidecimal number to 
   * stream.
   * @param[in] ptr pointer to program memory.
   */
  void print(const void *ptr) { print((unsigned int) ptr, 16); }

  /**
   * Print character to stream.
   * @param[in] c character to print.
   */
  void print(char c) { _dev->putchar(c); }

  /**
   * Print string in data memory to stream.
   * @param[in] ptr pointer to data memory string.
   */
  void print(char* s) { _dev->puts(s); }

  /**
   * Print string in program memory to stream.
   * Use macro PSTR() to generate a string constants in 
   * program memory.
   * @param[in] ptr pointer to program memory string.
   */
  void print_P(const char* s) { _dev->puts_P(s); }

  /**
   * Print end of line to stream.
   */
  void println() { _dev->putchar('\n'); }

  /**
   * Formated print with variable argument list.
   * @param[in] format string.
   * @param[in] args variable argument list.
   */
  void vprintf(const char* format, va_list args);

  /**
   * Formated print with variable argument list.
   * @param[in] format string.
   * @param[in] ... variable argument list.
   */
  void printf(const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }

 private:
  Device* _dev;

  /**
   * Print number prefix for non decimal base.
   * @param[in] base representation.
   */
  void print_prefix(uint8_t base);
};

#endif
