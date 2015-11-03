/**
 * @file DHCP.hh
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

#ifndef COSA_DHCP_HH
#define COSA_DHCP_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

/**
 * Dynamic Host Configuration Protocol. Supports dynamic assignment of
 * network address. Used with connection-less socket to configure a
 * client with network address and subnet mask. Also provides network
 * addresses for DHCP and DNS server.
 */
class DHCP {
public:
  /** DHCP Client port numbers. */
  static const uint16_t PORT = 68;

  /**
   * Construct DHCP client access with given hostname and hardware
   * address.
   * @param[in] hostname string in program memory.
   * @param[in] mac hardware address in program memory.
   */
  DHCP(const char* hostname, const uint8_t* mac);

  /**
   * Start interaction with DHCP server. Provide UDP socket with DHCP
   * client port. Returns true if successful otherwise false.
   * @param[in] sock connection-less socket (UDP/DHCP::PORT).
   * @return bool, true if successful otherwise false.
   */
  bool begin(Socket* sock);

  /**
   * Stop interaction with DHCP server. Closes socket. Maintains state
   * so that the client network address lease may be renewed. Returns
   * true if successful otherwise false.
   */
  bool end();

  /**
   * Discover DHCP servers and receive client network address
   * offer. Return zero if successful otherwise a negative error code;
   * -1 not initiated, -2 discover failed, -3 no response.
   * @return zero if successful otherwise a negative error code.
   */
  int discover();

  /**
   * Accept the offer provided by DHCP server on successful
   * discover(). Return zero if successful otherwise a negative error
   * code; -1 not initiated, -2 request failed, -3 no response.
   * Client network address and subnet mask are returned in
   * given reference parameters.
   * @param[in,out] ip granted network address.
   * @param[in,out] subnet mask.
   * @param[in,out] gateway network address.
   * @return zero if successful otherwise a negative error code.
   */
  int request(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4]);

  /**
   * Renew the granted network address lease from successful request().
   * Returns zero if successful otherwise a negative error code; -1
   * illegal state, -2 request failed, -3 no response.
   * @param[in] sock connection-less socket to use for release.
   * @return zero if successful otherwise a negative error code.
   */
  int renew(Socket* sock);

  /**
   * Release the granted network address lease from successful
   * request(). Returns zero if successful otherwise a negative error
   * code; -1 illegal state, -2 request failed, -3 no response.
   * The given socket is closed and the DHCP client handler is put in
   * idle state.
   * @param[in] sock connection-less socket to use for release.
   * @return zero if successful otherwise a negative error code.
   */
  int release(Socket* sock);

  /** Return time when lease was obtained. */
  uint32_t lease_obtained() const
  {
    return (m_lease_obtained);
  }

  /** Return time when lease will expire. */
  uint32_t lease_expires() const
  {
    return (m_lease_expires);
  }

  /** Return network address of DHCP server. */
  const uint8_t* dhcp_addr() const
  {
    return (m_dhcp);
  }

  /** Return network address of DNS server. */
  const uint8_t* dns_addr() const
  {
    return (m_dns);
  }

  /** Return network address of gateway (router). */
  const uint8_t* gateway_addr() const
  {
    return (m_gateway);
  }

private:
  /** DHCP message OP code. */
  enum {
    REQUEST = 1,
    REPLY = 2
  } __attribute__((packed));

  /** DHCP hardware type and address length. */
  enum {
    HTYPE_10MB = 1,
    HTYPE_100MB = 2,
    HLEN_ETHERNET = 6,
    FLAGS_BROADCAST = 0x8000
  };

  /** DHCP request/response header. */
  struct header_t {
    uint8_t OP;			//!< Message type.
    uint8_t HTYPE; 		//!< Hardware type.
    uint8_t HLEN;		//!< Hardware address length.
    uint8_t HOPS;		//!< Max. number of relay agents.
    uint32_t XID;		//!< Transaction identity (client).
    uint16_t SECS;		//!< Number of seconds elapsed since start.
    uint16_t FLAGS;		//!< Flags/Broadcast.
    uint8_t CIADDR[4];		//!< Client IP address (requested).
    uint8_t YIADDR[4];		//!< Your IP address (response).
    uint8_t SIADDR[4];		//!< Server IP address.
    uint8_t GIADDR[4];		//!< First relay agent IP address.
    uint8_t CHADDRB[16];	//!< Client hardware address.
#ifdef BOOTP_SECTION		//!< Legacy BOOTP section.
    uint8_t SNAME[64]; 		//!< Server host name.
    uint8_t FILE[128];		//!< Bootfile name.
#endif
  };

  /** DHCP option block start magic cookie. */
  static const uint32_t MAGIC_COOKIE = 0x63825363;

  /** DHCP options; subset. */
  enum {
    PAD_OPTION = 0,
    SUBNET_MASK = 1,
    TIMER_OFFSET = 2,
    ROUTERS_ON_SUBNET =	3,
    DNS_SERVER = 6,
    HOSTNAME = 12,
    DOMAIN_NAME = 15,
    REQUESTED_IP_ADDR = 50,
    IP_ADDR_LEASE_TIME = 51,
    MESSAGE_TYPE = 53,
    SERVER_IDENTIFIER = 54,
    PARAM_REQUEST = 55,
    T1_VALUE = 58,
    T2_VALUE = 59,
    CLIENT_IDENTIFIER = 61,
    END_OPTION = 255
  } __attribute__((packed));

  /** DHCP message type; MESSAGE_TYPE option. */
  enum {
    DHCP_DISCOVER = 1,
    DHCP_OFFER = 2,
    DHCP_REQUEST = 3,
    DHCP_DECLINE = 4,
    DHCP_ACK = 5,
    DHCP_NAK = 6,
    DHCP_RELEASE = 7,
    DHCP_INFORM = 8
  } __attribute__((packed));

  /** DHCP server address. */
  uint8_t m_dhcp[4];

  /** Router in network (DHCP option). */
  uint8_t m_gateway[4];

  /** DNS server address (DHCP option). */
  uint8_t m_dns[4];

  /** Network address (DHCP offer). */
  uint8_t m_ip[4];

  /** Subnet mask (DHCP option). */
  uint8_t m_subnet[4];

  /** Hostname. */
  const char* m_hostname;

  /** Hardware address. */
  const uint8_t* m_mac;

  /** UDP socket. */
  Socket* m_sock;

  /** Lease obtained. */
  uint32_t m_lease_obtained;

  /** Lease expires. */
  uint32_t m_lease_expires;

  /** DHCP Server port numbers. */
  static const uint16_t SERVER_PORT = 67;

  /**
   * Send request of given type. Return zero if successful otherwise
   * negative error code.
   * @param[in] type DHCP message type option.
   * @return zero if successful otherwise negative error code.
   */
  int send(uint8_t type);

  /**
   * Receive response of given type within the given time limit.
   * Return zero if successful otherwise negative error code; -1
   * message read error, -2 timeout, -3 illegal source port, -4
   * illegal message type, -5 illegal magic cookie.
   * @param[in] type DHCP message type option.
   * @return zero if successful otherwise negative error code.
   */
  int recv(uint8_t type, uint16_t ms = 2000);
};

#endif
