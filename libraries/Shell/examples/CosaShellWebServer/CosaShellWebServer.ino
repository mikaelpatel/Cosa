/**
 * @file CosaShellWebServer.ino
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
 * @section Description
 * W5100 Ethernet Controller device driver example code; HTTP server.
 * Demonstration of integrating Cosa Shell with the Cosa HTTP server
 * support. Queries are translated to commands for the Shell demo.
 *
 * @section Circuit
 * This sketch is designed for the Ethernet Shield.
 * @code
 *                       W5100/ethernet
 *                       +------------+
 * (D10)--------------29-|CSN         |
 * (D11)--------------28-|MOSI        |
 * (D12)--------------27-|MISO        |
 * (D13)--------------30-|SCK         |
 * (D2)-----[ ]-------56-|IRQ         |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <Shell.h>
#include <DNS.h>
#include <DHCP.h>
#include <HTTP.h>
#include <W5100.h>

#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

#include "Commands.h"

// Handling of CRLF in HTML strings
#define CRLF "\r\n"

class WebServer : public HTTP::Server {
public:
  WebServer() {}
  virtual void on_request(IOStream& page, char* method, char* path, char* query);
};

void
WebServer::on_request(IOStream& page, char* method, char* path, char* query)
{
  // Reply page; header and footer are static, contents dynamic
  static const char header[] __PROGMEM =
    "HTTP/1.1 200 OK" CRLF
    "Content-Type: text/html" CRLF
    "Connection: close" CRLF CRLF
    "<!DOCTYPE HTML>" CRLF
    "<HTML>" CRLF
    "<HEAD><TITLE>CosaShellWebServer</TITLE></HEAD>" CRLF
    "<BODY>" CRLF
    "<PRE>" CRLF;
  static const char footer[] __PROGMEM =
    "</PRE>" CRLF
    "</BODY>" CRLF
    "</HTML>";
  UNUSED(method);
  UNUSED(path);
  int res = -1;
  page << (str_P) header;
  if (query != NULL) {
    char c;
    for (char* qp = query; (c = *qp) != 0; qp++)
      if (c == '&' || c == '+') *qp = ' ';
    res = shell.execute(query);
  }
  if (res != 0) page << PSTR("illegal query") << endl;
  page << (str_P) footer;
}

// Network configuration
#define IP 192,168,1,150
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,1,1
#define PORT 80

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
WebServer server;
Socket* sock = NULL;

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

void setup()
{
  // Initiate uart and trace output stream. And timers; watchdog and rtc
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaShellWebServer: started"));
  Watchdog::begin();
  RTC::begin();

  // Set up idle time capture function
  yield = iowait;

  // Initiate ethernet controller with address and start server
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));
  ASSERT((sock = ethernet.socket(Socket::TCP, PORT)) != NULL);
  ASSERT(server.begin(sock));

  // Bind the socket to the shell io-stream
  ios.set_device(sock);

  // Set shell privilege level; ADMIN
  shell.set_privilege(Shell::ADMIN);
}

void loop()
{
  // Service incoming requests
  server.run();
}
