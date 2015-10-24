/**
 * @file Cosa/INET.hh
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

#ifndef COSA_INET_HH
#define COSA_INET_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Communication domain.
 */
enum {
  AF_INET = 2
};

/**
 * Socket type which specifies the communication semantics.
 */
enum {
  SOCK_STREAM = 1,
  SOCK_DGRAM = 2,
  SOCK_RAW = 3,
  SOCK_RDM = 4,
  SOCK_SEQPACKET = 5
};

/**
 * Internet Protocol Number.
 * http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
 */
enum  {
  IPPROTO_IP = 0,
  IPPROTO_IPV4 = 0,
  IPPROTO_ICMP = 1,
  IPPROTO_TCP = 6,
  IPPROTO_UDP = 17,
  IPPROTO_IPV6 = 41,
  IPPROTO_NONE = 59,
  IPPROTO_RAW = 255
};

class Socket;

/**
 * Internet message passing support functions.
 */
class INET {
public:
  static const uint8_t PATH_MAX = 64;
  static const uint8_t MAC_MAX = 6;
  static const uint8_t IP_MAX = 4;

  /**
   * Full Internet address; MAC, IP and port.
   */
  struct addr_t {
    uint8_t mac[MAC_MAX];	//!< Hardware address.
    uint8_t ip[IP_MAX];		//!< Network address.
    uint16_t port;		//!< Service port.

    addr_t() :
      port(0)
    {
      memset(mac, 0, MAC_MAX);
      memset(ip, 0, IP_MAX);
    }
  };

  /**
   * Check if the given address is illegal (0.0.0.0/255.255.255.255:0).
   * @return true if illegal otherwise false.
   */
  static bool is_illegal(uint8_t addr[IP_MAX], uint16_t port);

  /**
   * Convert an address string in dot notation to binary form.
   * Returns zero if successful otherwise negative error code.
   * @param[in] addr address string.
   * @param[in,out] ip network address.
   * @param[in] progmem address string in program memory flag.
   * @return zero if successful otherwise negative error code.
   */
  static int aton(const char* addr, uint8_t ip[IP_MAX], bool progmem = false);

  /**
   * Convert an address string in dot notation to binary form.
   * Returns zero if successful otherwise negative error code.
   * @param[in] addr address string in program memory.
   * @param[in,out] ip network address.
   * @return zero if successful otherwise negative error code.
   */
  static int aton_P(const char* addr, uint8_t ip[IP_MAX])
    __attribute__((always_inline))
  {
    return (aton(addr, ip, true));
  }

  /**
   * Convert an address string in dot notation to a request path.
   * The given path buffer must be able to hold at least PATH_MAX
   * characters. Returns length of path is successful otherwise
   * negative error code.
   * @param[in] hostname address string.
   * @param[in,out] path generated path from hostname.
   * @param[in] progmem hostname address string in program memory flag.
   * @return length of path if successful otherwise negative error
   * code.
   */
  static int nametopath(const char* hostname, char* path, bool progmem = false);

  /**
   * Convert an address string in dot notation to a request path.
   * The given path buffer must be able to hold at least PATH_MAX
   * characters. Returns length of path is successful otherwise
   * negative error code.
   * @param[in] hostname address string in program memory.
   * @param[in,out] path generated path from hostname.
   * @return length of path if successful otherwise negative error
   * code.
   */
  static int nametopath_P(const char* hostname, char* path)
    __attribute__((always_inline))
  {
    return (nametopath(hostname, path, true));
  }

  /**
   * Print path in dot notation to given output stream.
   * @param[in] outs output stream.
   * @param[in] path to print.
   */
  static void print_path(IOStream& outs, const char* path);

  /**
   * Print machine network address hex-colon notation to given
   * output stream.
   * @param[in] outs output stream.
   * @param[in] mac machine address to print.
   */
  static void print_mac(IOStream& outs, const uint8_t mac[MAC_MAX]);

  /**
   * Print network address and port in extended dot notation to given
   * output stream.
   * @param[in] outs output stream.
   * @param[in] addr network address to print.
   * @param[in] port.
   */
  static void print_addr(IOStream& outs, const uint8_t addr[IP_MAX], uint16_t port = 0);

  /**
   * Calculate Internet Checksum for given buffer with given
   * number of bytes. The buffer should contain data in network
   * order (big-endian). Return check sum.
   * @param[in] buf pointer to buffer.
   * @param[in] count number of bytes.
   * @return checksum.
   */
  static uint16_t checksum(const void* buf, size_t count);

  /**
   * Server request handler. Should be sub-classed and the virtual
   * member function on_request() should be implemented to receive
   * client requests and send responses.
   */
  class Server {
  public:
    /**
     * Default server constructor. Must call begin() to initiate with
     * socket. Associate with given io-stream. The socket will be
     * bound as the io-stream device.
     * @param[in] ios associated io-stream.
     */
    Server(IOStream& ios) :
      m_ios(ios),
      m_connected(false)
    {}

    /**
     * Get server socket.
     * @return socket.
     */
    Socket* socket()
    {
      return ((Socket*) m_ios.device());
    }

    /**
     * Get client address, network address and port.
     * @param[out] addr network address.
     */
    void client(INET::addr_t& addr);

    /**
     * @override{INET::Server}
     * Start server with given socket. Initiates socket for incoming
     * connection-oriented requests (TCP/listen). Returns true if
     * successful otherwise false.
     * @param[in] sock server socket.
     * @return bool.
     */
    virtual bool begin(Socket* sock);

    /**
     * @override{INET::Server}
     * Run server; service incoming client connect requests or data.
     * Wait for at most given time period. Zero time period will give
     * blocking behavior. Returns zero if successful or negative error
     * code. The error code ETIME is returned on timeout.
     * @param[in] ms timeout period (milli-seconds, default BLOCK(0L)).
     * @return zero or negative error code.
     */
    virtual int run(uint32_t ms = 0L);

    /**
     * @override{INET::Server}
     * Stop server and close socket. Returns true if successful
     * otherwise false.
     * @return bool.
     */
    virtual bool end();

    /**
     * @override{INET::Server}
     * Application extension; Called when a client connect has been
     * accepted. Return true if application accepts otherwise false.
     * @param[in] ios iostream for response.
     * @return bool.
     */
    virtual bool on_accept(IOStream& ios)
    {
      UNUSED(ios);
      return (true);
    }

    /**
     * @override{INET::Server}
     * Application extension; Called when a client connect has
     * been accepted.
     * @param[in] ios iostream for response.
     */
    virtual void on_connect(IOStream& ios)
    {
      UNUSED(ios);
    }

    /**
     * @override{INET::Server}
     * Application extension; Should implement the response to the
     * incoming request. Called with there is available data.
     * @param[in] ios iostream for request and response.
     */
    virtual void on_request(IOStream& ios) = 0;

    /**
     * @override{INET::Server}
     * Application extension; Called when a client disconnects.
     */
    virtual void on_disconnect() {}

  protected:
    /** Associated io-stream */
    IOStream& m_ios;

    /** State variable; listening/disconnect(false), connected(true). */
    bool m_connected;
  };
};

#endif
