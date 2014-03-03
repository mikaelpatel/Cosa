/**
 * @file Cosa/IoT/ThingSpeak.hh
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

#ifndef __COSA_IOT_THINGSPEAK_HH__
#define __COSA_IOT_THINGSPEAK_HH__

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

/**
 * ThingSpeak client implementation.
 * 
 * @section Reference
 * 1. ThingSpeak API, http://community.thingspeak.com/documentation/api/
 */
class ThingSpeak {
public:
  class Channel;

  class Client {
    friend class Channel;
  public:
    /**
     * Construct Thingspeak client.
     */
    Client();

    /**
     * Destruct Thingspeak client. Close and release socket.
     */
    ~Client();

    /**
     * Start Thingspeak client with given socket. Returns true if
     * successful otherwise false.
     * @param[in] sock connection-oriented socket to use.
     * @return bool
     */
    bool begin(Socket* sock);

    /**
     * Stop Thingspeak client, close and release socket. Returns true
     * if successful (correct mode) otherwise false.
     * @return bool
     */
    bool end();
  private:
    Socket* m_sock;
  };

  class Channel {
  public:
    /**
     * Construct channel to given client and given write key.
     * @param[in] client Thingspeak client.
     * @param[in] key api write key for channel (in program memory).
     */
    Channel(Client* client, const char* key);

    /**
     * Post given field update to channel. The update string is in
     * the format "field1=value1&...&field8=value8". Number of fields
     * depend on the channel definition in Thingspeak. Returns zero if
     * successful otherwise negative error code.
     * @param[in] fields string with field-value update.
     * @return zero if successful otherwise negative error code.
     */
    int post(const char* fields);

  private:
    Client* m_client;
    const char* m_key;
  };
};

#endif
