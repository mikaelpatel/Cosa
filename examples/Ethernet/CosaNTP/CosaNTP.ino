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

#include "Cosa/INET/NTP.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Network configuration
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define SE_POOL_NTP_ORG 85,8,42,205
#define TIME_NIST_GOV 64,236,96,53
#define NTP_UBUNTU_COM 91,189,94,4
#define SERVER SE_POOL_NTP_ORG
#define ZONE 1

static const uint8_t mac[6] PROGMEM = { MAC };
static const char hostname[] PROGMEM = "CosaNTP";

// W5100 Ethernet Controller
W5100 ethernet(mac);

void setup()
{
  // Initiate trace iostream on uart. Use watchdog for basic timing
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNTP:started"));
  Watchdog::begin();

  // Initiate the Ethernet Controller using DHCP
  ASSERT(ethernet.begin(hostname));
}

void loop()
{
  // Connect to the NTP server using given socket
  uint8_t server[4] = { SERVER };
  NTP ntp(ethernet.socket(Socket::UDP), server, ZONE);

  // Get current time in seconds since NTP epoch
  clock_t clock = ntp.time();
  if (clock == 0L) return;
  trace << clock << ' ';

  // Convert to time structure and print
  time_t now(clock);
  trace << now << endl;

  // Take a nap for 10 seconds (this is not 10 seconds period)
  SLEEP(10);
}
