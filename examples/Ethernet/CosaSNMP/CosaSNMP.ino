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
 * W5100 Ethernet Controller device driver example code; SNMP agent
 * with support for GET/GETNEXT/SET for MIB-2 SYSTEM and Arduino MIB. 
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
 * @section Testing
 * Test with Linux snmp commands:
 * 1. Access the MIB-2 SYSTEM (sysDescr)
 *   snmpget -v1 -c public 192.168.1.18 0.1.3.6.1.2.1.1.1
 * 2. Access the Arduino MIB (ardDigitalPin.0)
 *   snmpget -v1 -c public 192.168.1.18 0.1.3.6.1.4.1.36582.1.0
 * 3. Walk the OID tree
 *   snmpwalk -v1 -c public 192.168.1.18
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/InputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/INET/SNMP.hh"
#include "Cosa/Socket/Driver/W5100.hh"
#include "Cosa/IOStream/Driver/UART.hh"

/**
 * Arduino MIB OID(1.3.6.1.4.1.36582)
 */
class ARDUINO_MIB : public SNMP::MIB {
private:
  enum {
    ardDigitalPin = 1,	      // Digital pin[0..19](0..1), read-only
    ardAnalogPin = 2,	      // Analog pin[0..5](0..1023), read-only
    ardVcc = 3,		      // Power supply[0](0..VCC), mV, read-only
  } __attribute__((packed));

public:
  /**
   * @override SNMP::MIB
   * Return object identity root for Arduino MIB.
   */
  virtual const uint8_t* get_oid()
  {
    return (SNMP::ARDUINO_MIB_OID);
  }
    
  /**
   * @override SNMP::MIB
   * Handle Arduino MIB objects SNMP requests. Returns true and
   * value for SNMP::GET in given protocol data unit, otherwise false.
   * @param[in,out] pdu protocol data unit.
   * @return bool
   */
  virtual bool is_request(SNMP::PDU& pdu);
};

bool 
ARDUINO_MIB::is_request(SNMP::PDU& pdu)
{
  // Match with Arduino MIB OID root
  int pos = pdu.oid.match(get_oid(), false);
  if (pos < 0 || pdu.oid.length != 10) return (false);

  // Access pin type and number
  uint8_t sys = pdu.oid.name[pos];
  uint8_t index = pdu.oid.name[pos + 1];
  
  // Get next value; adjust index referens
  if (pdu.type == SNMP::PDU_GET_NEXT) {
    switch (sys) {
    case 0:
      sys = ardDigitalPin;
      index = 0;
      break;
    case ardDigitalPin:
      if (index < 19)
	index += 1;
      else {
	index = 0;
	sys = ardAnalogPin;
      } 
      break;
    case ardAnalogPin:
      if (index < 5)
	index += 1;
      else {
	index = 0; 
	sys = ardVcc;
      }
      break;
    case ardVcc:
      return (false);
    }
    pdu.oid.name[pos] = sys;
    pdu.oid.name[pos + 1] = index;
    pdu.type = SNMP::PDU_GET;
  }

  // Check request type
  if (sys < ardDigitalPin || sys > ardVcc) return (false);
  
  // Get value for digital or analog pin, or power supply voltage
  if (pdu.type == SNMP::PDU_GET) {
    switch (sys) {
    case ardDigitalPin:
      if (index > 19)
	pdu.error_status = SNMP::NO_SUCH_NAME;
      else {
	Board::DigitalPin pin;
	pin = (Board::DigitalPin) pgm_read_byte(&digital_pin_map[index]);
	pdu.value.encode(SNMP::SYNTAX_INT, (int16_t) InputPin::read(pin));
      }
      break;
    case ardAnalogPin:
      if (index > 5)
	pdu.error_status = SNMP::NO_SUCH_NAME;
      else {
	Board::AnalogPin pin;
	pin = (Board::AnalogPin) pgm_read_byte(&analog_pin_map[index]);
	pdu.value.encode(SNMP::SYNTAX_INT, (int16_t) AnalogPin::sample(pin));
      }
      break;
    case ardVcc:
      if (index > 0)
	pdu.error_status = SNMP::NO_SUCH_NAME;
      else
	pdu.value.encode(SNMP::SYNTAX_INT, (int16_t) AnalogPin::bandgap());
      break;
    }
  }

  // Set value is not allowed (yet)
  else if (pdu.type == SNMP::PDU_SET) {
    pdu.error_status = SNMP::READ_ONLY;
  }
  return (true);
}

// Disable SD on Ethernet Shield
#define USE_ETHERNET_SHIELD
#if defined(USE_ETHERNET_SHIELD)
#include "Cosa/OutputPin.hh"
OutputPin sd(Board::D4, 1);
#endif

// Network configuration
#define IP 192,168,1,100
#define SUBNET 255,255,255,0
#define GATEWAY 192,168,1,1
static const uint8_t mac[6] __PROGMEM = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed };

// SNMP MIB-2 System configuration
static const char descr[] __PROGMEM = "<service description>";
static const char contact[] __PROGMEM = "<contact information>";
static const char name[] __PROGMEM = "<device name>";
static const char location[] __PROGMEM = "<device location>";

W5100 ethernet(mac);
SNMP::MIB2_SYSTEM mib2(descr, contact, name, location);
ARDUINO_MIB arduino;
SNMP snmp;

void setup()
{
  uint8_t ip[4] = { IP };
  uint8_t subnet[4] = { SUBNET };

  // Start watchdog and uart. Use uart for trace output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaSNMP: started"));
  Watchdog::begin();

  // Start ethernet controller and request network address for hostname
  ASSERT(ethernet.begin_P(PSTR("CosaSNMPAgent")));

  // Alternative give network address and subnet mask
  // ASSERT(ethernet.begin(ip, subnet));

  // Print the given or default network address
  ethernet.get_addr(ip, subnet);
  trace << PSTR("IP = ");
  INET::print_addr(trace, ip);
  trace << endl;
  
  // Print some memory statistics
  TRACE(free_memory());
  TRACE(sizeof(W5100));
  TRACE(sizeof(SNMP));
  TRACE(sizeof(SNMP::OID));
  TRACE(sizeof(SNMP::VALUE));
  TRACE(sizeof(SNMP::PDU));
  TRACE(sizeof(SNMP::MIB2_SYSTEM));
  TRACE(sizeof(ARDUINO_MIB));

  // Start the SNMP manager with a connection-less socket
  ASSERT(snmp.begin(ethernet.socket(Socket::UDP, SNMP::PORT), &mib2, &arduino));
}

void loop()
{
  // Service SNMP requests with given MIB handlers
  SNMP::PDU pdu;
  if (snmp.request(pdu) < 0) return;

#if defined(PRINT_PDU)
  // Print available memory and resulting protocol data unit
  TRACE(free_memory());
  trace << pdu << endl;
#endif
}
