/**
 * @file
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
 * Common data types and syntax abstractions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TYPES_H__
#define __COSA_TYPES_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Serial.h"

/**
 * Macro for number of elements in a vector.
 * @param[in] x vector
 * @return number of elements
 */
#define membersof(x) (sizeof(x)/sizeof(x[0]))

/**
 * Macro for offset to member in structure.
 * @param[in] type data type name.
 * @param[in] member name.
 * @return offset.
 */
#define offsetof(type, member) __builtin_offsetof(type, member)

/**
 * Disable interrupts and return flags.
 * @return processor flags.
 */
inline uint8_t lock() 
{ 
  uint8_t key = SREG;
  cli();
  return (key);
}

/**
 * Restore processor flags and possible enable of interrupts.
 * @param[in] key processor flags.
 */
inline void unlock(uint8_t key)
{
  SREG = key;
}

/**
 * Syntax abstraction for synchronized block. Used in the form:
 * synchronized {
 *   ...
 * }
 * Interrupts are disabled in the block allowing secure update.
 */
#define synchronized for (uint8_t key = lock(), i = 1; i != 0; i--, unlock(key))

/**
 * Force compiler to store all values in memory at this point.
 * Alternative to volative declaration.
 */
#define barrier() __asm__ __volatile__("nop" ::: "memory") 

#endif

