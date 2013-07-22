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

IOStream& operator<<(IOStream& outs, const Socket::addr_t& src)
{
  univ32_t addr;
  addr.as_uint32 = src.addr;
  outs << addr.as_uint8[3] << '.'
       << addr.as_uint8[2] << '.'
       << addr.as_uint8[1] << '.'
       << addr.as_uint8[0] << ':' 
       << src.port;
  return (outs);
}

int8_t
Socket::Device::attach(Socket* s)
{
  int8_t res = -1;
  for (uint8_t i = 0; i < SOCKET_MAX; i++) {
    if (m_socket[i] == s) return (i);
    if (m_socket[i] == 0 && res < 0) res = i;
  }
  if (res != -1) m_socket[res] = s;
  return (res);
}

void
Socket::Device::detach(Socket* s)
{
  for (uint8_t i = 0; i < SOCKET_MAX; i++) {
    if (m_socket[i] != s) continue;
    m_socket[i] = 0;
    return;
  }
}

Socket*
Socket::Device::lookup(uint16_t port)
{
  for (uint8_t i = 0; i < SOCKET_MAX; i++) {
    Socket* s = m_socket[i];
    if (s == 0) continue;
    if (s->m_port != port) continue;
    return (s);
  }
  return (0);
}

void 
Socket::Device::set_connected(Client* client, const Socket::addr_t& dest)
{
  client->m_state = Client::CONNECTED_STATE;
  client->m_dest = dest;
}

void 
Socket::Device::set_disconnected(Client* client)
{
  client->m_state = Client::DISCONNECTED_STATE;
}

void 
Socket::Device::set_port(Client* client, uint16_t port)
{
  client->m_port = port;
}

Socket::addr_t
Socket::Device::get_dest_address(Client* client)
{
  return (client->m_dest);
}
