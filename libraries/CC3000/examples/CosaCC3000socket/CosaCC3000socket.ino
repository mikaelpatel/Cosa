/**
 * @file CosaCC3000socket.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * @section Description
 * Demonstrate CC3000 Wifi device driver; Cosa Socket level access.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <CC3000.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

#if defined(WICKEDDEVICE_WILDFIRE)
CC3000 wifi(Board::D21, Board::EXT2, Board::D23);
#else
CC3000 wifi(Board::D3, Board::EXT1, Board::D4);
#endif

#define CRLF "\r\n"

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCC3000socket: started"));
  Watchdog::begin();
  RTT::begin();
  wifi.begin_P(PSTR("CosaCC3300socket"));
}

void loop()
{
  const size_t BUF_MAX = 512;
  char buf[BUF_MAX];
  char msg[] =
    "GET / HTTP/1.1" CRLF
    "Connection: close" CRLF
    CRLF;
  Socket* socket = NULL;
  int count;
  int res;

  MEASURE("Create a socket:", 1)
    socket = wifi.socket(Socket::TCP);
  ASSERT(socket != NULL);

  uint8_t WWW_GOOGLE_COM[4] = { 173, 194, 113, 146 };
  do {
    MEASURE("Connect to server:", 1)
      res = socket->connect(WWW_GOOGLE_COM, 80);
    if (res < 0) wifi.service(1000);
  } while (res < 0);

  MEASURE("Send request:", 1)
    count = socket->send(msg, strlen(msg));
  ASSERT((size_t) count == strlen(msg));
  TRACE(count);

  count = 0;
  while (1) {
    MEASURE("Check if data is available:", 1)
      res = socket->available();
    ASSERT(res >= 0);
    if (res == 0) break;
    MEASURE("Receive HTML page buffer:", 1)
      res = socket->recv(buf, sizeof(buf));
    ASSERT(res >= 0);
    count += res;
#if defined(TRACE_RECV)
    for (int i = 0; i < res; i++)
      trace << (char) buf[i];
#endif
  }
#if defined(TRACE_RECV)
  trace << endl;
#endif
  TRACE(count);

  MEASURE("Close socket:", 1)
    res = socket->close();
  TRACE(res);
  trace << endl;

  wifi.service(10000);
}

