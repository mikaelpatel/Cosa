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
 * Demonstrate the Cosa HTTP::Client class and support for web access.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/INET/HTTP.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Network configuration
#define IP 192,168,1,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,1,1

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

// Simple web client; Prints response with url, number of bytes and time
class WebClient : public HTTP::Client {
public:
  virtual void on_response(const char* hostname, const char* path);
};

void 
WebClient::on_response(const char* hostname, const char* path)
{
  uint32_t start = Watchdog::millis();
  uint32_t count = 0L;
  char buf[129];
  int res;

  trace << PSTR("URL: http://") << (char*) hostname;
  if (*path) trace << '/' << (char*) path;
  trace << endl;

#if defined(PRINT_RESPONSE)
  while ((res = m_sock->read(buf, sizeof(buf) - 1)) >= 0) {
    if (res > 0) {
      buf[res] = 0;
      trace << buf;
      count += res;
    }
  }
#else
  while ((res = m_sock->read(buf, sizeof(buf) - 1)) >= 0) {
    if (res > 0) {
      trace << '.';
      count += res;
      if ((count & 0xfffL) == 0) trace << endl;
    }
  }
#endif
  if ((count & 0xfffL) != 0) trace << endl;
  trace << PSTR("Total (byte): ") << count << endl;
  trace << PSTR("Time (ms): ") << Watchdog::millis() - start << endl;
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWebClient: started"));
  Watchdog::begin();

  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));

  WebClient client;
  client.begin(ethernet.socket(Socket::TCP));
  client.get("www.google.com");
  client.get("www.arduino.cc");
  client.get("www.google.com/search?q=arduino");
  client.get("en.wikipedia.org/wiki/World_Wide_Web");
  client.end();
}

void loop()
{
  ASSERT(true == false);
}
