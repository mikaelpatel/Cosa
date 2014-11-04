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
 * Some NTP server hostnames; se.pool.ntp.org, time.nist.gov, 
 * ntp.ubuntu.com
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/INET/DNS.hh"
#include "Cosa/INET/NTP.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/RTC.hh"

// Time-zone; GMT+1, Stockholm
#define ZONE 1

// W5100 Ethernet Controller with default mac
W5100 ethernet;

// NTP server
#define NTP_SERVER "se.pool.ntp.org"

void setup()
{
  // Initiate trace iostream on uart. Use watchdog for basic timing
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNTP: started"));
  Watchdog::begin();
  RTC::begin();

  time_t::epoch_year( NTP_EPOCH_YEAR );
  time_t::epoch_weekday = NTP_EPOCH_WEEKDAY;
  time_t::pivot_year = 37; // 1937..2036 range
  
  // Note: This could also use_fastest_epoch if the clock_t offset was calculated
  // when the RTC is initiated.  NTP::gettimeofday would need modification.

  // Initiate the Ethernet Controller using DHCP
  ASSERT(ethernet.begin_P(PSTR("CosaNTPclient")));
}

void loop()
{
  static bool initiated = false;
  uint8_t server[4];

  // Use DNS to get the NTP server network address
  DNS dns;
  ethernet.get_dns_addr(server);
  if (!dns.begin(ethernet.socket(Socket::UDP), server)) return;
  if (dns.gethostbyname_P(PSTR(NTP_SERVER), server) != 0) return;

  // Connect to the NTP server using given socket
  NTP ntp(ethernet.socket(Socket::UDP), server, ZONE);

  // Get current time. Allow a number of retries
  const uint8_t RETRY_MAX = 20;
  clock_t clock;
  for (uint8_t retry = 0; retry < RETRY_MAX; retry++)
    if ((clock = ntp.time()) != 0L) break;
  ASSERT(clock != 0L);
  
  // Check if the RTC should be set
  if (!initiated) {
    RTC::time(clock);
    initiated = true;
  }

  // Get real-time clock and convert to time structure
  time_t rtc(RTC::seconds());
  uint16_t ms = RTC::millis() % 1000;
  
  // Print server
  INET::print_addr(trace, server);
  trace << PSTR(": ");
  
  // Print in stardate notation; dayno.secondno
  trace << (clock / SECONDS_PER_DAY) << '.' << (clock % SECONDS_PER_DAY) << ' ';

  // Convert to time structure and print day followed by date and time
  time_t now(clock);
  trace << now.day << ' ' << now << ' ' << rtc << '.';
  if (ms < 100) trace << '0';
  if (ms < 10) trace << '0';
  trace << ms << endl;

  // Take a nap for 10 seconds (this is not 10 seconds period)
  sleep(10);
}
