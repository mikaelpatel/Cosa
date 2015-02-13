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

#include "Cosa/INET/ICMP.hh"
#include "Cosa/Socket/Driver/W5100.hh"

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

// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define IP 192,168,1,150
#define SUBNET 255,255,255,0
static const uint8_t mac[6] __PROGMEM = { MAC };

// W5100 Ethernet Controller
W5100 ethernet(mac);

// An address to ping
#define DEST 192,168,1,104

void setup()
{
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaICMPping: started"));
  Watchdog::begin();
  RTC::begin();

  ASSERT(ethernet.begin(ip, subnet));
}

void loop()
{
  ICMP icmp(ethernet.socket(Socket::IPRAW, IPPROTO_ICMP));
  uint8_t dest[4] = { DEST };
  TRACE(icmp.ping(dest));
  sleep(3);
}
