/**
 * @file CosaCC3000server.ino
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
 * Demonstrate CC3000 Wifi device driver; simple web server.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <CC3000.h>

#include "Cosa/Memory.h"
#include "Cosa/InputPin.hh"
#include "Cosa/Clock.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

RTT::Clock clock;

#if defined(WICKEDDEVICE_WILDFIRE)
CC3000 wifi(Board::D21, Board::EXT2, Board::D23);
#else
CC3000 wifi(Board::D3, Board::EXT1, Board::D4);
#endif

#define CRLF "\r\n"

#define FD_ZERO() 0UL
#define FD_ISSET(fd,set) (((1UL << (fd)) & set) != 0UL)
#define FD_SET(fd,set) set = (1UL << (fd)) | set

int server;

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCC3000server: started"));
  Watchdog::begin();
  RTT::begin();

  ASSERT(wifi.begin_P(PSTR("CosaCC3300server")));

  uint8_t subnet[4];
  uint8_t ip[4];
  wifi.addr(ip, subnet);

  trace << "IP="; INET::print_addr(trace, ip);
  trace << ",SUBNET="; INET::print_addr(trace, subnet);
  trace << endl;

  MEASURE("Create a socket:", 1)
    server = wifi.socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ASSERT(server == 0);

  MEASURE("Bind to port:", 1)
    ASSERT(wifi.bind(server, 80) == 0);

  MEASURE("Listen mode:", 1)
    ASSERT(wifi.listen(server) == 0);
}

void loop()
{
  const size_t BUF_MAX = 64;
  static uint16_t requests = 0;
  static uint16_t errs = 0;
  static uint16_t closerrs = 0;
  uint32_t readhndls = FD_ZERO();
  uint32_t writehndls = FD_ZERO();
  uint32_t errorhndls = FD_ZERO();
  uint32_t count = 0;
  int recvs = 0;
  uint8_t ip[4];
  int client;
  int port;
  int res;

  client = wifi.accept(server, ip, port);
  if (client < 0) return;

  trace << PSTR("client=") << client << ",IP=";
  INET::print_addr(trace, ip);
  trace << PSTR(":") << (uint16_t) port << endl;

  FD_SET(client, readhndls);
  FD_SET(client, errorhndls);

  MEASURE("Recieve HTML request:", 1) {
    res = wifi.select(client + 1, readhndls, writehndls, errorhndls, 0, 100000);
    while ((res > 0) && (FD_ISSET(client,readhndls))) {
      char buf[BUF_MAX];
      res = wifi.recv(client, buf, sizeof(buf));
      recvs++;
      if (res < 0) break;
      count += res;
#if defined(TRACE_RECV)
      for (int i = 0; i < res; i++)
	trace << (char) buf[i];
#endif
      res = wifi.select(client + 1, readhndls, writehndls, errorhndls, 0, 5000);
    }
#if defined(TRACE_RECV)
    trace << endl;
#endif
  }

  // Reply page; header and footer are static, contents dynamic
  static const char header[] __PROGMEM =
    "HTTP/1.1 200 OK" CRLF
    "Content-Type: text/html" CRLF
    "Connection: close" CRLF
    "Refresh: 3" CRLF CRLF
    "<!DOCTYPE HTML>" CRLF
    "<HTML>" CRLF
    "<HEAD><TITLE>CosaCC3000server</TITLE></HEAD>" CRLF
    "<BODY>" CRLF;
  static const char footer[] __PROGMEM =
    "</BODY>" CRLF
    "</HTML>";
  static const char br[] __PROGMEM =
    "<BR/>" CRLF;
  str_P BR = (str_P) br;

  IOBuffer<1024> obuf;
  IOStream page(&obuf);
  if (count <= 0) goto error;
  TRACE(count);

  // Construct the page; header-contents-footer
  count = 0;
  MEASURE("Construct and send page:", 1) {
    page << (str_P) header;
    res = wifi.send(client, obuf, obuf.available());
    ASSERT(res == obuf.available());
    count += res;
    obuf.empty();

    // Digital pin status; table with green/red blocks and pin number
    page << PSTR("Digital Pin:") << BR;
    page << PSTR("<TABLE>") << endl;
    page << PSTR("<TR>") << endl;
    for (uint8_t i = 0; i < 14; i++) {
      page << PSTR("<TD>D") << i << PSTR("</TD>") << endl;
    }
    page << PSTR("</TR>") << endl;
    res = wifi.send(client, obuf, obuf.available());
    if (res < 0) goto error;
    ASSERT(res == obuf.available());
    count += res;
    obuf.empty();

    for (uint8_t ix = 0; ix < 14; ix++) {
    page << PSTR("<TR>") << endl;
    for (uint8_t i = 0; i < 14; i++) {
      Board::DigitalPin pin;
      pin = (Board::DigitalPin) pgm_read_byte(digital_pin_map + i);
      page << PSTR("<TD style=\"text-align: center; background-color: ");
      if (InputPin::read(pin))
	page << PSTR("red\">1");
      else
	page << PSTR("green\">0");
      page << PSTR("</TD>") << endl;
    }
    page << PSTR("</TR>") << endl;
    res = wifi.send(client, obuf, obuf.available());
    if (res != obuf.available()) goto error;
    count += res;
    obuf.empty();
    }
    page << PSTR("</TABLE>") << BR;
    page << BR;
    res = wifi.send(client, obuf, obuf.available());
    if (res != obuf.available()) goto error;
    count += res;
    obuf.empty();

    page << PSTR("Request: ") << ++requests << BR;
    page << PSTR("Send Errors: ") << errs << BR;
    page << PSTR("Close Errors: ") << closerrs << BR;
    page << PSTR("Time: ") << clock.time() << BR;
    page << (str_P) footer;
    res = wifi.send(client, obuf, obuf.available());
    if (res != obuf.available()) goto error;
    count += res;
  }
  TRACE(count);

 error:
  if (res < 0) {
    TRACE(res);
    wifi.service(1000);
  }
  errs += (res < 0);
  MEASURE("Close client connection:", 1)
    res = wifi.close(client);
  closerrs += (res < 0);
}
