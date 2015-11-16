/**
 * @file CosaDHCP.ino
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
 * W5100 Ethernet Controller device driver example code; DHCP client.
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

#include <DNS.h>
#include <DHCP.h>
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
#define IP 0,0,0,0
#define SUBNET 255,255,255,0
static const uint8_t mac[6] __PROGMEM = { MAC };
static const char hostname[] __PROGMEM = "CosaDHCP";

// W5100 Ethernet Controller and DHCP client
W5100 ethernet(mac);
// W5200 ethernet(mac);
DHCP dhcp(hostname, mac);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHCP: started"));
  Watchdog::begin();
  ASSERT(ethernet.begin());
}

void loop()
{
  // Allocate a connection-less socket on the DHCP port (client)
  Socket* sock = ethernet.socket(Socket::UDP, DHCP::PORT);

  // Discover DHCP server and request a network address
  uint8_t ip[4], subnet[4], gateway[4];
  if (!dhcp.begin(sock)) return;
  if (dhcp.discover()) goto error;
  if (dhcp.request(ip, subnet, gateway)) goto error;
  dhcp.end();

  // Print dynamic configuration
  trace << PSTR("DHCP = ");
  INET::print_addr(trace, dhcp.dhcp_addr());
  trace << endl;

  trace << PSTR("DNS = ");
  INET::print_addr(trace, dhcp.dns_addr());
  trace << endl;

  trace << PSTR("IP = ");
  INET::print_addr(trace, ip);
  trace << endl;

  trace << PSTR("GATEWAY = ");
  INET::print_addr(trace, gateway);
  trace << endl;

  trace << PSTR("SUBNET = ");
  INET::print_addr(trace, subnet);
  trace << endl;

  trace << PSTR("LEASE OBTAINED = ");
  trace << dhcp.lease_obtained();
  trace << endl;

  trace << PSTR("LEASE EXPIRES = ");
  trace << dhcp.lease_expires();
  trace << endl << endl;
  sleep(15);
  return;

 error:
  dhcp.end();
}
