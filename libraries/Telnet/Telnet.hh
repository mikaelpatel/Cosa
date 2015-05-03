/**
 * @file Telnet.hh
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

#ifndef COSA_TELNET_HH
#define COSA_TELNET_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Socket.hh"

class Telnet {
public:
  /** The Telnet server standard port. */
  static const uint16_t PORT = 23;

  /**
   * Telnet server request handler. Should be sub-classed and the
   * virtual member function INET::Server::on_request() should be
   * implemented to receive client requests and send responses.
   */
  class Server : public INET::Server {
  public:
    /**
     * Default telnet server constructor. Must call begin() to
     * initiate with socket.
     * @param[in] ios associated io-stream.
     */
    Server(IOStream& ios) : INET::Server(ios) {}

    /**
     * @override INET::Server
     * Start server with given socket. Initiates socket for incoming
     * connection-oriented requests (TCP/listen). Set io-stream device
     * (socket) in no echo mode. Returns true if successful otherwise
     * false.
     * @param[in] sock server socket.
     * @return bool.
     */
    virtual bool begin(Socket* sock);

  protected:
    /**
     * @override INET::Server
     * Application extension; Called when a client connect has been
     * accepted. Handle flush of telnet terminal setting on connect.
     * Return true if accepted otherwise false.
     * @param[in] ios iostream for response.
     * @return bool.
     */
    virtual bool on_accept(IOStream& ios);
  };
};

#endif
