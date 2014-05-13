/**
 * @file Cosa/INET/DHCP.cpp
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

#include "Cosa/INET/DHCP.hh"
#include "Cosa/Watchdog.hh"

DHCP::DHCP(const char* hostname, const uint8_t* mac) :
  m_hostname(hostname),
  m_mac(mac),
  m_sock(NULL),
  m_lease_obtained(0L),
  m_lease_expires(0L)
{
}

int 
DHCP::send(uint8_t type)
{
  // Start the construction of the message
  uint8_t BROADCAST[4] = { 0xff, 0xff, 0xff, 0xff };
  int res = m_sock->datagram(BROADCAST, SERVER_PORT);
  if (res < 0) return (-1);
  
  // Construct DHCP message header
  header_t header;
  memset(&header, 0, sizeof(header));
  header.OP = REQUEST;
  header.HTYPE = HTYPE_10MB;
  header.HLEN = HLEN_ETHERNET;
  header.XID = Watchdog::millis();
  header.SECS = 1;
  header.FLAGS = hton((int16_t) FLAGS_BROADCAST);
  memcpy_P(header.CHADDRB, m_mac, INET::MAC_MAX);
  res = m_sock->write(&header, sizeof(header));
  if (res < 0) return (-1);
  
  // Write BOOTP legacy (192 bytes zero)
  uint8_t buf[32];
  memset(buf, 0, sizeof(buf));
  for (uint8_t i = 0; i < 6; i++) m_sock->write(buf, sizeof(buf));

  // Write DHCP Magic Cookie
  uint32_t magic = hton((int32_t) MAGIC_COOKIE);
  res = m_sock->write(&magic, sizeof(magic));
  if (res < 0) return (-1);

  // Write DHCP options; message type, client hardware address and hostname
  uint8_t len = strlen_P(m_hostname) + 1;
  buf[0] = MESSAGE_TYPE;
  buf[1] = 1;
  buf[2] = type;
  buf[3] = CLIENT_IDENTIFIER;
  buf[4] = 7;
  buf[5] = 1;
  memcpy(&buf[6], m_mac, INET::MAC_MAX);
  buf[12] = HOSTNAME;
  buf[13] = len;
  memcpy_P(&buf[14], m_hostname, len);
  res = m_sock->write(buf, 14 + len);
  if (res < 0) return (-1);

  // On request add client and server address options
  if (type == DHCP_REQUEST) {
    buf[0] = REQUESTED_IP_ADDR;
    buf[1] = INET::IP_MAX;
    memcpy(&buf[2], m_ip, INET::IP_MAX);
    buf[6] = SERVER_IDENTIFIER;
    buf[7] = INET::IP_MAX;
    memcpy(&buf[8], m_dhcp, INET::IP_MAX);
    res = m_sock->write(buf, 8 + INET::IP_MAX);
    if (res < 0) return (-1);
  }

  // Parameter request and end option list
  static const uint8_t param[] __PROGMEM = {
    PARAM_REQUEST,
    6,
    SUBNET_MASK,
    ROUTERS_ON_SUBNET,
    DNS_SERVER,
    DOMAIN_NAME,
    IP_ADDR_LEASE_TIME,
    END_OPTION
  };
  res = m_sock->write_P(param, sizeof(param));
  if (res < 0) return (-1);
  return (m_sock->flush());
}

int 
DHCP::recv(uint8_t type, uint16_t ms)
{
  // Wait for a reply
  int res = 0;
  for (uint16_t i = 0; i < ms; i += 32) {
    if ((res = m_sock->available()) != 0) break;
    delay(32);
  }
  if (res == 0) return (-2);

  // Read response message
  header_t header;
  uint16_t port;
  res = m_sock->recv(&header, sizeof(header), m_dhcp, port);
  if (res <= 0) return (-1);
  // Fix: Should also check that the hardware address (broadcast)
  if (port != SERVER_PORT) return (-3);
  if (header.OP != REPLY) return (-4);
  memcpy(m_ip, header.YIADDR, sizeof(m_ip));

  // Skip legacy BOOTP parameters
  uint8_t buf[32];
  for (uint8_t i = 0; i < 6; i++) m_sock->read(buf, sizeof(buf));

  // Check Magic Cookie
  uint32_t magic;
  res = m_sock->read(&magic, sizeof(magic));
  if (res < 0) return (-1);
  magic = ntoh((int32_t) magic);
  if (magic != MAGIC_COOKIE) return (-5);

  // Parse options and collect; subnet mask, server addresses and lease time
  uint8_t op;
  uint8_t len;
  res = 0;
  while (m_sock->read(&op, sizeof(op)) == sizeof(op)) {
    if (op == END_OPTION) break;
    if (op == PAD_OPTION) continue;
    m_sock->read(&len, sizeof(len));
    m_sock->read(buf, len);
    switch (op) {
    case MESSAGE_TYPE: 
      if (buf[0] != type) res = -6;
      break;
    case SUBNET_MASK:
      memcpy(m_subnet, buf, sizeof(m_subnet));
      break;
    case DNS_SERVER:
      memcpy(m_dns, buf, sizeof(m_dns));
      break;
    case ROUTERS_ON_SUBNET:
      memcpy(m_gateway, buf, sizeof(m_gateway));
      break;
    case IP_ADDR_LEASE_TIME:
      m_lease_obtained = Watchdog::millis() / 1000;
      int32_t* expire_p = (int32_t*) buf;
      m_lease_expires = ntoh(*expire_p) + m_lease_obtained;
      break;
    };
  };

  // Flush any remains of the reply
  while (m_sock->available() > 0) m_sock->read(buf, sizeof(buf));
  return (res);
}

bool
DHCP::begin(Socket* sock)
{
  if (m_sock != NULL) return (false);
  m_sock = sock;
  return (true);
}

bool
DHCP::end()
{
  if (m_sock == NULL) return (false);
  m_sock->close();
  m_sock = NULL;
  return (true);
}

int 
DHCP::discover()
{
  if (m_sock == NULL) return (-1);
  if (send(DHCP_DISCOVER) < 0) return (-2);
  if (recv(DHCP_OFFER) < 0) return (-3);
  return (0);
}
  
int 
DHCP::request(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4])
{
  if (m_sock == NULL) return (-1);
  if (send(DHCP_REQUEST) < 0) return (-2);
  if (recv(DHCP_ACK) < 0) return (-3);
  memcpy(ip, m_ip, sizeof(m_ip));
  memcpy(subnet, m_subnet, sizeof(m_subnet));
  memcpy(gateway, m_gateway, sizeof(m_gateway));
  return (0);
}

int 
DHCP::renew(Socket* sock)
{
  if (m_sock != NULL) return (-1);
  if (m_lease_expires == 0L) return (-1);
  m_sock = sock;
  if (send(DHCP_REQUEST) < 0) return (-2);
  if (recv(DHCP_ACK) < 0) return (-3);
  m_sock->close();
  m_sock = NULL;
  return (0);
}

int 
DHCP::release(Socket* sock)
{
  if (m_sock != NULL) return (-1);
  m_sock = sock;
  if (send(DHCP_RELEASE) < 0) return (-2);
  if (recv(DHCP_ACK) < 0) return (-3);
  m_sock->close();
  m_sock = NULL;
  memset(m_ip, 0, sizeof(m_ip));
  m_lease_obtained = 0L;
  m_lease_expires = 0L;
  return (0);
}

