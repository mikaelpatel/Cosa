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

#include <CC3000.h>

#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

#if defined(WICKEDDEVICE_WILDFIRE)
CC3000 wifi(Board::D21, Board::EXT2, Board::D23);
#else
CC3000 wifi(Board::D3, Board::EXT1, Board::D4);
#endif

void setup()
{
  uint8_t PACKAGE_ID;
  uint8_t PACKAGE_BUILD_NR;
  int res;

  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCC3000info: started"));
  Watchdog::begin();
  RTT::begin();

  MEASURE("Start CC3000:", 1)
    ASSERT(wifi.begin_P(PSTR("CosaCC3300info")));

  MEASURE("Read software package information:", 1)
    res = wifi.read_sp_version(PACKAGE_ID, PACKAGE_BUILD_NR);
  if (res < 0) TRACE(res);
  TRACE(PACKAGE_ID);
  TRACE(PACKAGE_BUILD_NR);
  trace << endl;

  // #define DELETE_PROFILES
# if defined(DELETE_PROFILES)
  MEASURE("Clear all profiles:", 1)
    res = wifi.wlan_ioctl_del_profile(0xff);
  if (res < 0) TRACE(res);
# endif

  // #define SET_CONNECTION_POLICY
# if defined(SET_CONNECTION_POLICY)
  MEASURE("Set connect policy:", 1)
    res = wifi.wlan_ioctl_set_connection_policy(false, false, false);
  if (res < 0) TRACE(res);
# endif

  if (wifi.wlan_ioctl_statusget() == wifi.WLAN_STATUS_DISCONNECTED) {
    MEASURE("Connect to WLAN:", 1)
      res = wifi.wlan_connect(CC3000::WPA2_SECURITY_TYPE,
			      PSTR("SID"),
			      NULL,
			      PSTR("PASSWORD"));
    if (res < 0) TRACE(res);
  }

  uint8_t ip[4];
  uint8_t subnet[4];
  uint8_t dns[4];
  uint8_t mac[6];

  MEASURE("Wait for connection:", 1)
    while (1) {
      wifi.addr(ip, subnet);
      if (*ip != 0) break;
      wifi.service();
    }

  MEASURE("MAC and Network addresses:", 1) {
    wifi.dns_addr(dns);
    wifi.mac_addr(mac);
  }

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

