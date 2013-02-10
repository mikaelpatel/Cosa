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
 * @section Description
 * Circlic buffer for IOStreams. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOBUFFER_HH__
#define __COSA_IOBUFFER_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

class IOBuffer : public IOStream::Device {
private:
  volatile char* m_buffer;
  volatile uint8_t m_head;
  volatile uint8_t m_tail;
  uint8_t m_free;

public:
  const uint8_t BUFFER_MAX;
  const uint8_t BUFFER_MASK;

  /**
   * Allocate buffer object for iostream operations. Buffer size (max) 
   * should be power of 2.
   * @param[in] max number of bytes in buffer.
   * @param[in] buffer pointer to buffer.
   */
  IOBuffer(uint8_t max, char* buffer = 0) :
    IOStream::Device(),
    m_buffer(buffer != 0 ? buffer : (char*) malloc(max)),
    m_head(0),
    m_tail(0),
    m_free(buffer == 0),
    BUFFER_MAX(max),
    BUFFER_MASK(max - 1)
  {
  }

  /**
   * Deallocate buffer object. Free buffer if allocated
   * with malloc by constructor.
   */
  ~IOBuffer() 
  {
    if (m_free) free((void*) m_buffer);
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
    return (((m_head + 1) & BUFFER_MASK) == m_tail);
  }

  /**
   * @override
   * Number of bytes available in buffer.
   * @return bytes.
   */
  virtual int available()
  {
    return ((BUFFER_MASK + m_head - m_tail) % BUFFER_MASK);
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
   * Read character from buffer.
   * @return character or EOF(-1).
   */
  virtual int getchar();

  /**
   * @override
   * Reset buffer.
   * @return zero(0) or negative error code.
   */
  virtual int flush()
  {
    m_tail = 0;
    m_head = 0;
  }
};

#endif
