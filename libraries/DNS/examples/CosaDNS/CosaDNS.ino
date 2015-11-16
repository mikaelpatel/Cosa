/**
 * @file CosaDNS.ino
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
 * W5100 Ethernet Controller device driver example code; DNS client.
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

#include <DHCP.h>
#include <DNS.h>
#include <W5X00.h>
#include <W5100.h>
// #include <W5200.h>

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define IP 192,168,1,100
#define SUBNET 255,255,255,0
#define SERVER 192,168,1,1
// #define SERVER 79,138,0,180
// #define SERVER 85,8,31,209

// W5100 Ethernet Controller with MAC-address
const uint8_t mac[6] __PROGMEM = { MAC };
W5100 ethernet(mac);
// W5200 ethernet(mac);

// Query configuration
// #define NAME PSTR("www.arduino.cc")
#define NAME PSTR("www.google.com")
// #define NAME PSTR("www.github.com")

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDNS: started"));
  Watchdog::begin();

  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));
}

void loop()
{
  DNS dns;
  uint8_t server[4] = { SERVER };
  ASSERT(dns.begin(ethernet.socket(Socket::UDP), server));
  trace << PSTR("SERVER = ");
  INET::print_addr(trace, server, DNS::PORT);

  uint8_t host[4];
  ASSERT(dns.gethostbyname_P(NAME, host) == 0);
  trace << PSTR(":gethostbyname(") << NAME << PSTR(") = ");
  INET::print_addr(trace, host);
  trace.println();

  sleep(10);
}
