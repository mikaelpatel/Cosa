/**
 * @file CosaWebServer.ino
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
 * W5100 Ethernet Controller device driver example code; HTTP server.
 * Waits for requests. Reply with reading of digital pin(0..13), 
 * analog pin(0..3) samples, voltage (bandgap), amount of free memory
 * in loop and at setup, uptime (seconds), number of requests, and
 * the connecting client address (MAC, IP and port).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOBuffer.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Network configuration
#define IP 192,168,0,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,0,1
#define PORT 80

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
Socket* sock;

// Debugging mode; trace print
#define NDEBUG
#ifdef NDEBUG
#undef TRACE
#define TRACE(x) x
#endif

// HTML end of line
#define CRLF "\r\n"

// Amount of free memory at setup (to compare with loop)
uint16_t setup_free_memory;

void setup()
{
  setup_free_memory = free_memory();

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWebServer:started"));
  Watchdog::begin();

  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));

  // Allocate a TCP socket and listen
  ASSERT((sock = ethernet.socket(Socket::TCP, PORT)) != NULL);
  ASSERT(!sock->listen());
}

void loop()
{
  // Request sequence number
  static uint16_t nr = 1;

  // Wait for incoming connection requests
  while (sock->accept() != 0) Watchdog::delay(32);

  // Uptime in seconds
  uint32_t uptime =  Watchdog::millis() / 1000;
  uint16_t h = uptime / 3600;
  uint8_t m = (uptime / 60) % 60;
  uint8_t s = uptime % 60;

  // Bind the socket to an iostream
  IOStream page(sock);
  INET::addr_t addr;
  char buf[32];
  int res;

  // Get client connection information; MAC, IP address and port
  sock->get_src(addr);

  // Wait for the HTTP request
  while ((res = sock->available()) == 0);
  if (res < 0) goto error;
  TRACE(sock->available());

#ifndef NDEBUG
  // Read request and print in debug mode. Ignore contents
  trace << PSTR("Request#") << nr << endl;
  while ((res = sock->recv(buf, sizeof(buf) - 1)) > 0) {
    buf[res] = 0;
    trace << buf;
  }
  trace << endl;
#endif

  // Reply page; header and footer are static, contents dynamic
  static const char header[] PROGMEM = 
    "HTTP/1.1 200 OK" CRLF
    "Content-Type: text/html" CRLF
    "Connection: close" CRLF 
    "Refresh: 5" CRLF CRLF
    "<!DOCTYPE HTML>" CRLF
    "<HTML>" CRLF
    "<HEAD><TITLE>CosaWebServer</TITLE></HEAD>" CRLF 
    "<BODY>" CRLF;
  static const char footer[] PROGMEM = 
    "</BODY>" CRLF 
    "</HTML>";
  static const char BR[] PROGMEM = 
    "<BR/>" CRLF;

  // Construct the page; header-contents-footer
  page << header;
  for (uint8_t i = 0; i < 14; i++)
    page << PSTR("D") << i << PSTR(": ") << InputPin::read(i) << BR;
  for (uint8_t i = 0; i < 4; i++)
    page << PSTR("A") << i << PSTR(": ") << AnalogPin::sample(i) << BR;
  page << PSTR("Vcc (mV): ") << AnalogPin::bandgap() << BR;
  page << PSTR("Memory (byte): ") << free_memory();
  page << PSTR("(") << setup_free_memory << PSTR(")") << BR;
  page << PSTR("Uptime (h:m:s): ") << h << ':' << m << ':' << s << BR;
  page << PSTR("Requests: ") << nr++ << BR;
  page << PSTR("MAC: ") << addr.mac[0];
  for (uint8_t i = 1; i < 6; i++) page << PSTR(":") << addr.mac[i];
  page << BR;
  page << PSTR("IP: ") << addr.ip[0];
  for (uint8_t i = 1; i < 4; i++) page << PSTR(".") << addr.ip[i];
  page << BR;
  page << PSTR("PORT: ") << addr.port << BR;
  page << footer;
  page << flush;

  // Disconnect the client and allow new connection requests
 error:
  TRACE(sock->disconnect());
  TRACE(sock->listen());
}
