/**
 * @file CosaTelnetServer.ino
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
 * server port for trace stream output. Allow command "exit" and
 * carriage return to toggle trace output state. In trace mode 
 * this sketch will print the digital and analog pin values, bandgap
 * voltage (power supply voltage) and free memory to the connected
 * telnet client.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Socket/Driver/W5100.hh"

// Network configuration
#define IP 192,168,0,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,0,1
#define PORT 23

// W5100 Ethernet Controller with MAC-address
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };
W5100 ethernet(mac);
Socket* sock = NULL;

void setup()
{
  // Initiate watchdog for delay timing
  Watchdog::begin();

  // Initiate ethernet controller with address
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };
  ASSERT(ethernet.begin(ip, subnet));

  // Allocate a TCP socket and listen
  ASSERT((sock = ethernet.socket(Socket::TCP, PORT)) != NULL);
  ASSERT(!sock->listen());

  // Wait for incoming connection requests
  while (sock->accept() != 0) Watchdog::delay(32);
  trace.begin(sock, PSTR("CosaTelnetServer: started"));
  trace << flush;
}

void loop()
{
  // Trace output state (on/off)
  static bool state = true;

  // Check if a command is available
  int res = sock->available();
  if (res < 0) goto error;
  if (res > 0) {
    char buf[32];
    if (res > sizeof(buf)) res = sizeof(buf);
    sock->read(buf, res);
    if (res == 2) {
      state = !state;
      if (state) trace << PSTR("trace on");
      else trace << PSTR("trace off");
      trace << endl << flush;
    } 
    else if (!memcmp_P(buf, PSTR("exit"), 4)) goto error;
  }

  if (!state) return;

  // Trace state of device pins, power supply and free memory
  trace << Watchdog::millis() << PSTR(":D:");
  for (uint8_t i = 0; i < 14; i++) trace << InputPin::read(i);
  trace << PSTR(":A:") << AnalogPin::sample(0);
  for (uint8_t i = 1; i < 4; i++) trace << ':' << AnalogPin::sample(i);
  trace << PSTR(":VCC:") << AnalogPin::bandgap();
  trace << PSTR(":MEM:") << free_memory();
  trace << endl << flush;
  SLEEP(5);

  return;

 error:
  sock->disconnect();
  sock->listen();
  while (sock->accept() != 0) Watchdog::delay(32);
}
