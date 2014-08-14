/**
 * @file Cosa/INET/Telnet.hh
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

#ifndef COSA_INET_TELNET_HH
#define COSA_INET_TELNET_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Socket.hh"

class Telnet {
public:
  /** The Telnet server standard port. */
  static const uint16_t PORT = 23;

  /**
   * Telnet server request handler. Should be sub-classed and the
   * virtual member function on_request() should be implemented to
   * receive client requests and send responses.
   */
  class Server : public IOStream {
  public:
    /** 
     * Default telnet server constructor. Must call begin() to 
     * initiate with socket. 
     */
    Server() : 
      IOStream(),
      m_connected(false)
    {
    }

    /**
     * Get telnet server socket.
     * @return socket.
     */
    Socket* get_socket()
    {
      return ((Socket*) get_device());
    }  

    /**
     * Get client address, network address and port.
     * @param[out] addr network address.
     */
    void get_client(INET::addr_t& addr)
    {
      Socket* sock = get_socket();
      if (sock == NULL) return;
      sock->get_src(addr);
    }  

    /**
     * Start server with given socket. Initiates socket for incoming
     * connection-oriented requests (TCP/listen). Returns true if
     * successful otherwise false.
     * @param[in] sock server socket.
     * @return bool.
     */
    bool begin(Socket* sock);

    /**
     * Run server; service incoming client connect requests or data.
     * Wait for at most given time period or block. Return zero if
     * successful or negative error code. The error code -2 is 
     * returned on timeout. 
     * @param[in] ms timeout period (milli-seconds, default BLOCK).
     * @return zero or negative error code.
     */
    int run(uint32_t ms = 0L);

    /**
     * Stop server and close socket. Returns true if successful
     * otherwise false.
     * @return bool.
     */
    bool end();

    /**
     * @override Telnet::Server
     * Application extension; Called when a client connect has been
     * accepted. Return true if accepted otherwise false.
     * @param[in] ios iostream for response.
     * @return bool.
     */
    virtual bool on_connect(IOStream& ios) 
    { 
      UNUSED(ios);
      return (true);
    }

    /**
     * @override Telnet::Server
     * Application extension; Should implement the response to the
     * given request. Called with there is available data.
     * @param[in] ios iostream for request and response.
     */
    virtual void on_request(IOStream& ios) = 0;

    /**
     * @override Telnet::Server
     * Application extension; Called when a client disconnects.
     */
    virtual void on_disconnect() 
    {
    }

  protected:
    /** State variable; listening/disconnect(false), connected(true). */
    bool m_connected;
  };
};

#endif
