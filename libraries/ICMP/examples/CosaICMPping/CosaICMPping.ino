/**
 * @file CosaICMPping.ino
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
 * W5100 Ethernet Controller device driver example code; ICMP ping.
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
#include <ICMP.h>
#include <W5100.h>

#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Network configuration and network address to ping
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
static const uint8_t mac[6] __PROGMEM = { MAC };
#define IP 192,168,1,150
#define SUBNET 255,255,255,0
//#define LOCALHOST 192,168,1,102
#define GATEWAY 90,225,25,129
//#define DNS 195,67,199,21
#define DEST GATEWAY

// W5100 Ethernet Controller
W5100 ethernet(mac);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaICMPping: started"));
  Watchdog::begin();
  RTC::begin();

  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));
}

void loop()
{
  ICMP icmp(ethernet.socket(Socket::IPRAW, IPPROTO_ICMP));
  static uint16_t icmp_seq = 1;
  uint8_t dest[4] = { DEST };
  int res = icmp.ping(dest);
  trace << RTC::seconds() << ':';
  INET::print_addr(trace, dest);
  if (res < 0) {
    trace << PSTR(":could not reach network address") << endl;
  }
  else {
    trace << PSTR(":icmp_seq=") << icmp_seq
	  << PSTR(" time=") << res << PSTR(" ms")
	  << endl;
  }
  icmp_seq += 1;
  sleep(10);
}
