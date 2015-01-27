/**
 * @file CosaCC3000.ino
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
 * Sandbox for CC3000 Wifi device driver development.
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

#define CRLF "\r\n"

#define FD_ZERO() 0UL
#define FD_ISSET(fd,set) ((1UL << (fd)) & set)
#define FD_SET(fd,set) set = (1UL << (fd)) | set

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaCC3000: started"));
  Watchdog::begin();
  RTC::begin();

  MEASURE("Start CC3000:", 1)
    ASSERT(wifi.begin_P(PSTR("CosaCC3300")));

  uint8_t PACKAGE_ID;
  uint8_t PACKAGE_BUILD_NR;
  int res;

  MEASURE("Read software package information:", 1)
    res = wifi.read_sp_version(PACKAGE_ID, PACKAGE_BUILD_NR);
  if (res == 0) {
    TRACE(PACKAGE_ID);
    TRACE(PACKAGE_BUILD_NR);
    trace << endl;
  }

  INFO("Get the network addresses provided by the access point:", 0);
  uint8_t subnet[4];
  uint8_t dns[4];
  uint8_t ip[4];
  wifi.get_addr(ip, subnet);
  wifi.get_dns_addr(dns);
  trace << "IP="; 
  INET::print_addr(trace, ip);
  trace << ",SUBNET="; 
  INET::print_addr(trace, subnet);
  trace << ",DNS="; 
  INET::print_addr(trace, dns);
  trace << endl << endl;

  MEASURE("Request scan for available access points:", 1)
    ASSERT(wifi.wlan_ioctl_set_scanparam() == 0);
  INFO("Wait for scan results (2 sec)", 0);
  sleep(2);
  CC3000::hci_evnt_wlan_ioctl_get_scan_results_t info;
  while (wifi.wlan_ioctl_get_scan_results(info) > 0) {
    trace << PSTR("ssid=") << (char*) info.ssid
	  << PSTR(",rssi=") << info.rssi
	  << PSTR(",wlan_security=") << info.wlan_security
	  << endl;
  }
  trace << endl;

  int handle;
  MEASURE("Create a socket:", 1)
    handle = wifi.socket(wifi.AF_INET, wifi.SOCK_STREAM, wifi.IPPROTO_TCP);
  ASSERT(handle >= 0);

  uint8_t WWW_GOOGLE_COM[4] = { 216, 58, 209, 132 };
  MEASURE("Connect to server:", 1) 
    ASSERT(wifi.connect(handle, WWW_GOOGLE_COM, 80) == 0);
  
  char msg[] = 
    "GET / HTTP/1.1" CRLF
    "Host: WildFire" CRLF
    "Connection: close" CRLF
    CRLF;
  trace << msg;
  trace.flush();
  MEASURE("Send request:", 1)
    res = wifi.send(handle, msg, strlen(msg));
  ASSERT((res > 0) && ((size_t) res == strlen(msg)));
  
  char buf[1500];
  uint32_t read_mask = FD_ZERO();
  uint32_t write_mask = FD_ZERO();
  uint32_t error_mask = FD_ZERO();
  FD_SET(handle, read_mask);
  FD_SET(handle, error_mask);
  MEASURE("Use select to wait for response:", 1)
    res = wifi.select(handle + 1, read_mask, write_mask, error_mask, 0, 0);
  ASSERT(res > 0);
  while ((res > 0) && (FD_ISSET(handle,read_mask))) {
    MEASURE("Receive HTML page:", 1) 
      res = wifi.recv(handle, buf, sizeof(buf));
    if (res < 0) break;
    for (int i = 0; i < res; i++)
      trace << (char) buf[i];
    res = wifi.select(1, read_mask, write_mask, error_mask, 0, 5000);
  } 
  trace << endl;

  MEASURE("Close socket:", 1)
    ASSERT(wifi.close(handle) == 0);
  trace << endl;

  INFO("Service WiFi events:", 0);
  trace << endl;
}

void loop()
{
  wifi.service();
  delay(100);
}

