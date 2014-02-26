/**
 * @file Cosa/Types.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * Common literals, data types and syntax abstractions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TYPES_H__
#define __COSA_TYPES_H__

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <util/delay_basic.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Cosa/Errno.h"
#include "Cosa/Board.hh"

/**
 * Number of bits in a character.
 */
#define CHARBITS 8

/**
 * Standard integer range
 */
#ifndef INT8_MAX
# define INT8_MIN 0x80
# define INT8_MAX 0x7f
#endif
#ifndef INT16_MAX
# define INT16_MIN 0x8000
# define INT16_MAX 0x7fff
#endif
#ifndef INT32_MAX
# define INT32_MIN 0x80000000L
# define INT32_MAX 0x7fffffffL
#endif
#ifndef INT_MAX
# define INT_MIN INT16_MIN
# define INT_MAX INT16_MAX
#endif

/**
 * Standard null pointer
 */
#ifndef NULL
# define NULL ((void*) 0)
#endif

/**
 * Standard floating point number, 32-bit.
 */
typedef float float32_t;

/**
 * Universal type union, 16-bit.
 */
union univ16_t {
  uint16_t as_uint16;
  int16_t as_int16;
  uint8_t as_uint8[2];
  int8_t as_int8[2];
  const void* as_ptr_P;
  void* as_ptr;
  struct {
    uint8_t low;
    uint8_t high;
  };
};
typedef univ16_t univ_t;

/**
 * Universal type union, 32-bit.
 */
union univ32_t {
  float32_t as_float32;
  uint32_t as_uint32;
  int32_t as_int32;
  univ16_t as_univ16[2];
  uint16_t as_uint16[2];
  int16_t as_int16[2];
  uint8_t as_uint8[4];
  int8_t as_int8[4];
  const void* as_ptr_P[2];
  void* as_ptr[2];
  struct {
    uint16_t low;
    uint16_t high;
  };
};

/**
 * Compiler branch prediction hinting.
 */
#define LIKELY(x) __builtin_expect((x),1)
#define UNLIKELY(x) __builtin_expect((x),0)

/**
 * Compiler warning on unused varable.
 */
#define UNUSED(x) (void) (x)

/**
 * Macro for number of elements in a vector.
 * @param[in] x vector
 * @return number of elements
 */
#define membersof(x) (sizeof(x) / sizeof(x[0]))

/**
 * Workaround for gcc program memory data warning in Arduino build
 * with older version of AVR-GCC. 
 */
#if ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 8))
# ifndef PROGMEM
#  define PROGMEM  __attribute__((section(".progmem.data")))
# endif
# define __PROGMEM PROGMEM
#else
# define __PROGMEM  __attribute__((section(".progmem.data")))
#endif

#undef PSTR
/**
 * Create constant string in program memory. Allow IOStream output 
 * operator. Allows link time reduction of equal strings or substrings.
 * @param[in] s string literal (at compile time).
 * @return string literal in program memory.
 */
#ifndef NSHARE
#define PSTR(str) __PSTR1(str,__COUNTER__)
#define __PSTR1(str,num) __PSTR2(str,num)
#define __PSTR2(str,num)						\
  (__extension__(							\
    {									\
       const char* ptr;							\
       asm volatile (							\
	 ".pushsection .progmem.data, \"SM\", @progbits, 1" "\n\t" 	\
	 "PSTR_" #num ": .string " #str                     "\n\t"	\
	 ".popsection"                                      "\n\t"	\
       ); 								\
       asm volatile (							\
	 "ldi %A0, lo8(PSTR_" #num ")"	        	    "\n\t"	\
	 "ldi %B0, hi8(PSTR_" #num ")"   	    	    "\n\t"	\
	 : "=d" (ptr)							\
       ); 								\
       ptr;								\
     }									\
   ))
#else
#define PSTR(str) __PSTR(str)
#endif

/**
 * Program string literal that may be used in macro. Is not unique.
 * @param[in] s string literal (at compile time).
 * @return string literal in program memory.
 */
#define __PSTR(s)							\
  (__extension__(							\
    {									\
      static const char __c[]						\
        __attribute__((section(".progmem.pstr"))) = (s);		\
      &__c[0];								\
    }									\
  ))

/** String in program memory */
typedef const PROGMEM char* str_P;

/** Pointer table in program memory */
typedef const PROGMEM void* void_P;
typedef const PROGMEM void_P void_vec_P;

/**
 * Instruction clock cycles per micro-second. Assumes clock greater
 * or equal to 1 MHz.
 */
#define I_CPU (F_CPU / 1000000L)

/**
 * Macro for micro-second level delay. See also USLEEP().
 * @param[in] us micro-seconds.
 */
#define DELAY(us) _delay_loop_2(((us) * I_CPU) / 4)

/**
 * Macro for micro-second level delay. See also DELAY().
 * @param[in] us micro-seconds.
 */
#define USLEEP(us) DELAY(us)

/**
 * Macro for sleep for number of milli-seconds. Requires include of the
 * Watchdog and that it has been initiated with Watchdog::begin().
 * @param[in] ms milli-seconds.
 */
#define MSLEEP(ms) Watchdog::delay(ms)

/**
 * Macro for sleep for number of seconds. Requires include of the
 * Watchdog and that it has been initiated with
 * Watchdog::begin(). Allowed values are; 1, 2, 4, and 8 seconds. 
 * @param[in] seconds.
 */
#define SLEEP(seconds) Watchdog::delay(seconds * 1000)

/**
 * Disable interrupts and return flags.
 * @return processor flags.
 */
inline uint8_t 
lock() 
{ 
  uint8_t key = SREG;
  cli();
  return (key);
}

/**
 * Restore processor flags and possible enable of interrupts.
 * @param[in] key processor flags.
 */
inline void 
unlock(uint8_t key)
{
  SREG = key;
}

/**
 * Syntactic sugar for synchronized block. Used in the form:
 * synchronized {
 *   ...
 *   synchronized_return(expr);
 *   ...
 *   synchronized_goto(label);
 *   ...
 * }
 * Interrupts are disabled in the block allowing secure update.
 */
#define synchronized							\
  for (uint8_t __key = lock(), i = 1; i != 0; i--, unlock(__key))
#define synchronized_return(expr)					\
  return (unlock(__key), expr)
#define synchronized_goto(label)					\
  do { unlock(__key); goto label; } while (0)

/**
 * Force compiler to store all values in memory at this point.
 * Alternative to volatile declaration.
 */
#define barrier() __asm__ __volatile__("nop" ::: "memory") 

/**
 * Buffer structure for scatter/gather.
 */
struct iovec_t {
  void* buf;
  size_t size;
};

/**
 * Return total size of null terminated io buffer vector.
 * @return size.
 */
inline size_t
iovec_size(const iovec_t* vec)
{
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++)
    len += vp->size;
  return (len);
}

/**
 * Set next io-vector buffer.
 * @param[in,out] vp io vector pointer
 * @param[in] buf buffer.
 * @param[in] size number of bytes.
 */
inline void
iovec_arg(iovec_t* &vp, const void* buf, size_t size)
{
  vp->buf = (void*) buf;
  vp->size = size;
  vp++;
}

/**
 * Mark end of io-vector buffer at given index.
 * @param[in,out] vp io vector.
 */
inline void
iovec_end(iovec_t* &vp)
{
  vp->buf = 0;
  vp->size = 0;
}

/**
 * Swap bytes in 16-bit integer.
 * @param[in] value to byte swap.
 * @return new value.
 */
inline int16_t
swap(int16_t value)
{
  asm volatile("mov __tmp_reg__, %A0" 	"\n\t"
	       "mov %A0, %B0" 		"\n\t"
	       "mov %B0, __tmp_reg__" 	"\n\t"
	       : "=r" (value)
	       : "0" (value)
	       );
  return (value);
}

/**
 * Swap bytes in 16-bit integer vector.
 * @param[in] dest destination buffer.
 * @param[in] src source buffer.
 * @param[in] size number of integers to swap.
 */
inline void
swap(int16_t* dest, const int16_t* src, size_t size)
{
  if (size == 0) return;
  do { 
    *dest++ = swap(*src++); 
  } while (--size);
}

/**
 * Swap bytes in 16-bit integers in struct.
 * @param[in] T type of struct.
 * @param[in] dest destination buffer.
 * @param[in] src source buffer.
 */
template<class T> 
void swap(T* dest, const T* src)
{
  swap((int16_t*) dest, (const int16_t*) src, sizeof(T) / sizeof(int16_t));
}

/**
 * Destructive swap bytes in 16-bit integer vector.
 * @param[in] buf buffer.
 * @param[in] size number of integers to swap.
 */
inline void
swap(int16_t* buf, size_t size)
{
  if (size == 0) return;
  do { 
    int16_t data = *buf;
    *buf++ = swap(data); 
  } while (--size);
}

/**
 * Destructive swap bytes in 16-bit integers in struct.
 * @param[in] T type of struct.
 * @param[in] buf buffer.
 */
template<class T> 
void swap(T* buf)
{
  swap((int16_t*) buf, sizeof(T) / sizeof(int16_t));
}

/**
 * Swap bytes in 32-bit integer.
 * @param[in] value to byte swap.
 * @return new value.
 */
inline int32_t
swap(int32_t value)
{
  asm volatile("mov __tmp_reg__, %A0" 	"\n\t"
	       "mov %A0, %D0" 		"\n\t"
	       "mov %D0, __tmp_reg__" 	"\n\t"
	       "mov __tmp_reg__, %B0" 	"\n\t"
	       "mov %B0, %C0" 		"\n\t"
	       "mov %C0, __tmp_reg__" 	"\n\t"
	       : "=r" (value)
	       : "0" (value)
	       );
  return (value);
}

/**
 * Convert values between host and network byte order. AVR is
 * littlendian and network is bigendian so byte swap.
 */
#define ntoh swap
#define hton swap

/**
 * Template map function for given class/data type.
 * @param[in] T class value to map.
 * @param[in] x value to map.
 * @param[in] in_min minimum value in input range.
 * @param[in] in_max maximum value in input range.
 * @param[in] out_min minimum value in output range.
 * @param[in] out_max maximum value in output range.
 * @return mapping
 */
template<class T>
T map(T x, T in_min, T in_max, T out_min, T out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/**
 * Template constrain function for given class/data type.
 * @param[in] T class value to constrain.
 * @param[in] x value to constrain.
 * @param[in] low minimum value.
 * @param[in] high maximum value.
 * @return constrain
 */
template<class T>
T constrain(T x, T low, T high) 
{
  return (x < low ? low : (x > high ? high : x));
}

/**
 * Convert 4-bit LSB value to hexadecimal character ('0'..'f').
 * @param[in] value.
 * @return character.
 */
inline char
tohex(uint8_t value)
{
  value &= 0xf;
  if (value > 9) 
    return (value - 10 + 'a');
  return (value + '0');
}

/**
 * Convert 4-bit LSB value to hexadecimal character ('0'..'F').
 * @param[in] value.
 * @return character.
 */
inline char
toHEX(uint8_t value)
{
  value &= 0xf;
  if (value > 9) 
    return (value - 10 + 'A');
  return (value + '0');
}

#endif

