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
 * Demonstrates usage of the Cosa HTTP::Server support class. The
 * example class WebServer will reply with HTTP page with reading of
 * digital pin(0..13), analog pin(0..3) samples, voltage (bandgap),
 * amount of free memory, uptime (seconds), number of requests, and
 * the connecting client address (MAC, IP and port). 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/INET.hh"
#include "Cosa/INET/HTTP.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// HTML end of line
#define CRLF "\r\n"

// Example WebServer that responds with HTTP page with state of pins 
class WebServer : public HTTP::Server {
public:
  // Construct WebServer and initiate sequence counter
  WebServer() : m_nr(0) {}

  // Response member function
  virtual void on_request(char* http);

private:
  // Request sequence number
  uint16_t m_nr;
};

void 
WebServer::on_request(char* http)
{
  // Uptime in seconds
  uint32_t uptime =  Watchdog::millis() / 1000;
  uint16_t h = uptime / 3600;
  uint8_t m = (uptime / 60) % 60;
  uint8_t s = uptime % 60;

  // Bind the socket to an iostream
  IOStream page(m_sock);
  INET::addr_t addr;

  // Get client connection information; MAC, IP address and port
  m_sock->get_src(addr);

  // Reply page; header and footer are static, contents dynamic
  static const char header[] __PROGMEM = 
    "HTTP/1.1 200 OK" CRLF
    "Content-Type: text/html" CRLF
    "Connection: close" CRLF 
    "Refresh: 5" CRLF CRLF
    "<!DOCTYPE HTML>" CRLF
    "<HTML>" CRLF
    "<HEAD><TITLE>CosaWebServer</TITLE></HEAD>" CRLF 
    "<BODY>" CRLF;
  static const char footer[] __PROGMEM = 
    "</BODY>" CRLF 
    "</HTML>";
  static const char BR[] __PROGMEM = 
    "<BR/>" CRLF;

  // Construct the page; header-contents-footer
  page << header;
  for (uint8_t i = 0; i < 14; i++)
    page << PSTR("D") << i << PSTR(": ") << InputPin::read(i) << BR;
  for (uint8_t i = 0; i < 4; i++)
    page << PSTR("A") << i << PSTR(": ") << AnalogPin::sample(i) << BR;
  page << PSTR("Vcc (mV): ") << AnalogPin::bandgap() << BR;
  page << PSTR("Memory (byte): ") << free_memory() << BR;
  page << PSTR("Uptime (h:m:s): ") << h << ':' << m << ':' << s << BR;
  page << PSTR("Request(") << ++m_nr << PSTR("): ") << http << BR;
  page << PSTR("MAC: "); INET::print_mac(page, addr.mac); page << BR;
  page << PSTR("IP: "); INET::print_addr(page, addr.ip, addr.port); page << BR;
  page << footer;
}

// Network configuration
#define IP 192,168,0,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,0,1
#define PORT 80

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
WebServer server;

void setup()
{
  // Initiate uart and trace output stream. And watchdog
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWebServer: started"));
  Watchdog::begin();

  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));

  // Start the server
  ASSERT(server.begin(ethernet.socket(Socket::TCP, PORT)));
}

void loop()
{
  // Service incoming requests
  server.request();
}
