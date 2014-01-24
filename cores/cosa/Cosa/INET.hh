/**
 * @file Cosa/INET.hh
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_INET_HH__
#define __COSA_INET_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Internet message passing support functions.
 */
class INET {
public:
  static const uint8_t PATH_MAX = 64;
  static const uint8_t MAC_MAX = 6;
  static const uint8_t IP_MAX = 4;

  /**
   * Full Internet address; MAC, IP and port
   */
  struct addr_t {
    uint8_t mac[MAC_MAX];	//!< hardware address
    uint8_t ip[IP_MAX];		//!< network address
    uint16_t port;		//!< service port
  };

  /**
   * Check if the given address is illegal (0.0.0.0/255.255.255.255:0)
   * @return true if illegal otherwise false.
   */
  static bool is_illegal(uint8_t addr[IP_MAX], uint16_t port);

  /**
   * Convert an address string in dot notation to binary form.
   * Returns zero if successful otherwise negative error code.
   * @param[in] addr address string in program memory.
   * @param[inout] ip network address.
   * @return zero if successful otherwise negative error code. 
   */
  static int aton_P(const char* addr, uint8_t ip[IP_MAX]);

  /**
   * Convert an address string in dot notation to a request path. 
   * The given path buffer must be able to hold at least PATH_MAX 
   * characters. Returns length of path is successful otherwise
   * negative error code.
   * @param[in] hostname address string in program memory.
   * @param[inout] path generated path from hostname.
   * @return length of path if successful otherwise negative error
   * code.  
   */
  static int nametopath_P(const char* hostname, char* path);

  /**
   * Print path in dot notation to given output stream.
   * @param[in] outs output stream.
   * @param[in] path to print.
   */
  static void print_path(IOStream& outs, const char* path);

  /**
   * Print network address and port in extended dot notation to given
   * output stream. 
   * @param[in] outs output stream.
   * @param[in] addr network address to print.
   * @param[in] port.
   */
  static void print_addr(IOStream& outs, const uint8_t addr[IP_MAX], uint16_t port = 0);
};

#endif
