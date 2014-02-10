/**
 * @file Cosa/INET/HTTP.hh
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

#ifndef __COSA_INET_HTTP_HH__
#define __COSA_INET_HTTP_HH__

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

class HTTP {
public:
  /**
   * HTTP server request handler. Should be sub-classed and the
   * virtual member function on_request() should be implemented to
   * produce response to HTTP requests.
   */
  class Server {
  public:
    /**
     * Start server with given socket. Initiates socket for incoming
     * connection-oriented requests (TCP/listen). Returns true if
     * successful otherwise false.
     * @param[in] sock server socket.
     * @return bool.
     */
    bool begin(Socket* sock);

    /**
     * Server loop function; wait for a request for the given time
     * period. Parse incoming requests from client and calls
     * on_request(). The socket is disconnected and reinitialized for
     * incoming requests after the processing the request. Returns
     * zero if successful otherwise a negative error code; 
     * timeout(-2) if the time period is exceeded 
     * @param[in] ms timeout period (milli-seconds, default BLOCK).
     * @return zero or negative error code.
     */
    int request(uint32_t ms = 0L);

    /**
     * Stop server and close socket. Returns true if successful
     * otherwise false.
     * @return bool.
     */
    bool end();

    /**
     * @override
     * Application extension; Should implement the response to the
     * given request (url).
     * @param[in] url request string. 
     */
    virtual void on_request(char* url) = 0;

  protected:
    /** Serviced socket */
    Socket* m_sock;
  };
};

#endif
