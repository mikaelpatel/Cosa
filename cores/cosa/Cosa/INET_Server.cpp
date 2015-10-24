/**
 * @file Cosa/INET_Server.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#include "Cosa/INET.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Socket.hh"

void
INET::Server::client(INET::addr_t& addr)
{
  Socket* sock = socket();
  if (UNLIKELY(sock == NULL)) return;
  sock->src(addr);
}

bool
INET::Server::begin(Socket* sock)
{
  // Sanity check parameter
  if (UNLIKELY(sock == NULL)) return (false);

  // Bind to io-stream
  m_ios.device(sock);

  // Set socket to listen mode
  return (sock->listen() == 0);
}

int
INET::Server::run(uint32_t ms)
{
  // Sanity check server state
  Socket* sock = socket();
  if (UNLIKELY(sock == NULL)) return (ENOTSOCK);

  // When not connected; Check incoming connect requests
  uint32_t start = Watchdog::millis();
  int res;
  if (!m_connected) {
    while (((res = sock->accept()) != 0) &&
	   ((ms == 0L) || (Watchdog::since(start) < ms)))
      yield();
    if (res != 0) return (ETIME);
    // Check if application accepts the connection
    if (!on_accept(m_ios)) goto error;
    // Run application connect
    on_connect(m_ios);
    // Flush response message
    sock->flush();
    m_connected = true;
    return (0);
  }

  // Client has been accepted; check for incoming requests
  while (((res = sock->available()) == 0) &&
	 ((ms == 0L) || (Watchdog::since(start) < ms)))
    yield();
  // If a message is available call application request handling
  if (res > 0) {
    on_request(m_ios);
    res = sock->flush();
  }
  if (res == 0) return (0);

 error:
  // Error handling; close and restart listen mode
  on_disconnect();
  m_connected = false;
  sock->disconnect();
  sock->listen();
  return (res);
}

bool
INET::Server::end()
{
  // Sanity check server state
  Socket* sock = socket();
  if (UNLIKELY(sock == NULL)) return (false);

  // Close the socket and mark as disconnected
  sock->close();
  m_connected = false;
  return (true);
}
