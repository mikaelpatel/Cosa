/**
 * @file CosaNTP.ino
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
 * W5100 Ethernet Controller device driver example code; NTP client.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/INET/DHCP.hh"
#include "Cosa/INET/NTP.hh"
#include "Cosa/Socket/Driver/W5100.hh"
  
#include "Cosa/Time.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define SE_POOL_NTP_ORG 85,8,42,205
#define TIME_NIST_GOV 64,236,96,53
#define NTP_UBUNTU_COM 91,189,94,4
#define SERVER SE_POOL_NTP_ORG

static const uint8_t mac[6] PROGMEM = { MAC };
static const char hostname[] PROGMEM = "CosaNTP";

// W5100 Ethernet Controller
W5100 ethernet(mac);

void setup()
{
  static const uint8_t RETRY_MAX = 4;

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNTP:started"));
  Watchdog::begin();

  // Get network address from DHCP server
  ASSERT(ethernet.begin());
  DHCP dhcp(hostname, mac);
  ASSERT(dhcp.begin(ethernet.socket(Socket::UDP, DHCP::PORT)));
  uint8_t res;
  for (uint8_t retry = 0; retry < RETRY_MAX; retry++) {
    res = dhcp.discover();
    if (res != 0) continue;
    uint8_t ip[4], subnet[4];
    res = dhcp.request(ip, subnet);
    if (res != 0) continue;
    dhcp.end();
    ethernet.bind(ip, subnet);
    return;
  }
  ASSERT(res == 0);
}

void loop()
{
  // Get the time from NTP server
  uint8_t server[4] = { SERVER };
  NTP ntp(ethernet.socket(Socket::UDP), server, 1);
  clock_t clock = ntp.time();
  if (clock == 0L) return;
  time_t now(clock);
  trace << clock << ' ';
  trace << now << endl;
  SLEEP(10);
}
