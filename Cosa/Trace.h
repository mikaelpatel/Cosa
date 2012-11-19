/**
 * @file Cosa/Trace.h
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
 * Basic trace support class. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TRACE_H__
#define __COSA_TRACE_H__

#include "Cosa/Types.h"

class Trace {

public:
  /**
   * Start trace stream over uart transmitter.
   * @param[in] baudrate serial bitrate.
   * @return true(1) if successful otherwise false(0)
   */
  static bool begin(uint32_t baudrate = 9600);

  /**
   * Stop trace stream over uart transmitter.
   * @return true(1) if successful otherwise false(0)
   */
  static bool end();

  /**
   * Print integer value in given base to trace stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  static void print(int value, uint8_t base = 10);

  /**
   * Print long integer 32-bit value in given base to trace stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  static void print(long int value, uint8_t base = 10);

  /**
   * Print unsigned 8-bit integer value in given base to trace 
   * stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  static void print(uint8_t value, uint8_t base = 10)
  {
    print((int) value, base);
  }

  /**
   * Print unsigned 16-bit integer value in given base to trace 
   * stream.
   * @param[in] value to print.
   * @param[in] base to represent value in (default 10).
   */
  static void print(uint16_t value, uint8_t base = 10)
  {
    print((long int) value, base);
  }
    
  /**
   * Print pointer as a hexidecimal number to trace stream.
   * @param[in] ptr pointer to data memory.
   */
  static void print(void *ptr)
  { 
    print((int16_t) ptr, 16);
  }

  /**
   * Print pointer to program memory as a hexidecimal number to 
   * trace stream.
   * @param[in] ptr pointer to program memory.
   */
  static void print(const void *ptr)
  { 
    print((int16_t) ptr, 16);
  }

  /**
   * Print character to trace stream.
   * @param[in] c character to print.
   */
  static void print(char c);
  
  /**
   * Print string in data memory to trace stream.
   * @param[in] ptr pointer to data memory string.
   */
  static void print(char* s)
  {
    char c; 
    while ((c = *s++) != 0) 
      print(c);
  }

  /**
   * Print string in program memory to trace stream.
   * Use macro PSTR() to generate a string constants in 
   * program memory.
   * @param[in] ptr pointer to program memory string.
   */
  static void print_P(const char* s)
  {
    char c; 
    while ((c = pgm_read_byte(s++)) != 0)
      print(c);
  }

  /**
   * Print end of line to trace stream.
   */
  static void println()
  {
    print_P(PSTR("\n"));
  }
};

#ifndef NDEBUG
# define TRACE(expr)				\
  do {						\
    Trace::print_P(PSTR(#expr " = "));		\
    Trace::print(expr);				\
    Trace::println();				\
  } while (0)
#endif

#endif
