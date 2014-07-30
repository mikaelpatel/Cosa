/**
 * @file CosaTelnetShell.ino
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
 * W5100 Ethernet Controller device driver example code; Use TELNET
 * server port for communication with the Cosa Shell.
 *
 * @section Circuit
 * This sketch is designed for the Ethernet Shield.
 * 
 *                       W5100/ethernet
 *                       +------------+
 * (D10)--------------29-|CSN         |
 * (D11)--------------28-|MOSI        |
 * (D12)--------------27-|MISO        |
 * (D13)--------------30-|SCK         |
 * (D2)-----[ ]-------56-|IRQ         |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Commands.h"
#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Network configuration; Telnet port number
#define PORT 23

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
Socket* sock = NULL;

// Input/Output Stream (bound to client socket)
IOStream ios;

// Idle time capture count. Reset of RTC micro-seconds overflow
uint32_t idle = 0L;

void iowait()
{
  uint32_t start = RTC::micros();
  Power::sleep(); 
  uint32_t stop = RTC::micros();
  idle = (start > stop) ? 0L : idle + (stop - start);
}

void setup()
{
  // Initiate timers
  Watchdog::begin();
  RTC::begin();

  // Set up idle time capture
  yield = iowait;

  // Setup trace output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTelnetShell: started"));

  // Start ethernet controller and request network address for hostname
  ASSERT(ethernet.begin_P(PSTR("CosaTelenetShell")));

  // Print the given or default network address
  uint8_t subnet[4];
  uint8_t ip[4];
  ethernet.get_addr(ip, subnet);
  trace << PSTR("server:IP=");
  INET::print_addr(trace, ip);
  trace << endl;

  // Allocate a TCP socket and listen
  ASSERT((sock = ethernet.socket(Socket::TCP, PORT)) != NULL);
  ASSERT(sock->listen() == 0);

  // Bind the socket to io-stream, set line and echo mode
  ios.set_device(sock);
  sock->set_eol(IOStream::CRLF_MODE);
  shell.set_echo(false);
}

void loop()
{
  // Wait for incoming connection requests
  while (sock->accept() != 0) yield();
  
  // Trace client connection information; MAC, IP address and port
  INET::addr_t addr;
  sock->get_src(addr);
  trace << PSTR("client:IP="); 
  INET::print_addr(trace, addr.ip, addr.port); 
  trace << PSTR(", MAC="); 
  INET::print_mac(trace, addr.mac); 
  trace << endl;
  
  // Skip the first telnet line and prompt
  int res;
  while ((res = sock->available()) == 0) yield();
  if (res < 0) goto error;
  while (res--) sock->getchar();
  shell.run(ios);
  sock->flush();

  // Process incoming commands
  while ((res = sock->available()) != IOStream::EOF) {
    if (res > 0) {
      shell.run(ios);
      sock->flush();
    }
    yield();
  }
  
 error:
  // Disconnect and wait for a new client
  ASSERT((sock->disconnect()) == 0);
  ASSERT((sock->listen()) == 0);

  // Reset shell for new session
  shell.reset();
}
