/**
 * @file CosaW5100.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

void setup()
{
  uint8_t ip[4] = { 192, 168, 0, 100 };
  uint8_t subnet[4] = { 255, 255, 255, 0 };
  // uint8_t dest[4] = { 192, 168,   0,   1 }; 
  // uint8_t dest[4] = { 192, 168,   0,   2 }; 
  // uint8_t dest[4] = { 192, 168,   0,  30 }; 
  uint8_t dest[4] = {  74, 125, 232, 128 };
  uint16_t port = 80;

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaW5100:started"));
  Watchdog::begin();
  ASSERT(ethernet.begin(ip, subnet));

  Socket* sock;
  ASSERT((sock = ethernet.socket(Socket::TCP)) != NULL);

  int res;
  ASSERT(!sock->connect(dest, port));
  while (sock->accept() == 0) SLEEP(1);
  
  char msg[] = "GET / HTTP/1.1\r\n\r\n\r\n";
  ASSERT(sock->send(msg, strlen(msg)) == strlen(msg));

  char buf[128];
  while ((res = sock->recv(buf, sizeof(buf))) >= 0) {
    if (res > 0) {
      buf[res] = 0;
      trace << buf;
    }
  }
  ASSERT(!sock->disconnect());
  ASSERT(!sock->close());
}

void loop()
{
  ASSERT(true == false);
}
