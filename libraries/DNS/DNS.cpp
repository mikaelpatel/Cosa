/**
 * @file INET/DNS.cpp
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

#include "DNS.hh"
#include "Cosa/INET.hh"
#include "Cosa/Errno.h"

bool
DNS::begin(Socket* sock, uint8_t server[4])
{
  memcpy(m_server, server, sizeof(m_server));
  m_sock = sock;
  return (sock != NULL);
}

bool
DNS::end()
{
  if (UNLIKELY(m_sock == NULL)) return (false);
  m_sock->close();
  m_sock = NULL;
  return (true);
}

int
DNS::gethostbyname(const char* hostname, uint8_t addr[4], bool progmem)
{
  if (UNLIKELY(m_sock == NULL)) return (ENOTSOCK);

  // Check if we already have a network address (as a string)
  if (INET::aton(hostname, addr, progmem) == 0) return (0);

  // Convert hostname to a path
  char path[INET::PATH_MAX];
  int len = INET::nametopath(hostname, path, progmem);
  if (UNLIKELY(len <= 0)) return (EFAULT);

  // Construct request header
  header_t request;
  request.ID = hton((int16_t) ID);
  request.FC = hton(QUERY_FLAG | OPCODE_STANDARD_QUERY | RECURSION_DESIRED_FLAG);
  request.QC = hton(1);
  request.ANC = 0;
  request.NSC = 0;
  request.ARC = 0;

  // And query attributes
  attr_t attr;
  attr.TYPE = hton(TYPE_A);
  attr.CLASS = hton(CLASS_IN);

  // Send request and wait for reply
  for (int8_t retry = 0; retry < RETRY_MAX; retry++) {
    m_sock->datagram(m_server, PORT);
    m_sock->write(&request, sizeof(request));
    m_sock->write(path, len);
    m_sock->write(&attr, sizeof(attr));
    m_sock->flush();

    // Wait for a reply
    int res;
    for (uint16_t i = 0; i < TIMEOUT; i += 32) {
      if ((res = m_sock->available()) != 0) break;
      delay(32);
    }
    if (res == 0) continue;

    // Receive the DNS response
    uint8_t response[128];
    uint8_t dest[4];
    uint16_t port;
    res = m_sock->recv(response, sizeof(response), dest, port);
    if (UNLIKELY(res <= 0)) continue;

    // The response header
    header_t* header = (header_t*) response;
    ntoh((int16_t*) header, (int16_t*) header, sizeof(header_t) / 2);
    if (header->ID != ID) continue;
    uint8_t* ptr = &response[sizeof(header_t)];

    // The query; Path and attributes
    uint8_t n;
    while ((n = *ptr++) != 0) ptr += n;
    ptr += sizeof(attr_t);

    // The answer; domain name, attributes and data (address)
    for (uint16_t i = 0; i < header->ANC; i++) {
      do {
	n = *ptr++;
	if ((n & LABEL_COMPRESSION_MASK) == 0) {
	  if ((n & 0x80) == 0) {
	    ptr += n;
	  }
	}
	else {
	  ptr += 1;
	  n = 0;
	}
      } while (n != 0);
      rec_t* rec = (rec_t*) ptr;
      ntoh((int16_t*) rec, (int16_t*) rec, sizeof(rec_t) / 2);
      ptr += sizeof(rec_t);
      ptr += rec->RDL;
      if (rec->TYPE != TYPE_A) continue;
      if (rec->CLASS != CLASS_IN) continue;
      if (rec->RDL != INET::IP_MAX) continue;
      memcpy(addr, rec->RD, INET::IP_MAX);
      return (0);
    }
  }
  return (EIO);
}
