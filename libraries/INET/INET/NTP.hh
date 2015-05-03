/**
 * @file Cosa/INET/NTP.hh
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

#ifndef COSA_INET_NTP_HH
#define COSA_INET_NTP_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"
#include "Cosa/Time.hh"

/**
 * Network Time Protocol client; access time from server on network.
 * Used for clock synchronization between computer systems over
 * packet-switched, variable-latency data networks. Note: this
 * implementation does not adjust for clock drift or network latency.
 */
class NTP {
public:
  /**
   * Construct NTP client with given socket, server name and time zone
   * adjustment.
   * @param[in] sock communications socket (UDP::PORT).
   * @param[in] server network address.
   * @param[in] zone time zone adjustment (Default GMT).
   */
  NTP(Socket* sock, uint8_t server[4], int8_t zone = 0);

  /**
   * Destruct NTP client. Close socket.
   */
  ~NTP();

  /**
   * Get current time as seconds from NTP Epoch. Returns zero if fails.
   * @return clock.
   */
  clock_t time();

  /**
   * Get current time as year, month, hours, minutes and seconds.
   * @param[out] time structure.
   * @return zero if successful otherwise negative error code.
   */
  int gettimeofday(time_t& time);

private:
  /** NTP server port. */
  static const uint16_t PORT = 123;

  /** Timeout period for response from time server (milli-seconds). */
  static const uint16_t TIMEOUT = 32;

  /** Network address of server. */
  uint8_t m_server[4];

  /** Socket for communication with server. */
  Socket* m_sock;

  /** Time zone adjustment (hours). */
  int8_t m_zone;
};

#endif
