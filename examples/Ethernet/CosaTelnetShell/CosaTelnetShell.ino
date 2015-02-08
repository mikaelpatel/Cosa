/**
 * @file CosaTelnetShell.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
#include "TelnetCommands.h"

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// The Telnet Shell Server
class TelnetShell : public Telnet::Server {
public:
  TelnetShell(Shell& shell, IOStream& ios) :
    Telnet::Server(ios),
    m_shell(shell)
  {}
  bool begin(Socket* sock)
  {
    if (!Telnet::Server::begin(sock)) return (false);
    m_shell.set_echo(false);
    return (true);
  }
  virtual void on_connect(IOStream& ios) { m_shell.run(ios); }
  virtual void on_request(IOStream& ios) { m_shell.run(ios); }
  virtual void on_disconnect() { m_shell.reset(); }
protected:
  Shell& m_shell;
};
TelnetShell server(shell, ios);

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);

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
  ASSERT(server.begin(ethernet.socket(Socket::TCP, Telnet::PORT)));
}

void loop()
{
  // Run the server in blocking mode
  server.run();
}
