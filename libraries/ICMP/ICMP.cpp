/**
 * @file ICMP.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "ICMP.hh"
#include "Cosa/RTC.hh"

int
ICMP::ping_request(uint8_t dest[4])
{
  // Check given socket
  if (m_sock == NULL) return (ENOTSOCK);

  // Build echo request block
  header_t req;
  req.type = ECHO;
  req.code = 0;
  req.checksum = 0;
  req.echo.id = m_id;
  req.echo.seq = ++m_seq;
  req.timestamp = RTC::millis();
  req.checksum = hton(INET::checksum(&req, sizeof(req)));

  // And send to destination network address
  int res = m_sock->send(&req, sizeof(req), dest, 0);
  if (res < 0) return (res);
  return (res == sizeof(req) ? 0 : -1);
}

int
ICMP::ping_await(uint16_t timeout)
{
  // Check given socket
  if (m_sock == NULL) return (ENOTSOCK);
  header_t reply;
  uint8_t src[4];
  uint16_t port;
  int res = 0;

  // Await the reply
  for (uint16_t retry = 0; retry < timeout; retry++) {
    if ((res = m_sock->available()) > 0) break;
    delay(1);
  }

  // Check size of reply before actually recieving
  if (res < (int) sizeof(reply)) return (EMSGSIZE);
  res = m_sock->recv(&reply, sizeof(reply), src, port);
  if (res < 0) return (res);

  // Sanity check the reply; right size, type, id, seq nr, and checksum
  if ((res != sizeof(reply))
      || (reply.type != ECHOREPLY)
      || (reply.echo.id != m_id)
      || (reply.echo.seq != m_seq)
      || (INET::checksum(&reply, sizeof(reply)) != 0))
    return (ENXIO);
  return (RTC::millis() - reply.timestamp);
}
