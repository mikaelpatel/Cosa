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
 * W5100 Ethernet Controller device driver example code; Cosa Telnet 
 * and Shell example sketch.
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

#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/INET/Telnet.hh"
#include "Commands.h"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Input/Output Stream (bound to client socket)
IOStream ios;

// The Telnet Shell Server
class TelnetShell : public Telnet::Server {
public:
  bool begin(Socket* sock)
  {
    if (!Telnet::Server::begin(sock)) return (false);
    ios.set_device(sock);
    shell.set_echo(false);
    return (true);
  }
  virtual void on_request(IOStream& ios) 
  {
    shell.run(ios);
  }
};
TelnetShell server;

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

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
  ASSERT(ethernet.begin_P(PSTR("CosaTelnetShell")));

  // Allocate a TCP socket and start server
  Socket* sock = ethernet.socket(Socket::TCP, Telnet::PORT);
  ASSERT(sock != NULL);
  ASSERT(server.begin(sock));
}

void loop()
{
  // Run the server in blocking mode
  server.run();
}
