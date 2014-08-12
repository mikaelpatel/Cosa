/**
 * @file Cosa/INET/Telnet.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/INET/Telnet.hh"
#include "Cosa/Watchdog.hh"

bool 
Telnet::Server::begin(Socket* sock)
{
  if (sock == NULL) return (false);
  // Set socket and bind to io-stream
  m_sock = sock;
  m_sock->set_eol(IOStream::CRLF_MODE);
  m_ios.set_device(m_sock);
  // Set in listen mode
  return (sock->listen() == 0);
}

int 
Telnet::Server::run(uint32_t ms)
{
  uint32_t start = Watchdog::millis();
  int res;

  // Check if not checked; accept incoming requests
  if (!m_connected) {
    while (((res = m_sock->accept()) != 0) &&
	   ((ms == 0L) || (Watchdog::since(start) < ms))) 
      yield();
    if (res != 0) return (-2);
    // Skip first line from client; terminal settings
    while ((res = m_sock->available()) == 0) yield();
    if (res < 0) goto error;
    while (res--) m_sock->getchar();
    // Call handler for initial prompt
    on_request(m_ios);
    m_sock->flush();
    m_connected = true;
    return (0);
  }

  // Client has been accepted; check for incoming command
  while (((res = m_sock->available()) == 0) &&
	 ((ms == 0L) || (Watchdog::since(start) < ms))) 
    yield();
  if (res > 0) {
    on_request(m_ios);
    res = m_sock->flush();
  }
  if (res == 0) return (0);

 error:
  // Error handling; close and restart listen mode
  m_connected = false;
  m_sock->disconnect();
  m_sock->listen();
  return (res);
}

bool 
Telnet::Server::end()
{
  if (m_sock == NULL) return (false);
  // Close the socket and mark as disconnected
  m_sock->close();
  m_sock = NULL;
  m_connected = false;
  return (true);
}

