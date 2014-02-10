/**
 * @file Cosa/INET/HTTP.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#include "Cosa/INET/HTTP.hh"
#include "Cosa/Watchdog.hh"

bool
HTTP::Server::begin(Socket* sock)
{
  if (sock == NULL) return (false);
  m_sock = sock;
  return (!sock->listen());
}

int
HTTP::Server::request(uint32_t ms)
{
  // Wait for incoming connection requests
  uint32_t start = Watchdog::millis();
  int res;
  while (((res = m_sock->accept()) != 0) &&
	 ((ms == 0L) || (Watchdog::millis() - start < ms)))
    Watchdog::delay(16);
  if (res != 0) return (-2);

  // Wait for the HTTP request
  while ((res = m_sock->available()) == 0) Watchdog::delay(16);
  if (res < 0) goto error;

  // Read request, call handler and flush/send any buffered response
  char url[64];
  m_sock->gets(url, sizeof(url));
  on_request(url);
  m_sock->flush();
  
  // Disconnect the client and allow new connection requests
 error:
  m_sock->disconnect();
  m_sock->listen();
  return (res);
}

bool
HTTP::Server::end()
{
  if (m_sock == NULL) return (false);
  m_sock->close();
  m_sock = NULL;
  return (true);
}
