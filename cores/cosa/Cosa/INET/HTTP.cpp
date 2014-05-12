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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/INET/HTTP.hh"
#include "Cosa/Watchdog.hh"
#include <alloca.h>
#include <ctype.h>

#define CRLF "\r\n"

int
HTTP::Server::request(uint32_t ms)
{
  // Wait for incoming connection requests
  // NOTE: LTO error; need rewrite to stack allocation
  // char line[REQUEST_MAX];
  char* line = (char*) alloca(REQUEST_MAX);
  char* method;
  char* path;
  char* query;
  char* sp;
  int res;
  
  uint32_t start = Watchdog::millis();
  while (((res = m_sock->accept()) != 0) &&
	 ((ms == 0L) || (Watchdog::millis() - start < ms))) 
    yield();
  if (res != 0) return (-2);

  // Wait for the HTTP request
  while ((res = m_sock->available()) == 0) yield();

  // Parse request (method and url), call handler and flush buffered response
  if (res < 0) goto error;
  m_sock->gets(line, REQUEST_MAX);
  method = line;
  sp = strpbrk(line, " ");
  if (sp == NULL) goto error;
  path = sp + 1;
  *sp = 0;
  sp = strpbrk(path, " ?");
  if (sp == NULL) goto error;
  if (*sp != '?') 
    query = NULL;
  else {
    query = sp + 1;
    *sp = 0;
    sp =  strpbrk(query, " ");
    if (sp == NULL) goto error;
  }
  *sp = 0;
  on_request(method, path, query);
  m_sock->flush();

  // Disconnect the client and allow new connection requests
 error:
  m_sock->disconnect();
  m_sock->listen();
  return (res);
}

bool 
HTTP::Client::begin(Socket* sock)
{
  if (sock == NULL) return (false);
  m_sock = sock;
  return (true);
}

bool 
HTTP::Client::end()
{
  if (m_sock == NULL) return (false);
  m_sock->close();
  m_sock = NULL;
  return (true);
}

int
HTTP::Client::get(const char* url, uint32_t ms)
{
  if (m_sock == NULL) return (-1);
  const uint8_t PREFIX_MAX = 7;
  uint16_t port = 80;
  char hostname[HOSTNAME_MAX];
  uint32_t start;
  uint8_t i;
  int res;
  char c;

  // Parse given url for hostname
  if (memcmp_P(url, PSTR("http://"), PREFIX_MAX) == 0) url += PREFIX_MAX;
  i = 0; 
  while (1) {
    c = *url;
    hostname[i] = c;
    if (c == 0) break;
    url += 1;
    if ((c == '/') || (c == ':')) break;
    i += 1;
    if (i == sizeof(hostname)) return (-2);
  }
  if (c != 0) hostname[i] = 0;

  // Parse url for port number
  if (c == ':') {
    char num[16];
    i = 0;
    while (isdigit(c = *url)) {
      url += 1;
      num[i++] = c;
      if (i == sizeof(num)) return (-2);
    }
    if (i == 0) return (-2);
    num[i] = 0;
    port = atoi(num);
    if (c == '/') url += 1;
  }

  // Connect to the server
  res = m_sock->connect((const char*) hostname, port);
  if (res != 0) goto error;
  while ((res = m_sock->isconnected()) == 0) Watchdog::delay(16);
  if (res == 0) res = -3;
  if (res < 0) goto error;
  
  // Send a HTTP request
  m_sock->puts_P(PSTR("GET /"));
  m_sock->puts(url);
  m_sock->puts_P(PSTR(" HTTP/1.1" CRLF "Host: "));
  m_sock->puts(hostname);
  m_sock->puts_P(PSTR(CRLF "Connection: close" CRLF CRLF));
  m_sock->flush();
  
  // Wait for the response
  start = Watchdog::millis();
  while (((res = m_sock->available()) == 0) &&
	 ((ms == 0L) || (Watchdog::millis() - start < ms)))
    Watchdog::delay(16);
  if (res == 0) res = -4;
  if (res < 0) goto error;
  on_response(hostname, url);
  res = 0;

  // Close the connect and reopen for additional get
 error:
  m_sock->disconnect();
  m_sock->close();
  m_sock->open(Socket::TCP, 0, 0);
  return (res);
}

