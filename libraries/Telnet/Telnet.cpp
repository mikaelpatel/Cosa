/**
 * @file Telnet.cpp
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

#include "Telnet.hh"

bool
Telnet::Server::begin(Socket* sock)
{
  // Sanity check parameter
  if (sock == NULL) return (false);

  // Set telnet end of line mode (crlf)
  sock->set_eol(IOStream::CRLF_MODE);

  // Complete the setup
  return (INET::Server::begin(sock));
}

bool
Telnet::Server::on_accept(IOStream& ios)
{
  UNUSED(ios);

  // Sanity check server state
  Socket* sock = get_socket();
  if (sock == NULL) return (false);

  // Skip first line from client; terminal settings not implemented
  int res;
  while ((res = sock->available()) == 0) yield();
  if (res < 0) return (false);
  while (res--) sock->getchar();
  return (true);
}
