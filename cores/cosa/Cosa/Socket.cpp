/**
 * @file Cosa/Socket.cpp
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

#include "Cosa/Socket.hh"

Socket::Socket() : 
  IOStream::Device(),
  m_port(0)
{
  memset(m_mac, 0, sizeof(m_mac));
  memset(m_addr, 0, sizeof(m_addr));
}

int 
Socket::write(const void* buf, size_t size)
{
  return (send(buf, size));
}

int 
Socket::write_P(const void* buf, size_t size)
{
  return (send_P(buf, size));
}

int 
Socket::getchar()
{
  char c;
  int res = recv(&c, sizeof(c));
  return (res == sizeof(c) ? (c & 0xff) : res);
}

int 
Socket::read(void* buf, size_t size)
{
  return (recv(buf, size));
}

bool
Socket::is_illegal(uint8_t addr[4], uint16_t port)
{
  return 
    (((addr[0] == 0xff) && (addr[1] == 0xff) && (addr[2] == 0xff) && (addr[3] == 0xff)) ||
     ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
     (port == 0x0000));
}

