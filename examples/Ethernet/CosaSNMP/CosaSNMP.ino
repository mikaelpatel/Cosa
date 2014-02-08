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
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/INET/SNMP.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// Network configuration
#define IP 192,168,0,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,0,1

static const char hostname[] PROGMEM = "CosaSNMPAgent";
static const uint8_t mac[6] PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };

W5100 ethernet(mac);
SNMP snmp;

/**
 * Handle Arduino MIB objects SNMP requests. Returns true and value
 * for SNMP::GET in given protocol data unit, otherwise false.
 * @param[in,out] pdu protocol data unit.
 * @return bool
 */
bool arduino_mib(SNMP::PDU& pdu)
{
  // Match with Arduino MIB OID root
  int sys = pdu.oid.match(SNMP::ARDUINO_MIB, false);
  if (sys < 0 || pdu.oid.length != 11) return (false);

  // Access pin number and requested object
  uint8_t pin = pdu.oid.name[sys + 1];
  sys = pdu.oid.name[sys];
  if (sys < SNMP::ardDigitalPin || sys > SNMP::ardVcc) return (false);

  // Get value for digital or analog pin, or power supply voltage
  if (pdu.type == SNMP::PDU_GET) {
    switch (sys) {
    case SNMP::ardDigitalPin:
      if (pin > 22) 
	pdu.error_status = SNMP::BAD_VALUE;
      else
	pdu.value.encode(SNMP::SYNTAX_INT, (int16_t) InputPin::read(pin));
      break;
    case SNMP::ardAnalogPin:
      if (pin > 7)
	pdu.error_status = SNMP::BAD_VALUE;
      else
	pdu.value.encode(SNMP::SYNTAX_INT, (int16_t) AnalogPin::sample(pin));
      break;
    case SNMP::ardVcc:
      pdu.value.encode(SNMP::SYNTAX_INT, (int16_t) AnalogPin::bandgap());
      break;
    }
  }

  // Set value is not allowed
  else if (pdu.type == SNMP::PDU_SET) {
    pdu.error_status = SNMP::READ_ONLY;
  }
  return (true);
}

/**
 * Handle SNMP MIB-2 System objects SNMP requests. Returns true and
 * value for SNMP::GET in given protocol data unit, otherwise false. 
 * @param[in,out] pdu protocol data unit.
 * @return bool
 */
bool system_mib(SNMP::PDU& pdu)
{
  // Match with SNMP MIB-2 System OID root
  int sys = pdu.oid.match(SNMP::MIB2_SYSTEM);
  if (sys < SNMP::sysDescr || sys > SNMP::sysServices) return (false);

  // Get system value
  if (pdu.type == SNMP::PDU_GET) {
    switch (sys) {
    case SNMP::sysDescr:
      static const char DESCR[] PROGMEM = "<description>";
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, DESCR, sizeof(DESCR) - 1);
      break;
    case SNMP::sysObjectID:
      pdu.value.encode_P(SNMP::SYNTAX_OID, (const char*) SNMP::ARDUINO_MIB, 
			 pgm_read_byte(SNMP::ARDUINO_MIB) + 1);
      break;
    case SNMP::sysUpTime:
      pdu.value.encode(SNMP::SYNTAX_UINT32, Watchdog::millis() / 1000L);
      break;
    case SNMP::sysServices:
      pdu.value.encode(SNMP::SYNTAX_INT, 42);
      break;
    case SNMP::sysContact:
      static const char CONTACT[] PROGMEM = "<your name>";
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, CONTACT, sizeof(CONTACT) - 1);
      break;
    case SNMP::sysName:
      static const char NAME[] PROGMEM = "<device name>";
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, NAME, sizeof(NAME) - 1);
      break;
    case SNMP::sysLocation:
      static const char LOCATION[] PROGMEM = "<device location>";
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, LOCATION, sizeof(LOCATION) - 1);
      break;
    }
  }

  // Set system value
  else if (pdu.type == SNMP::PDU_SET) {
    switch (sys) {
    case SNMP::sysContact:
    case SNMP::sysName:
    case SNMP::sysLocation:
      break;
    default:
      pdu.error_status = SNMP::READ_ONLY;
    }
  }
  return (true);
}

void setup()
{
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };

  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSNMP: started"));
  Watchdog::begin();

  // Start ethernet controller and request network address for hostname
  ASSERT(ethernet.begin(hostname));

  // Alternative give network address and subnet mask
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
  SNMP::PDU pdu;

  // Wait for a request
  if (snmp.recv(pdu) < 0) return;

  // Match with MIB handlers
  if (!arduino_mib(pdu) && !system_mib(pdu))
    pdu.error_status = SNMP::NO_SUCH_NAME;
  
  // Send the response value
  if (snmp.send(pdu) < 0) return;

  // Print amount of free memory and the reponse
  TRACE(free_memory());
  trace << pdu << endl;
}
