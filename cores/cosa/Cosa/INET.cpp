/**
 * @file Cosa/INET.cpp
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

#include "Cosa/INET.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Socket.hh"
#include <ctype.h>

bool
INET::is_illegal(uint8_t addr[4], uint16_t port)
{
  return 
    (((addr[0] == 0xff) 
      && (addr[1] == 0xff) 
      && (addr[2] == 0xff) 
      && (addr[3] == 0xff)) 
     ||
     ((addr[0] == 0x00) 
      && (addr[1] == 0x00) 
      && (addr[2] == 0x00) 
      && (addr[3] == 0x00)) 
     ||
     (port == 0x0000));
}

int 
INET::aton(const char* addr, uint8_t ip[4], bool progmem)
{
  const char* ap = addr;
  char c;
  for (uint8_t i = 0; i < IP_MAX; i++) {
    uint16_t r = 0;
    while (isdigit(c = (progmem ? pgm_read_byte(ap++) : *ap++)))
      r = (r * 10) + (c - '0');
    if (c != 0) {
      if (c != '.') return (-1);
    }
    else {
      if (i < IP_MAX - 1) return (-1);
    }
    if (r > 255) return (-1);
    ip[i] = r;
  }
  return (0);
}

int
INET::nametopath(const char* hostname, char* path, bool progmem)
{
  const char* hp = hostname;
  char* sp = path;
  char* np = path + 1;
  uint8_t n = 0;
  int res = 0;
  while (res < PATH_MAX) {
    char c = (progmem ? pgm_read_byte(hp++) : *hp++);
    if (c == 0 || c == '.') {
      if (n == 0) return (-1);
      *sp = n;
      res += n + 1;
      sp = np++;
      n = 0;
      if (c == 0) {
	*sp = 0;
	return (res + 1);
      };
    }
    else {
      *np++ = c;
      n++;
    }
  }
  return (-2);
}

void
INET::print_path(IOStream& outs, const char* path)
{
  const char* p = path;
  uint8_t n = *p++;
  int i = 0;
  char c;
  while (i < PATH_MAX) {
    if (n == 0) return;
    i += n;
    while (n--) {
      c = *p++;
      outs << c;
    }
    n = *p++;
    if (n != 0) outs << '.';
  }
}

void
INET::print_mac(IOStream& outs, const uint8_t* mac)
{
  outs << toHEX(mac[0] >> 4) << toHEX(mac[0]);
  for (uint8_t i = 1; i < MAC_MAX; i++) 
    outs << ':' << toHEX(mac[i] >> 4) << toHEX(mac[i]);
}

void
INET::print_addr(IOStream& outs, const uint8_t* addr, uint16_t port)
{
  outs << addr[0];
  for (uint8_t i = 1; i < IP_MAX; i++) outs << '.' << addr[i];
  if (port == 0) return;
  outs << ':' << port;
}

void 
INET::Server::get_client(INET::addr_t& addr)
{
  Socket* sock = get_socket();
  if (sock == NULL) return;
  sock->get_src(addr);
}  

bool 
INET::Server::begin(Socket* sock)
{
  // Sanity check parameter
  if (sock == NULL) return (false);

  // Bind to io-stream
  m_ios.set_device(sock);

  // Set socket to listen mode
  return (sock->listen() == 0);
}

int 
INET::Server::run(uint32_t ms)
{
  // Sanity check server state
  Socket* sock = get_socket();
  if (sock == NULL) return (-1);

  // When not connected; Check incoming connect requests
  uint32_t start = Watchdog::millis();
  int res;
  if (!m_connected) {
    while (((res = sock->accept()) != 0) &&
	   ((ms == 0L) || (Watchdog::since(start) < ms))) 
      yield();
    if (res != 0) return (-2);
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
  Socket* sock = get_socket();
  if (sock == NULL) return (false);

  // Close the socket and mark as disconnected
  sock->close();
  m_connected = false;
  return (true);
}

