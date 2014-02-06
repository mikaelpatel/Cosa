/**
 * @file CosaSNMP.ino
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
 * W5100 Ethernet Controller device driver example code; SNMP agent.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/INET/SNMP.hh"

// Network configuration
#define IP 192,168,0,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,0,1

static const char hostname[] PROGMEM = "CosaSNMPAgent";
static const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };

W5100 ethernet(mac);
SNMP snmp;

void setup()
{
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSNMP: started"));
  Watchdog::begin();

  // Start ethernet controller and request network address for hostname
  ASSERT(ethernet.begin(hostname));
  // ASSERT(ethernet.begin(ip, subnet));

  // Print the given or default network address
  ethernet.get_addr(ip, subnet);
  trace.print_P(PSTR("HOSTNAME = "));
  trace.print_P(hostname);
  trace.println();
  trace.print_P(PSTR("IP = "));
  INET::print_addr(trace, ip);
  trace.println();
  
  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(W5100));
  TRACE(sizeof(SNMP));
  TRACE(sizeof(SNMP::OID));
  TRACE(sizeof(SNMP::VALUE));
  TRACE(sizeof(SNMP::PDU));

  // Start the SNMP manager with a connection-less socket
  ASSERT(snmp.begin(ethernet.socket(Socket::UDP, SNMP::PORT)));
}

void loop()
{
  static uint8_t nr = 0;
  SNMP::PDU pdu;

  // Wait for a request
  if (snmp.recv(pdu) < 0) return;

  // Should match PDU OID and if it is an get-request prepare a response value
  if (pdu.type == SNMP::PDU_GET) {
    pdu.value.encode(SNMP::SYNTAX_INT, -42);
  }

  // Use the error codes
  pdu.error_status = (nr++) % 6;

  // Send the response value
  if (snmp.send(pdu) < 0) return;

  // Print amount of free memory and the PDU
  TRACE(free_memory());
  trace << pdu << endl;
}
