/**
 * @file Cosa/Socket.hh
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

#ifndef COSA_SOCKET_HH
#define COSA_SOCKET_HH

#include "Cosa/Types.h"
#include "Cosa/INET.hh"
#include "Cosa/IOStream.hh"

/**
 * Abstract Interface for Internet Sockets.
 */
class Socket : public IOStream::Device {
public:
  /** First dynamic, private or ephemeral port number. */
  static const uint16_t DYNAMIC_PORT = 49152;

  /** Socket type. */
  enum Protocol {
    TCP = SOCK_STREAM,
    UDP = SOCK_DGRAM,
    IPRAW = SOCK_RAW,
    MACRAW = SOCK_RDM,
    PPPoE = SOCK_SEQPACKET
  } __attribute__((packed));

  /**
   * Socket constructor. Initial state of socket.
   */
  Socket() :
    IOStream::Device(),
    m_src(),
    m_proto(0),
    m_port(0),
    m_flags(0),
    m_server(false)
  {
  }

  /**
   * Get source machine address, network address and port.
   * @param[out] addr network address.
   */
  void src(INET::addr_t& addr) const
  {
    addr = m_src;
  }

  /**
   * Get socket protocol.
   * @return protocol.
   */
  Protocol proto() const
  {
    return ((Protocol) m_proto);
  }

  /**
   * Get socket port.
   * @return port.
   */
  uint16_t port() const
  {
    return (m_port);
  }

  /**
   * Get socket flags.
   * @return flags.
   */
  uint8_t flags() const
  {
    return (m_flags);
  }

  /** Overloaded virtual member function write. */
  using IOStream::Device::write;

  /**
   * @override{IOStream::Device}
   * Write data from buffer with given size to device.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write(const void* buf, size_t size)
  {
    return (write(buf, size, false));
  }

  /**
   * @override{IOStream::Device}
   * Write data from buffer in program memory with given size to device.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write_P(const void* buf, size_t size)
  {
    return (write(buf, size, true));
  }

  /**
   * @override{IOStream::Device}
   * Read character from device.
   * @return character or EOF(-1).
   */
  virtual int getchar()
  {
    char c;
    int res = recv(&c, sizeof(c));
    return (res == sizeof(c) ? (c & 0xff) : res);
  }

  /** Overloaded virtual member function read. */
  using IOStream::Device::read;

  /**
   * @override{IOStream::Device}
   * Read data to given buffer with given size from device.
   * @param[in] buf buffer to read into.
   * @param[in] size number of bytes to read.
   * @return number of bytes read or EOF(-1).
   */
  virtual int read(void* buf, size_t size)
  {
    return (recv(buf, size));
  }

  /**
   * @override{Socket}
   * Initiate socket to the given protocol and possible port.
   * @param[in] proto protocol.
   * @param[in] port source port.
   * @param[in] flags socket options.
   * @return zero if successful otherwise negative error code.
   */
  virtual int open(Protocol proto, uint16_t port, uint8_t flags) = 0;

  /**
   * @override{Socket}
   * Close the socket.
   * @return zero if successful otherwise negative error code.
   */
  virtual int close() = 0;

  /**
   * @override{Socket}
   * Mark socket for incoming requests; server mode.
   * @return zero if successful otherwise negative error code.
   */
  virtual int listen() = 0;

  /**
   * @override{Socket}
   * Check for incoming requests from clients. Return zero if
   * the socket has accepted a request and a connection is
   * established.
   * @return zero if successful otherwise negative error code.
   */
  virtual int accept() = 0;

  /**
   * @override{Socket}
   * Connect the socket to the given address and port; client mode.
   * @param[in] addr destination address.
   * @param[in] port destination port.
   * @return zero if successful otherwise negative error code.
   */
  virtual int connect(uint8_t addr[4], uint16_t port) = 0;

  /**
   * @override{Socket}
   * Connect the socket to the given hostname and port; client mode.
   * Returns zero if connection established otherwise negative error code.
   * @param[in] hostname string.
   * @param[in] port destination port.
   * @return zero if successful otherwise negative error code.
   */
  virtual int connect(const char* hostname, uint16_t port) = 0;

  /**
   * @override{Socket}
   * Returns positive integer if a connection is established, zero is
   * not yet established, otherwise a negative error code.
   * @return positive integer connected, zero if not otherwise
   * negative error code.
   */
  virtual int is_connected() = 0;

  /**
   * @override{Socket}
   * Disconnect socket from server.
   * @return zero if successful otherwise negative error code.
   */
  virtual int disconnect() = 0;

  /**
   * @override{Socket}
   * Start the construction of a datagram to the given address and
   * port.
   * @param[in] addr destination address.
   * @param[in] port destination port.
   * @return zero if successful otherwise negative error code.
   */
  virtual int datagram(uint8_t addr[4], uint16_t port) = 0;

  /**
   * Send given data in buffer on connection-oriented socket. Return
   * number of bytes or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes sent if successful otherwise negative error code.
   */
  int send(const void* buf, size_t len)
  {
    return (send(buf, len, false));
  }

  /**
   * @override{Socket}
   * Send given data in program memory buffer on connection-oriented
   * socket. Return number of bytes or negative error code.
   * @param[in] buf program memory pointer.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes sent if successful otherwise negative
   * error code.
   */
  int send_P(const void* buf, size_t len)
  {
    return (send(buf, len, true));
  }

  /**
   * @override{Socket}
   * Receive data from connection-oriented socket. The data is stored
   * in given buffer with given maximum number of bytes. Return number of
   * bytes or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes sent if successful otherwise negative
   * error code.
   */
  virtual int recv(void* buf, size_t len) = 0;

  /**
   * Send given data in buffer on connectionless socket as a datagram
   * to given destination address (dest:port). Return number of bytes
   * sent or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] dest destination address.
   * @param[in] port destination port.
   * @return number of bytes sent if successful otherwise negative
   * error code.
   */
  int send(const void* buf, size_t len, uint8_t dest[4], uint16_t port)
  {
    return (send(buf, len, dest, port, false));
  }

  /**
   * Send given data in program memory buffer on connectionless socket
   * as a datagram to given destination address (dest:port). Return
   * number of bytes
   * sent or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] dest destination address.
   * @param[in] port destination port.
   * @return number of bytes sent if successful otherwise negative
   * error code.
   */
  int send_P(const void* buf, size_t len, uint8_t dest[4], uint16_t port)
  {
    return (send(buf, len, dest, port, true));
  }

  /**
   * @override{Socket}
   * Receive datagram on connectionless socket into given buffer with
   * given maximum size. Returns zero(0) if successful with
   * information in Datagram otherwise negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] src source address.
   * @param[in] port source port.
   * @return number of bytes received if successful otherwise negative
   * error code.
   */
  virtual int recv(void* buf, size_t len, uint8_t src[4], uint16_t& port) = 0;

protected:
  /** Source address; MAC, IP and port. */
  INET::addr_t m_src;

  /** Socket protocol if open otherwise zero(0). */
  uint8_t m_proto;

  /** Socket port */
  uint16_t m_port;

  /** Socket flags */
  uint8_t m_flags;

  /** Socket is server (listen has been called) */
  bool m_server;

  /**
   * @override{Socket}
   * Write data from buffer with given size to device. Boolean flag
   * progmem defined if the buffer is in program memory. Return number
   * of bytes or negative error code.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @param[in] progmem program memory pointer flag.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write(const void* buf, size_t size, bool progmem)
  {
    return (send(buf, size, progmem));
  }

  /**
   * @override{Socket}
   * Send given data in buffer on connection-oriented socket. Boolean flag
   * progmem defined if the buffer is in program memory. Return number
   * of bytes or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] progmem program memory pointer flag.
   * @return number of bytes sent if successful otherwise negative
   * error code.
   */
  virtual int send(const void* buf, size_t len, bool progmem) = 0;

  /**
   * @override{Socket}
   * Send given data on connectionless socket as a datagram to given
   * destination address (dest:port). Return number of bytes
   * sent or negative error code.
   * @param[in] buf buffer pointer.
   * @param[in] len number of bytes in buffer.
   * @param[in] dest destination address.
   * @param[in] port destination port.
   * @param[in] progmem program memory pointer flag.
   * @return number of bytes sent if successful otherwise negative
   * error code.
   */
  virtual int send(const void* buf, size_t len,
		   uint8_t dest[4], uint16_t port,
		   bool progmem) = 0;
};
#endif
