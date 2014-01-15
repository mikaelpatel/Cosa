/**
 * @file Cosa/Socket.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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

#ifndef __COSA_SOCKET_HH__
#define __COSA_SOCKET_HH__

#include "Cosa/Types.h"

/**
 * Abstract Interface for Internet Sockets.
 */
class Socket {
public:
  /** Dynamic, private or ephemeral start ports number */
  static const uint16_t DYNAMIC_PORT = 49152;

  /** Protocol */
  enum Protocol {
    TCP = 0x01,
    UDP = 0x02,
    IPRAW = 0x03,
    MACRAW = 0x04,
    PPPoE = 0x05
  } __attribute__((packed));

  /**
   * Check if the given address is illegal (0.0.0.0/255.255.255.255:0)
   * @return true if illegal otherwise false.
   */
  bool is_illegal(uint8_t addr[4], uint16_t port);

  /**
   * @override Socket
   * Initiate socket to the given protocol and possible port.
   * @param[in] proto protocol.
   * @param[in] port source port.
   * @param[in] flag socket options.
   * @return zero if successful otherwise negative error code.
   */
  virtual int open(Protocol proto, uint16_t port, uint8_t flag) = 0;

  /**
   * @override Socket
   * Close the socket. 
   * @param[in] proto protocol.
   * @param[in] port source port.
   * @param[in] flag socket options.
   * @return zero if successful otherwise negative error code.
   */
  virtual int close() = 0;

  /**
   * @override Socket
   * Mark socket for incoming requests; server mode.
   * @return zero if successful otherwise negative error code.
   */
  virtual int listen() = 0;

  /**
   * @override Socket
   * Check for incoming requests from clients. Return zero if 
   * the socket has accepted a request and a connection is
   * established. 
   * @return zero if successful otherwise negative error code.
   */
  virtual int accept() = 0;
  
  /**
   * @override Socket
   * Connect the socket to the given address and port; client mode. 
   * @param[in] addr destination address.
   * @param[in] port destination port.
   * @return zero if successful otherwise negative error code.
   */
  virtual int connect(uint8_t addr[4], uint16_t port) = 0;

  /**
   * @override Socket
   * Disconnect socket from server.
   * @return zero if successful otherwise negative error code.
   */
  virtual int disconnect() = 0;

  /**
   * @override Socket
   * Send given data in buffer on connection-oriented socket. Return
   * number of bytes or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes sent if successful otherwise negative error code. 
   */
  virtual int send(const void* buf, size_t len) = 0;

  /**
   * @override Socket
   * Receive data from connection-oriented socket. The data is stored
   * in given buffer with given maximum number of bytes. Return number of
   * bytes or negative error code. 
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes sent if successful otherwise negative error code. 
   */
  virtual int recv(void* buf, size_t len) = 0;
  
  /**
   * @override Socket
   * Send given data in buffer on connectionless socket as a datagram
   * to given destination address (dest:port). Return number of bytes
   * sent or negative error code. 
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] dest destination address.
   * @param[in] port destination port.
   * @return number of bytes sent if successful otherwise negative error code. 
   */
  virtual int send(const void* buf, size_t len, uint8_t dest[4], uint16_t port) = 0;
  
  /**
   * @override Socket
   * Receive datagram on connectionless socket into given buffer with
   * given maximum size. Returns zero(0) if successful with
   * information in Datagram otherwise negative error code. 
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] src source address.
   * @param[in] port source port.
   * @return number of bytes received if successful otherwise negative error code. 
   */
  virtual int recv(void* buf, size_t len, uint8_t src[4], uint16_t& port) = 0;
};
#endif
