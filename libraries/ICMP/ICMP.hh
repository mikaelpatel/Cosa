/**
 * @file ICMP.hh
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

#ifndef COSA_ICMP_HH
#define COSA_ICMP_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

/**
 * Internet Control Message Protocol client; ping network address.
 *
 * @section References
 * 1. RFC792, https://www.ietf.org/rfc/rfc792.txt
 * 2. Ping, http://en.wikipedia.org/wiki/Ping_%28networking_utility%29
 */
class ICMP {
public:
  /**
   * Construct ICMP client with given socket.
   * @param[in] sock communications socket (Socket::IPRAW).
   * @param[in] id request identity (Default 0xC05A).
   */
  ICMP(Socket* sock, uint16_t id = 0xC05A) :
    m_sock(sock),
    m_id(id),
    m_seq(0)
  {}

  /**
   * Destruct ICMP client. Close socket.
   */
  ~ICMP()
  {
    if (m_sock == NULL) return;
    m_sock->close();
  }

  /**
   * Ping given network address and wait at most given timeout period
   * in milli-seconds. Return roundtrip in milli-second or negative
   * error code.
   * @param[in] dest network address.
   * @param[in] timeout max number of milli-seconds to wait for reply.
   */
  int ping(uint8_t dest[4], uint16_t timeout = DEFAULT_TIMEOUT)
  {
    int res = ping_request(dest);
    if (res < 0) return (res);
    return (ping_await(timeout));
  }

  /**
   * Issue ICMP ECHO to given network address. Return zero if
   * successful otherwise a negative error code.
   * @param[in] dest network address.
   * @return zero or negative error code.
   */
  int ping_request(uint8_t dest[4]);

  /**
   * Await ICMP ECHOREPLY after previous ping_request(). Wait at most
   * given timeout period in milli-seconds. Return roundtrip in
   * milli-second or negative error code.
   * @param[in] timeout max number of milli-seconds to wait for reply.
   * @return roundtrip or negative error code.
   */
  int ping_await(uint16_t timeout = DEFAULT_TIMEOUT);

protected:
  /** Timeout period for response from destination machine (milli-seconds). */
  static const uint16_t DEFAULT_TIMEOUT = 250;

  /** Socket for communication. */
  Socket* m_sock;

  /** Request Identiy. */
  uint16_t m_id;

  /** Request Sequence number. */
  uint16_t m_seq;

  /**
   * ICMP header as defined in Linux (netinet/ip_icmp.h).
   * Note: extended with echo request timestamp.
   */
  struct header_t {
    uint8_t type;		//!< Message type.
    uint8_t code;		//!< Type sub-code.
    uint16_t checksum;		//!< Internet message checksum.

    union {
      struct {
	uint16_t id;		//!< Echo identity code.
	uint16_t seq;		//!< Echo sequence number.
      } echo;			//!< Echo datagram.

      uint32_t gateway;		//!< Gateway address.

      struct {
	uint16_t reserved;	//!< Reserved.
	uint16_t mtu;		//!< MTU code.
      } frag;			//!< Path MTU discovery.
    };

    uint32_t timestamp;		//!< Echo request timestamp (ms).
  };

  /**
   * Message types.
   */
  enum Type {
    ECHOREPLY = 0,	        //!< Echo Reply.
    DEST_UNREACH = 3,		//!< Destination Unreachable.
    SOURCE_QUENCH = 4,		//!< Source Quench.
    REDIRECT = 5,		//!< Redirect (change route).
    ECHO = 8,			//!< Echo Request.
    TIME_EXCEEDED = 11,		//!< Time Exceeded.
    PARAMETERPROB = 12,		//!< Parameter Problem.
    TIMESTAMP = 13,		//!< Timestamp Request.
    TIMESTAMPREPLY = 14,	//!< Timestamp Reply.
    INFO_REQUEST = 15,		//!< Information Request.
    INFO_REPLY = 16,		//!< Information Reply.
    ADDRESS = 17,		//!< Address Mask Request.
    ADDRESSREPLY = 18,		//!< Address Mask Reply.
    NR_ICMP_TYPES = 18
  };

  /**
   * Codes for UNREACH.
   */
  enum {
    NET_UNREACH = 0,		//!< Network Unreachable.
    HOST_UNREACH = 1,		//!< Host Unreachable.
    PROT_UNREACH = 2,		//!< Protocol Unreachable.
    PORT_UNREACH = 3,		//!< Port Unreachable.
    FRAG_NEEDED = 4,		//!< Fragmentation Needed/DF set.
    SR_FAILED = 5,	        //!< Source Route failed.
    NET_UNKNOWN = 6,
    HOST_UNKNOWN = 7,
    HOST_ISOLATED = 8,
    NET_ANO = 9,
    HOST_ANO = 10,
    NET_UNR_TOS = 11,
    HOST_UNR_TOS = 12,
    PKT_FILTERED = 13,		//!< Packet filtered.
    PREC_VIOLATION = 14,	//!< Precedence violation.
    PREC_CUTOFF = 15,		//!< Precedence cut off.
    NR_ICMP_UNREACH = 15
  };

  /*
   * Codes for REDIRECT.
   */
  enum {
    REDIR_NET =	0,	      	//!< Redirect Net.
    REDIR_HOST = 1,		//!< Redirect Host.
    REDIR_NETTOS = 2,		//!< Redirect Net for TOS.
    REDIR_HOSTTOS = 3,		//!< Redirect Host for TOS.
  };

  /*
   * Codes for TIME_EXCEEDED.
   */
  enum {
    EXC_TTL = 0,		//!< TTL count exceeded.
    EXC_FRAGTIME = 1		//!< Fragment Reass time exceeded.
  };
};

#endif
