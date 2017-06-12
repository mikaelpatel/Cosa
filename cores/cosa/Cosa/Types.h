/**
 * @file Cosa/Types.h
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
 * @section Description
 * Common literals, data types and syntax abstractions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_TYPES_H
#define COSA_TYPES_H

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
#include "Cosa.h"

/**
 * Create an unique symbol for macro from given name and line.
 */
#define __UNIQUE(name) __CONCAT(name,__LINE__)

/**
 * Round integer division.
 */
#define ROUND(x,y) (((x) + (y - 1)) / (y))

/**
 * Number of bits in a character.
 */
#define CHARBITS 8

/**
 * Number of bytes for given number of bits.
 */
#define BYTES(bits) ROUND(bits, CHARBITS)

/**
 * Standard integer range
 */
#ifndef UINT8_MAX
# define UINT8_MAX ((uin8_t) 0xffU)
#endif
#ifndef UINT16_MAX
# define UINT16_MAX ((uint16_t) 0xffffU)
#endif
#ifndef UINT32_MAX
# define UINT32_MAX ((uint32_t) 0xffffffffUL)
#endif
#ifndef INT_MAX
# define INT_MIN INT16_MIN
# define INT_MAX INT16_MAX
#endif
#ifndef INT8_MAX
# define INT8_MIN ((int8_t) 0x80)
# define INT8_MAX ((int8_t) 0x7f)
#endif
#ifndef INT16_MAX
# define INT16_MIN ((int16_t) 0x8000)
# define INT16_MAX ((int16_t) 0x7fff)
#endif
#ifndef INT32_MAX
# define INT32_MIN ((int32_t) 0x80000000L)
# define INT32_MAX ((int32_t) 0x7fffffffL)
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
 * Compiler branch prediction hinting. The AVR processor pipeline will
 * stall for one cycle when a condition is not true and a branch is
 * taken. UNLIKELY will help the compiler adjust the code generation
 * so that the branch is not taken.
 */
#define LIKELY(x) __builtin_expect((x), true)
#define UNLIKELY(x) __builtin_expect((x), false)

/**
 * Compiler warning on unused varable.
 */
#define UNUSED(x) (void) (x)

/**
 * Macro for number of elements in a vector.
 * @param[in] x vector.
 * @return number of elements.
 */
#define membersof(x) (sizeof(x) / sizeof(x[0]))

/*
 * Workaround for gcc offsetof macro usage and program memory data
 * warning in Arduino build with older version of AVR-GCC (1.0.5,
 * 1.5.6-r2 etc).
 */
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ <= 3))
# undef offsetof
# define offsetof(t,m)							\
  (__extension__(							\
    {									\
      const t* __p = NULL;						\
      (size_t) &__p->m;							\
    }									\
  ))
# define __PROGMEM  __attribute__((section(".progmem.data")))
#else
# define __PROGMEM PROGMEM
#endif

/** Unique data type for strings in program memory. */
typedef const PROGMEM class prog_str* str_P;

/**
 * Program string literal that may be used in macro. Is not unique.
 * @param[in] s string literal (at compile time).
 * @return string literal in program memory.
 */
#define STR_P(s)							\
  (__extension__(							\
    {									\
      static const char __c[] __PROGMEM = (s);				\
      (str_P) &__c[0];							\
    }									\
  ))
#undef PSTR
#define PSTR(s) STR_P(s)
#define __PSTR(s) STR_P(s)

inline char*
strcat_P(char* s1, str_P s2)
{
  return (strcat_P(s1, (const char*) s2));
}

inline str_P
strchr_P(str_P s, int __val)
{
  return ((str_P) strchr_P((const char*) s, __val));
}

inline str_P
strchrnul_P(str_P s, int __val)
{
  return ((str_P) strchrnul_P((const char*) s, __val));
}

inline int
strcmp_P(const char *s1, str_P s2)
{
  return (strcmp_P(s1, (const char*) s2));
}

inline int
strncmp_P(const char *s1, str_P s2, size_t n)
{
  return (strncmp_P(s1, (const char*) s2, n));
}

inline char*
strcpy_P(char* s1, str_P s2)
{
  return (strcpy_P(s1, (const char*) s2));
}

inline int
strcasecmp_P(const char *s1, str_P s2)
{
  return (strcasecmp_P(s1, (const char*) s2));
}

inline char*
strcasestr_P(const char *s1, str_P s2)
{
  return (strcasestr_P(s1, (const char*) s2));
}

inline size_t
strlen_P(str_P s)
{
  return (strlen_P((const char*) s));
}

/** Pointer table in program memory. */
typedef const PROGMEM void* void_P;
typedef const PROGMEM void_P void_vec_P;

/* Check if static_assert needs to be disabled */
#if (ARDUINO < 150)
# if !defined(__GXX_EXPERIMENTAL_CXX0X__)
#   define static_assert(condition,message)
# endif
#endif

/**
 * Instruction clock cycles per micro-second. Assumes clock greater
 * or equal to 1 MHz.
 */
#define I_CPU (F_CPU / 1000000L)

/**
 * Macro for micro-second level delay.
 * @param[in] us micro-seconds.
 */
#define DELAY(us) _delay_loop_2((us) * (F_CPU / 4000000L))

/**
 * Delay given number of milli-seconds. This function pointer may be
 * redefined to allow low-power and/or multi-tasking duing wait.
 * @param[in] ms milli-seconds delay.
 */
extern void (*delay)(uint32_t ms);

/**
 * Sleep given number of seconds. This function pointer may be
 * redefined to allow low-power and/or multi-tasking duing wait.
 * @param[in] s seconds delay.
 */
extern void (*sleep)(uint16_t s);

/**
 * Allow context switch to other tasks if available. The default
 * implementation is a low-power sleep and wait for interrupt.
 */
extern void (*yield)();

/**
 * No-operation; 1 clock cycle delay.
 */
#define nop() __asm__ __volatile__("nop")

/**
 * Force compiler to store all values in memory at this
 * point. Compiler may not reorder statements and sub-expression over
 * barriers. This is an alternative to volatile declaration.
 */
#define barrier() __asm__ __volatile__("" ::: "memory")

/**
 * Disable interrupts and return flags.
 * @return processor flags.
 */
inline uint8_t lock() __attribute__((always_inline));
inline uint8_t lock()
{
  uint8_t key = SREG;
  __asm__ __volatile__("cli" ::: "memory");
  return (key);
}

/**
 * Restore processor flags and possible enable of interrupts.
 * @param[in] key processor flags.
 */
inline void unlock(uint8_t key) __attribute__((always_inline));
inline void unlock(uint8_t key)
{
  SREG = key;
  __asm__ __volatile__("" ::: "memory");
}

/**
 * Restore processor flags and possible enable of interrupts.
 * Internal clean up function for synchronized block.
 * @param[in] key processor flags.
 */
inline void __unlock(uint8_t* key) __attribute__((always_inline));
inline void __unlock(uint8_t* key)
{
  SREG = *key;
  __asm__ __volatile__("" ::: "memory");
}

/**
 * Syntactic sugar for synchronized block. Used in the form:
 * @code
 * synchronized {
 *   ...
 * }
 * label:
 * @endcode
 * Interrupts are disabled in the block allowing secure update.
 * All control structures are allowed (e.g. return, goto).
 */
#define synchronized							\
  for (uint8_t __key __attribute__((__cleanup__(__unlock))) = lock(),	\
       i = 1; i != 0; i--)

/**
 * Conditional variable.
 */
typedef volatile bool condvar_t;

/**
 * Wait until condition variable is true then disable interrupts and
 * set condition variable to false and return flags.
 * @param[in] cond condition variable.
 * @return processor flags.
 */
inline uint8_t lock(condvar_t &cond) __attribute__((always_inline));
inline uint8_t lock(condvar_t &cond)
{
  uint8_t key = lock();
  while (UNLIKELY(cond)) {
    unlock(key);
    yield();
    key = lock();
  }
  cond = true;
  return (key);
}

/**
 * Buffer structure for scatter/gather.
 */
struct iovec_t {
  void* buf;			//!< Buffer pointer.
  size_t size;			//!< Size of buffer in bytes.
  iovec_t(void* buf = NULL, size_t size = 0)
  {
    this->buf = buf;
    this->size = size;
  }
};

/**
 * Return total size of null terminated io buffer vector.
 * @param[in] vp io vector pointer
 * @return size.
 */
inline size_t iovec_size(const iovec_t* vec) __attribute__((always_inline));
inline size_t iovec_size(const iovec_t* vec)
{
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++)
    len += vp->size;
  return (len);
}

/**
 * Set next io-vector buffer. Used in to form:
 * @code
 * iovec_t vec[N];
 * iovec_t* vp = vec;
 * iovec_arg(vp, buf, size);
 * ..
 * iovec_end(vp);
 * @endcode
 * @param[in,out] vp io vector pointer
 * @param[in] buf buffer.
 * @param[in] size number of bytes.
 */
inline void iovec_arg(iovec_t* &vp, const void* buf, size_t size)
  __attribute__((always_inline));
inline void iovec_arg(iovec_t* &vp, const void* buf, size_t size)
{
  vp->buf = (void*) buf;
  vp->size = size;
  vp++;
}

/**
 * Mark end of io-vector buffer at given index. Used in the form:
 * @code
 * iovec_t vec[N];
 * iovec_t* vp = vec;
 * iovec_arg(vp, buf, size);
 * ..
 * iovec_end(vp);
 * @endcode
 * @param[in,out] vp io vector.
 */
inline void iovec_end(iovec_t* &vp) __attribute__((always_inline));
inline void iovec_end(iovec_t* &vp)
{
  vp->buf = 0;
  vp->size = 0;
}

/**
 * Swap bytes in 16-bit unsigned integer.
 * @param[in] value to byte swap.
 * @return new value.
 */
inline uint16_t swap(uint16_t value) __attribute__((always_inline));
inline uint16_t swap(uint16_t value)
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
 * Swap bytes in 16-bit unsigned integer vector.
 * @param[in] dest destination buffer.
 * @param[in] src source buffer.
 * @param[in] size number of integers to swap.
 */
inline void swap(uint16_t* dest, const uint16_t* src, size_t size)
{
  if (UNLIKELY(size == 0)) return;
  do {
    *dest++ = swap(*src++);
  } while (--size);
}

/**
 * Swap bytes in 16-bit values in struct.
 * @param[in] T type of struct.
 * @param[in] dest destination buffer.
 * @param[in] src source buffer.
 */
template<class T>
void swap(T* dest, const T* src)
{
  swap((uint16_t*) dest, (const uint16_t*) src, sizeof(T) / sizeof(uint16_t));
}

/**
 * Destructive swap bytes in 16-bit unsigned integer vector.
 * @param[in] buf buffer.
 * @param[in] size number of integers to swap.
 */
inline void swap(uint16_t* buf, size_t size)
{
  if (UNLIKELY(size == 0)) return;
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
  swap((uint16_t*) buf, sizeof(T) / sizeof(uint16_t));
}

/**
 * Swap bytes in 16-bit signed integer.
 * @param[in] value to byte swap.
 * @return new value.
 */
inline int16_t swap(int16_t value) __attribute__((always_inline));
inline int16_t swap(int16_t value)
{
  return ((int16_t) swap((uint16_t) value));
}

/**
 * Swap bytes in 16-bit integer vector.
 * @param[in] dest destination buffer.
 * @param[in] src source buffer.
 * @param[in] size number of integers to swap.
 */
inline void swap(int16_t* dest, const int16_t* src, size_t size)
{
  if (UNLIKELY(size == 0)) return;
  do {
    *dest++ = swap(*src++);
  } while (--size);
}

/**
 * Swap bytes in 32-bit unsigned integer.
 * @param[in] value to byte swap.
 * @return new value.
 */
inline uint32_t swap(uint32_t value) __attribute__((always_inline));
inline uint32_t swap(uint32_t value)
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
 * Swap bytes in 32-bit signed integer.
 * @param[in] value to byte swap.
 * @return new value.
 */
inline int32_t swap(int32_t value) __attribute__((always_inline));
inline int32_t swap(int32_t value)
{
  return ((int32_t) swap((uint32_t) value));
}

/**
 * Convert values between host and network byte order. AVR is
 * littlendian and network is bigendian so byte swap.
 */
#define ntoh swap
#define hton swap

/**
 * Convert 4-bit LSB value to hexadecimal character ('0'..'f').
 * @param[in] value.
 * @return character.
 */
inline char tohex(uint8_t value) __attribute__((always_inline));
inline char tohex(uint8_t value)
{
  value &= 0xf;
  if (UNLIKELY(value > 9))
    return (value - 10 + 'a');
  return (value + '0');
}

/**
 * Convert 4-bit LSB value to hexadecimal character ('0'..'F').
 * @param[in] value.
 * @return character.
 */
inline char toHEX(uint8_t value) __attribute__((always_inline));
inline char toHEX(uint8_t value)
{
  value &= 0xf;
  if (UNLIKELY(value > 9))
    return (value - 10 + 'A');
  return (value + '0');
}

#endif
