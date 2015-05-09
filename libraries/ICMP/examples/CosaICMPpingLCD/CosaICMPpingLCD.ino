/**
 * @file CosaICMPpingLCD.ino
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
 * W5100 Ethernet Controller device driver example code; ICMP ping
 * with display on LCD.
 *
 * @section Circuit
 * This sketch is designed for the Ethernet Shield and 1602 LCD.
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
#include "Cosa/IOStream.hh"

// Select port type to use with the LCD device driver.
// LCD and communication port
#include <HD44780.h>

// HD44780 driver built-in adapters
// HD44780::Port4b port;
// HD44780::SR3W port;
// HD44780::SR3WSPI port;
// HD44780::SR4W port;

// I2C expander io port based adapters
#include <PCF8574.h>
#include <MJKDZ_LCD_Module.h>
// MJKDZ_LCD_Module port;
// MJKDZ_LCD_Module port(0);
#include <GY_IICLCD.h>
GY_IICLCD port;
// #include <DFRobot_IIC_LCD_Module.h>
// DFRobot_IIC_LCD_Module port;
// #include <SainSmart_LCD2004.h>
// SainSmart_LCD2004 port;

// HD44780 based LCD with support for serial communication
// #include <ERM1602_5.h>
// ERM1602_5 port;

// HD44780 variants; 16X1, 16X2, 16X4, 20X4, default 16X2
// HD44780 lcd(&port, 20, 4);
// HD44780 lcd(&port, 16, 4);
HD44780 lcd(&port);
IOStream cout(&lcd);

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Network configuration and network address to ping
#define MAC 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed
#define IP 192,168,1,150
#define SUBNET 255,255,255,0

// W5100 Ethernet Controller
static const uint8_t mac[6] __PROGMEM = { MAC };
W5100 ethernet(mac);

void setup()
{
  // Our address (fixed)
  uint8_t subnet[4] = { SUBNET };
  uint8_t ip[4] = { IP };

  // Initiate necessary components
  Watchdog::begin();
  RTC::begin();
  lcd.begin();

  // Start the ethernet controller
  ethernet.begin(ip, subnet);
}

void loop()
{
  // List of network addresses to ping
  static const uint8_t DEST_MAX = 6;
  uint8_t dest[DEST_MAX][4] = {
    { 192,168,  1,  1 },
    { 192,168,  1,100 },
    { 192,168,  1,101 },
    { 192,168,  1,102 },
    {  90,225, 25,129 },
    { 195, 67,199, 21 }
  };

  // Max roundtrip in milli-seconds
  static int stat[DEST_MAX] = {
    -1, -1, -1, -1, -1, -1
  };

  // Current network address table index
  static uint8_t ix = 0;

  // Create a socket and ping current address
  ICMP icmp(ethernet.socket(Socket::IPRAW, IPPROTO_ICMP));
  int res = icmp.ping(dest[ix]);

  // Display network address, roundtrip and max
  cout << clear;
  INET::print_addr(cout, dest[ix]);
  cout << endl;
  if (res < 0) {
    cout << '-';
  }
  else {
    if (res > stat[ix]) stat[ix] = res;
    cout << res;
  }
  cout << '(' << stat[ix] << PSTR(") ms");
  ix += 1;
  if (ix == DEST_MAX) ix = 0;

  // Take a nap until the next ping
  sleep(3);
}
