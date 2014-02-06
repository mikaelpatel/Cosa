/**
 * @file CosaWebClient.ino
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
 * @section Description
 * W5100 Ethernet Controller device driver example code; HTTP client.
 * Send a HTTP GET request to server and print result page.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Network configuration
#define IP 192,168,0,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,0,1
#define FILESERVER 192,168,0,2
#define WIFIBRIDGE 192,168,0,30
#define UNDEFINED 74,125,232,128
#define WWW_GOOGLE_COM 173,194,71,99
#define DEST WWW_GOOGLE_COM
#define DEST WWW_GOOGLE_COM
#define PORT 80

// HTTP GET request configuration
#define ROOT ""
#define SEARCH "search?q=arduino"
#define URL ROOT

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWebClient: started"));
  Watchdog::begin();

  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));

  // Allocate a TCP socket and connect to server
  uint8_t dest[4] = { DEST };
  uint16_t port = PORT;
  Socket* sock;
  int res;
  ASSERT((sock = ethernet.socket(Socket::TCP)) != NULL);
  ASSERT(!sock->connect(dest, port));
  TRACE(sock->isconnected());
  while ((res = sock->isconnected()) == 0);
  ASSERT(sock->isconnected() > 0);
  
  // Send a HTTP request
  static const char request[] PROGMEM = "GET /" URL " HTTP/1.1\r\n\r\n\r\n";
  size_t len = strlen_P(request);
  TRACE(sock->room());
  ASSERT(sock->send_P(request, len) == len);
  
  // Wait for the reply
  while ((res = sock->available()) == 0);
  ASSERT(res > 0);
  TRACE(sock->available());

  // Receive the reply and print to output stream. Note buffer size
  char buf[32];
  while ((res = sock->recv(buf, sizeof(buf) - 1)) > 0) {
    buf[res] = 0;
    trace << buf;
  }

  // Disconnect and close the socket
  ASSERT(!sock->disconnect());
  ASSERT(!sock->close());
}

void loop()
{
  ASSERT(true == false);
}
