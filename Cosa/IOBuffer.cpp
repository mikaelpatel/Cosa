/**
 * @file Cosa/IOBuffer.cpp
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
 * Ring buffer for IOStreams.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/IOBuffer.hh"

int 
IOBuffer::putchar(char c)
{
  uint8_t next = (m_head + 1) & BUFFER_MASK;
  if (next == m_tail) return (-1);
  m_buffer[next] = c;
  m_head = next;
  return (c & 0xff);
}

int 
IOBuffer::getchar()
{
  if (is_empty()) return (-1);
  uint8_t next = (m_tail + 1) & BUFFER_MASK;
  int c = m_buffer[m_tail];
  m_tail = next;
  return (c & 0xff);
}

int 
IOBuffer::flush()
{
  m_tail = 0;
  m_head = 0;
}
