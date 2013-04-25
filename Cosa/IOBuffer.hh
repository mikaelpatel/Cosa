/**
 * @file Cosa/IOBuffer.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOBUFFER_HH__
#define __COSA_IOBUFFER_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Power.hh"

/**
 * Circular buffer template class for IOStreams. Size must be
 * Power(2). May be used as a string buffer device, or to connect
 * different IOStreams. See UART.hh for an example. Buffer size should
 * be power of 2 and max 256.
 * @param[in] size number of bytes in buffer.
 */
template <uint8_t size>
class IOBuffer : public IOStream::Device {
private:
  volatile uint8_t m_head;
  volatile uint8_t m_tail;
  char m_buffer[size];

public:
  /**
   * Allocate buffer object for iostream operations. 
   */
  IOBuffer() :
    IOStream::Device(),
    m_head(0),
    m_tail(0)
  {
  }

  /**
   * Return true(1) if the buffer is empty, otherwise false(0).
   * @return bool
   */
  bool is_empty()
  {
    return (m_head == m_tail);
  }

  /**
   * Return true(1) if the buffer is full, otherwise false(0).
   * @return bool
   */
  bool is_full()
  {
    return (((m_head + 1) & (size - 1)) == m_tail);
  }

  /**
   * @override
   * Number of bytes available in buffer.
   * @return bytes.
   */
  virtual int available()
  {
    return (((size - 1) + m_head - m_tail) % (size - 1));
  }

  /**
   * @override
   * Write character to buffer.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * @override
   * Peek at the next character from buffer.
   * @return character or EOF(-1).
   */
  virtual int peekchar();
    
  /**
   * @override
   * Read character from buffer.
   * @return character or EOF(-1).
   */
  virtual int getchar();

  /**
   * @override
   * Wait for the buffer to become empty.
   * @param[in] mode sleep mode on flush wait.
   * @return zero(0) or negative error code.
   */
  virtual int flush(uint8_t mode = SLEEP_MODE_IDLE);
};

template <uint8_t size>
int 
IOBuffer<size>::putchar(char c)
{
  uint8_t next = (m_head + 1) & (size - 1);
  if (next == m_tail) return (-1);
  m_buffer[next] = c;
  m_head = next;
  return (c & 0xff);
}

template <uint8_t size>
int 
IOBuffer<size>::peekchar()
{
  if (m_head == m_tail) return (-1);
  uint8_t next = (m_tail + 1) & (size - 1);
  return (m_buffer[next]);
}

template <uint8_t size>
int 
IOBuffer<size>::getchar()
{
  if (m_head == m_tail) return (-1);
  uint8_t next = (m_tail + 1) & (size - 1);
  m_tail = next;
  return (m_buffer[next]);
}

template <uint8_t size>
int
IOBuffer<size>::flush(uint8_t mode)
{
  while (m_head != m_tail) Power::sleep(mode);
  return (0);
}

#endif
