/**
 * @file CosaCC3000info.ino
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
 * Demonstration of CC3000 Wifi device driver; fetch network information. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/INET.hh"
#include "Cosa/Socket/Driver/CC3000.hh"

CC3000 wifi(Board::D21, Board::EXT2, Board::D23);

uint8_t PACKAGE_ID;
uint8_t PACKAGE_BUILD_NR;

void setup()
{
  int res;

  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCC3000info: started"));
  Watchdog::begin();
  RTC::begin();

  MEASURE("Start CC3000:", 1)
    ASSERT(wifi.begin_P(PSTR("CosaCC3300info")));

  MEASURE("Read software package information:", 1)
    res = wifi.read_sp_version(PACKAGE_ID, PACKAGE_BUILD_NR);
  if (res == 0) {
    TRACE(PACKAGE_ID);
    TRACE(PACKAGE_BUILD_NR);
    trace << endl;
  }

  INFO("MAC and Network addresses:", 0);
  uint8_t subnet[4];
  uint8_t dns[4];
  uint8_t ip[4];
  uint8_t mac[6];
  wifi.nvmem_read(CC3000::NVMEM_MAC_FILEID, mac, 0, sizeof(mac));
  wifi.get_addr(ip, subnet);
  wifi.get_dns_addr(dns);
  trace << "MAC="; INET::print_mac(trace, mac); trace << endl;
  trace << "IP=";  INET::print_addr(trace, ip); trace << endl;
  trace << "SUBNET="; INET::print_addr(trace, subnet); trace << endl;
  trace << "DNS="; INET::print_addr(trace, dns); trace << endl;
  trace << endl;
}

void loop()
{
  MEASURE("Request scan for available access points:", 1)
    ASSERT(wifi.wlan_ioctl_set_scanparam() == 0);

  INFO("Wait for scan results (10 sec)", 0);
  sleep(10);

  CC3000::hci_evnt_wlan_ioctl_get_scan_results_t info;
  MEASURE("Get scan results:", 1) {
    while (wifi.wlan_ioctl_get_scan_results(info) > 0) {
      trace << PSTR("ssid=") << (char*) info.ssid
	    << PSTR(",rssi=") << info.rssi
	    << PSTR(",wlan_security=") << info.wlan_security
	    << endl;
    }
  }
  trace << endl;
}

