/**
 * @file Cosa/IOBuffer.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_IOBUFFER_HH
#define COSA_IOBUFFER_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Power.hh"

/**
 * Circular buffer template class for IOStreams. May be used as a
 * string buffer device, or to connect different IOStreams. See
 * UART.hh for an example. Buffer size should be power of 2 and
 * max 32Kbyte.
 * @param[in] SIZE number of bytes in buffer.
 */
template <uint16_t SIZE>
class IOBuffer : public IOStream::Device {
  static_assert(SIZE && !(SIZE & (SIZE - 1)), "SIZE should be power of 2");
public:
  /**
   * Constuct buffer object for stream operations.
   */
  IOBuffer() :
    IOStream::Device(),
    m_head(0),
    m_tail(0)
  {}

  /**
   * Return true(1) if the buffer is empty, otherwise false(0).
   * @return bool
   */
  bool is_empty()
    __attribute__((always_inline))
  {
    return (m_head == m_tail);
  }

  /**
   * Return true(1) if the buffer is full, otherwise false(0).
   * @return bool
   */
  bool is_full()
    __attribute__((always_inline))
  {
    return (((m_head + 1) & MASK) == m_tail);
  }

  /**
   * @override{IOStream::Device}
   * Number of bytes available in buffer before empty.
   * @return bytes.
   */
  virtual int available()
    __attribute__((always_inline))
  {
    return ((SIZE + m_head - m_tail) & MASK);
  }

  /**
   * @override{IOStream::Device}
   * Number of bytes room in buffer before full.
   * @return bytes.
   */
  virtual int room()
    __attribute__((always_inline))
  {
    return ((SIZE - m_head + m_tail - 1) & MASK);
  }

  /**
   * @override{IOStream::Device}
   * Write character to buffer.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override{IOStream::Device}
   * Peek at the next character from buffer.
   * @return character or EOF(-1).
   */
  virtual int peekchar();

  /**
   * @override{IOStream::Device}
   * Peek for the given character in the stream. Return number of
   * character or EOF(-1).
   * @return number of characters or EOF(-1).
   */
  virtual int peekchar(char c);

  /**
   * @override{IOStream::Device}
   * Read character from buffer.
   * @return character or EOF(-1).
   */
  virtual int getchar();

  /**
   * @override{IOStream::Device}
   * Wait for the buffer to become empty.
   * @return zero(0) or negative error code.
   */
  virtual int flush();

  /**
   * Empty the buffer.
   */
  virtual void empty()
    __attribute__((always_inline))
  {
    m_head = m_tail = 0;
  }

  /**
   * Cast iobuffer to a character pointer.
   */
  operator const char*()
    __attribute__((always_inline))
  {
    return (&m_buffer[m_tail + 1]);
  }

private:
  static const uint16_t MASK = (SIZE - 1);
  volatile uint16_t m_head;
  volatile uint16_t m_tail;
  char m_buffer[SIZE];
};

template <uint16_t SIZE>
int
IOBuffer<SIZE>::putchar(char c)
{
  uint16_t next = (m_head + 1) & MASK;
  if (UNLIKELY(next == m_tail)) return (IOStream::EOF);
  m_buffer[next] = c;
  m_head = next;
  return (c & 0xff);
}

template <uint16_t SIZE>
int
IOBuffer<SIZE>::peekchar()
{
  if (UNLIKELY(m_head == m_tail)) return (IOStream::EOF);
  uint16_t next = (m_tail + 1) & MASK;
  return (m_buffer[next] & 0xff);
}

template <uint16_t SIZE>
int
IOBuffer<SIZE>::peekchar(char c)
{
  uint16_t tail = m_tail;
  int res = 0;
  while (tail != m_head) {
    res += 1;
    tail = (tail + 1) & MASK;
    if (UNLIKELY(m_buffer[tail] == c)) return (res);
  }
  return (IOStream::EOF);
}

template <uint16_t SIZE>
int
IOBuffer<SIZE>::getchar()
{
  if (UNLIKELY(m_head == m_tail)) return (IOStream::EOF);
  uint16_t next = (m_tail + 1) & MASK;
  m_tail = next;
  return (m_buffer[next] & 0xff);
}

template <uint16_t SIZE>
int
IOBuffer<SIZE>::flush()
{
  while (m_head != m_tail) yield();
  return (0);
}

#endif
