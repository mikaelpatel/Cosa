/**
 * @file CosaCC3000demo.ino
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
 * Demonstration of CC3000 Wifi device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <CC3000.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

// Enable printout of data received
#define TRACE_RECV

#if defined(WICKEDDEVICE_WILDFIRE)
CC3000 wifi(Board::D21, Board::EXT2, Board::D23);
#else
CC3000 wifi(Board::D3, Board::EXT1, Board::D4);
#endif

#define CRLF "\r\n"

#define FD_ZERO() 0UL
#define FD_ISSET(fd,set) (((1UL << (fd)) & set) != 0UL)
#define FD_SET(fd,set) set = (1UL << (fd)) | set

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCC3000demo: started"));
  Watchdog::begin();
  RTT::begin();
  ASSERT(wifi.begin_P(PSTR("CosaCC3300demo")));
}

void loop()
{
  const size_t BUF_MAX = 64;
  uint8_t WWW_GOOGLE_COM[4] = { 173, 194, 113, 146 };
  char msg[] =
    "GET / HTTP/1.1" CRLF
    "Connection: close" CRLF
    CRLF;
  int sock;
  uint32_t readhndls = FD_ZERO();
  uint32_t writehndls = FD_ZERO();
  uint32_t errorhndls = FD_ZERO();
  char buf[BUF_MAX];
  int res;

  MEASURE("Create a socket:", 1) {
    sock = wifi.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  }
  TRACE(sock);

  MEASURE("Connect to server:", 1) {
    res = wifi.connect(sock, WWW_GOOGLE_COM, 80);
  }
  if (res < 0) goto error;

  MEASURE("Send request:", 1) {
    res = wifi.send(sock, msg, strlen(msg));
  }
  if (res < 0) goto error;

  FD_SET(sock, readhndls);
  FD_SET(sock, errorhndls);
  MEASURE("Use select to wait for response:", 1) {
    res = wifi.select(sock + 1, readhndls, writehndls, errorhndls, 0, 50000);
  }
  if (res < 0) goto error;
  MEASURE("Receive HTML page:", 1) {
    while ((res > 0) && (FD_ISSET(sock,readhndls))) {
      res = wifi.recv(sock, buf, sizeof(buf));
      if (res < 0) break;
#if defined(TRACE_RECV)
      for (int i = 0; i < res; i++)
	trace << (char) buf[i];
#endif
      res = wifi.select(1, readhndls, writehndls, errorhndls, 0, 5000);
    }
  }
#if defined(TRACE_RECV)
  trace << endl;
#endif

 error:
  if (res < 0) TRACE(res);
  MEASURE("Close socket:", 1) {
    res = wifi.close(sock);
  }
  TRACE(res);
  trace << endl;

  wifi.service(3000);
  sleep(2);
}

