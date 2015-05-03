/**
 * @file INET/DNS.hh
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

#ifndef COSA_INET_DNS_HH
#define COSA_INET_DNS_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

/**
 * Domain Name Server request handler. Allows mapping from symbolic
 * human readable names in dot notation to network addresses.
 */
class DNS {
public:
  /** DNS standard port number. */
  static const uint16_t PORT = 53;

  /**
   * Construct DNS request handler. Use begin() to initiate the
   * handler and end() to terminate.
   */
  DNS() {}

  /**
   * Construct DNS request handler and initiate with given UDP socket and
   * server address. The destructor will automaically close the socket.
   * @param[in] sock socket.
   * @param[in] server network address.
   */
  DNS(Socket* sock, uint8_t server[4])
  {
    begin(sock, server);
  }

  /**
   * Destruct the DNS request handler; close the socket.
   */
  ~DNS()
  {
    end();
  }

  /**
   * Initiate the DNS request handler with given UDP socket and server
   * address. The destructor will automaically close the
   * socket. Returns true if successful otherwise false.
   * @param[in] sock socket.
   * @param[in] server network address.
   */
  bool begin(Socket* sock, uint8_t server[4]);

  /**
   * Terminate the DNS request handler and close the socket. Returns
   * true if successful otherwise false.
   */
  bool end();

  /**
   * Lookup the given hostname and return the network address. Returns
   * zero if successful otherwise negative error code.
   * @param[in] hostname to lookup.
   * @param[in] ip network address.
   * @return zero if successful otherwise negative error code.
   */
  int gethostbyname(const char* hostname, uint8_t ip[4])
    __attribute__((always_inline))
  {
    return (gethostbyname(hostname, ip, false));
  }

  /**
   * Lookup the given hostname and return the network address. Returns
   * zero if successful otherwise negative error code.
   * @param[in] hostname to lookup (in program memory).
   * @param[in] ip network address.
   * @return zero if successful otherwise negative error code.
   */
  int gethostbyname_P(str_P hostname, uint8_t ip[4])
    __attribute__((always_inline))
  {
    return (gethostbyname((const char*) hostname, ip, true));
  }

private:
  /**
   * Header Flags and Codes (little-endian).
   */
  enum {
    QUERY_FLAG = (0),
    RESPONSE_FLAG = (1<<15),
    QUERY_RESPONSE_MASK = (1<<15),
    OPCODE_STANDARD_QUERY = (0),
    OPCODE_INVERSE_QUERY = (1<<11),
    OPCODE_STATUS_REQUEST = (2<<11),
    OPCODE_MASK = (15<<11),
    AUTHORITATIVE_FLAG = (1<<10),
    TRUNCATION_FLAG = (1<<9),
    RECURSION_DESIRED_FLAG = (1<<8),
    RECURSION_AVAILABLE_FLAG = (1<<7),
    RESP_NO_ERROR = (0),
    RESP_FORMAT_ERROR = (1),
    RESP_SERVER_FAILURE = (2),
    RESP_NAME_ERROR = (3),
    RESP_NOT_IMPLEMENTED = (4),
    RESP_REFUSED = (5),
    RESP_MASK = (15),
    TYPE_A = (0x0001),
    CLASS_IN = (0x0001),
    LABEL_COMPRESSION_MASK = (0xC0)
  };

  /** Request/Response header. */
  struct header_t {
    uint16_t ID;		//!< Identifier.
    uint16_t FC;		//!< Flags and Codes.
    uint16_t QC;		//!< Question Count.
    uint16_t ANC;		//!< Answer Record Count.
    uint16_t NSC;		//!< Name Server Count.
    uint16_t ARC;		//!< Additional Record Count.
  };

  /**
   * Question attributes.
   */
  struct attr_t {
    uint16_t TYPE;		//!< Type of data in record.
    uint16_t CLASS;		//!< Data class.
  };

  /**
   * Answer record.
   */
  struct rec_t {
    uint16_t TYPE;		//!< Type of data in record.
    uint16_t CLASS;		//!< Data class.
    uint32_t TTL;		//!< Time To Live; seconds cached.
    uint16_t RDL;		//!< Resource Data Length.
    uint8_t RD[];		//!< Resource Data.
  };

  static const uint16_t TIMEOUT = 300;
  static const uint8_t RETRY_MAX = 8;
  static const uint16_t ID = 0xC05AU;
  uint8_t m_server[4];
  Socket* m_sock;

  /**
   * Lookup the given hostname and return the network address. Returns
   * zero if successful otherwise negative error code.
   * @param[in] hostname to lookup.
   * @param[in] ip network address.
   * @param[in] progmem flag if hostname string in program memory.
   * @return zero if successful otherwise negative error code.
   */
  int gethostbyname(const char* hostname, uint8_t ip[4], bool progmem);
};
#endif
