/**
 * @file Cosa/INET/SNMP.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/INET/SNMP.hh"
#include "Cosa/Watchdog.hh"

// SNMP MIB MIB-2 System OID(1.3.6.1.2.1.1.n)
const uint8_t SNMP::MIB2_SYSTEM::OID[] __PROGMEM = {
  6,0x2b,6,1,2,1,1
};

// Arduino MIB OID(1.3.6.1.4.1.36582)
const uint8_t SNMP::ARDUINO_MIB_OID[] __PROGMEM = {
  8,0x2b,6,1,4,1,130,157,102
};

bool
SNMP::MIB2_SYSTEM::is_request(PDU& pdu)
{
  // Match with SNMP MIB-2 System OID root
  int sys = pdu.oid.match(OID);
  if (sys < 0) return (false);

  // Get next value or step to next mib
  if (pdu.type == SNMP::PDU_GET_NEXT) {
    if (sys < sysServices) {
      sys += 1;
      pdu.oid.name[pdu.oid.length - 1] = sys;
      pdu.type = SNMP::PDU_GET;
    }
    else {
      memcpy_P(&pdu.oid, 
	       SNMP::ARDUINO_MIB_OID, 
	       pgm_read_byte(SNMP::ARDUINO_MIB_OID) + 1);
      pdu.oid.name[pdu.oid.length++] = 0;
      pdu.oid.name[pdu.oid.length++] = 0;
      return (false);
    }
  }

  // Check request type
  if (sys < sysDescr || sys > sysServices) return (false);

  // Get system value
  if (pdu.type == SNMP::PDU_GET) {
    switch (sys) {
    case sysDescr:
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, m_descr, strlen_P(m_descr));
      break;
    case sysObjectID:
      pdu.value.encode_P(SNMP::SYNTAX_OID, 
			 (const char*) ARDUINO_MIB_OID + 1, 
			 pgm_read_byte(ARDUINO_MIB_OID));
      break;
    case sysUpTime:
      pdu.value.encode(SNMP::SYNTAX_TIME_TICKS, Watchdog::millis() / 1000L);
      break;
    case sysContact:
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, m_contact, strlen_P(m_contact));
      break;
    case sysName:
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, m_name, strlen_P(m_name));
      break;
    case sysLocation:
      pdu.value.encode_P(SNMP::SYNTAX_OCTETS, m_location, strlen_P(m_location));
      break;
    case sysServices:
      pdu.value.encode(SNMP::SYNTAX_INT, 0x42);
      break;
    }
  }

  // Set system value
  else if (pdu.type == SNMP::PDU_SET) {
    pdu.error_status = SNMP::READ_ONLY;
  }
  return (true);
}

IOStream& operator<<(IOStream& outs, SNMP::OID& oid)
{
  for (uint8_t i = 0; i < oid.length; i++) {
    if (i > 0) outs << '.';
    uint16_t lv = 0;
    uint8_t v;
    while ((v = oid.name[i]) & 0x80) {
      lv = (lv << 7) | (v & 0x7f);
      i += 1;
    }
    lv = (lv << 7) | v;
    outs << lv;
  }
  return (outs);
}

int 
SNMP::OID::match(const uint8_t* coid, bool flag)
{
  uint8_t clen = pgm_read_byte(&coid[0]);
  if (length < clen) return (-1);
  for (uint8_t i = 0; i < clen; i++) 
    if (name[i] != pgm_read_byte(&coid[i + 1])) return (-1);
  if (flag) {
    if (length - 1 == clen) return (name[clen]);
    return (-1);
  }
  return (length == clen ? 0 : clen);
}

IOStream& operator<<(IOStream& outs, SNMP::PDU& pdu)
{
  outs << PSTR("dest = "); INET::print_addr(outs, pdu.dest, pdu.port); outs << endl;
  outs << PSTR("version = ") << pdu.version + 1 << endl;
  outs << PSTR("community = ") << pdu.community << endl;
  outs << PSTR("type = ") << hex << pdu.type << endl;
  outs << PSTR("request_id = ") << pdu.request_id << endl;
  outs << PSTR("error_status = ") << pdu.error_status << endl;
  outs << PSTR("error_index = ") << pdu.error_index << endl;
  outs << PSTR("oid = ") << pdu.oid << endl;
  outs << PSTR("value(syntax,length) = ");
  outs << pdu.value.syntax << PSTR(", ") << pdu.value.length << endl;
  outs.print(&pdu.value, pdu.value.length + 2, IOStream::hex);
  return (outs);
}

bool
SNMP::VALUE::encode(SYNTAX syn, const char* value, size_t size) 
{
  if ((syn == SYNTAX_OCTETS) 
      || (syn == SYNTAX_OPAQUE)) {
    if (size < DATA_MAX) {
      length = size;
      syntax = syn;
      memcpy(data, value, size);
      return (true);
    }
  }
  return (false);
}

bool
SNMP::VALUE::encode_P(SYNTAX syn, const char* value, size_t size) 
{
  if ((syn == SYNTAX_OCTETS) 
      || (syn == SYNTAX_OPAQUE)
      || (syn == SYNTAX_OID)) {
    if (size < DATA_MAX) {
      length = size;
      syntax = syn;
      memcpy_P(data, value, size);
      return (true);
    }
  }
  return (false);
}

bool
SNMP::VALUE::encode(SYNTAX syn, int16_t value) 
{
  if ((syn == SYNTAX_INT) 
      || (syn == SYNTAX_OPAQUE)) {
    uint8_t *p = (uint8_t*) &value;
    length = sizeof(value);
    syntax = syn;
    data[0] = p[1];
    data[1] = p[0];
    return (true);
  }
  return (false);
}

bool
SNMP::VALUE::encode(SYNTAX syn, int32_t value) 
{
  if ((syn == SYNTAX_INT32) 
      || (syn == SYNTAX_OPAQUE)) {
    uint8_t *p = (uint8_t*) &value;
    length = sizeof(value);
    syntax = syn;
    data[0] = p[3];
    data[1] = p[2];
    data[2] = p[1];
    data[3] = p[0];
    return (true);
  }
  return (false);
}

bool
SNMP::VALUE::encode(SYNTAX syn, uint32_t value) 
{
  if ((syn == SYNTAX_COUNTER) 
      || (syn == SYNTAX_TIME_TICKS) 
      || (syn == SYNTAX_GAUGE) 
      || (syn == SYNTAX_UINT32) 
      || (syn == SYNTAX_OPAQUE)) {
    uint8_t *p = (uint8_t*) &value;
    length = sizeof(value);
    syntax = syn;
    data[0] = p[3];
    data[1] = p[2];
    data[2] = p[1];
    data[3] = p[0];
    return (true);
  }
  return (false);
}

bool
SNMP::VALUE::encode(SYNTAX syn, const uint8_t* value)
{
  if ((syn == SYNTAX_IP_ADDRESS) 
      || (syn == SYNTAX_NSAPADDR) 
      || (syn == SYNTAX_OPAQUE)) {
    uint8_t *p = (uint8_t*) &value;
    length = sizeof(uint32_t);
    syntax = syn;
    data[0] = p[3];
    data[1] = p[2];
    data[2] = p[1];
    data[3] = p[0];
    return (true);
  }
  return (false);
}

bool
SNMP::VALUE::encode(SYNTAX syn, bool value)
{
  if ((syn == SYNTAX_BOOL) 
      || (syn == SYNTAX_OPAQUE)) {
    length = sizeof(uint8_t);
    syntax = syn;
    data[0] = value ? 0xff : 0x00;
    return (true);
  }
  return (false);
}

bool
SNMP::VALUE::encode(SYNTAX syn) 
{
  if ((syn == SYNTAX_NULL) 
      || (syn == SYNTAX_OPAQUE)) {
    length = 0;
    syntax = syn;
    return (true);
  }
  return (false);
}

bool 
SNMP::read_byte(uint8_t& value)
{
  return (read(&value, sizeof(value)) == sizeof(value));
}

bool 
SNMP::read_tag(uint8_t expect, uint8_t& length)
{
  uint8_t buf[2];
  if (read(buf, sizeof(buf)) != sizeof(buf)) return (false);
  length = buf[1];
  return (buf[0] == expect);
}

bool 
SNMP::decode_null()
{
  uint8_t length;
  return (read_tag(SYNTAX_NULL, length) && (length == 0));
}

bool 
SNMP::decode_integer(int32_t& value)
{
  uint8_t length;
  value = 0L;
  if (!read_tag(SYNTAX_INT, length)) return (false);
  if (length > sizeof(value)) return (false);
  if (read(&value, length) != length) return (false);
  value = value << (32 - (length * CHARBITS));
  value = ntoh(value);
  return (true);
}

bool 
SNMP::decode_string(char* buf, size_t count)
{
  uint8_t length;
  if (!read_tag(SYNTAX_OCTETS, length)) return (false);
  if (length > count - 1) return (false);
  if (read(buf, length) != length) return (false);
  buf[length] = 0;
  return (true);
}

bool 
SNMP::decode_sequence(uint8_t& length)
{
  return (read_tag(SYNTAX_SEQUENCE, length));
}

bool 
SNMP::decode_oid(OID& oid)
{
  if (!read_tag(SYNTAX_OID, oid.length)) return (false);
  if (oid.length > OID::NAME_MAX) return (false);
  return (read(oid.name, oid.length) == oid.length);
}

bool 
SNMP::encode_null()
{
  uint8_t header[] = { SYNTAX_NULL, 0 };
  return (write(header, sizeof(header)) == sizeof(header));
}

bool 
SNMP::encode_integer(int32_t value)
{
  uint8_t header[] = { SYNTAX_INT, sizeof(value) };
  if (write(header, sizeof(header)) != sizeof(header)) return (false);
  value = hton(value);
  return (write(&value, sizeof(value)) == sizeof(value));
}

bool 
SNMP::encode_string(const char* buf)
{
  uint8_t count = strlen(buf);
  uint8_t header[] = { SYNTAX_OCTETS, count };
  if (write(header, sizeof(header)) != sizeof(header)) return (false);
  return (write(buf, count) == count);
}

bool 
SNMP::encode_sequence(int32_t count)
{
  uint8_t header[] = { SYNTAX_SEQUENCE, (uint8_t) count };
  return (write(header, sizeof(header)) == sizeof(header));
}

bool 
SNMP::encode_oid(OID& oid)
{
  uint8_t header[] = { SYNTAX_OID, oid.length };
  if (write(header, sizeof(header)) != sizeof(header)) return (false);
  return (write(oid.name, oid.length) == oid.length);
}

bool 
SNMP::encode_pdu(uint8_t type, uint8_t size)
{
  return ((write(&type, sizeof(type)) == sizeof(type)) 
	  && (write(&size, sizeof(size)) == sizeof(size)));
}

bool 
SNMP::encode_value(VALUE& value)
{
  return (write(&value, value.length + 2) == value.length + 2);
}

bool 
SNMP::begin(Socket* sock, MIB2_SYSTEM* sys, MIB* mib)
{
  if ((sock == NULL) || (sys == NULL) || (mib == NULL)) return (false);
  m_sock = sock;
  m_sys = sys;
  m_mib = mib;
  return (true);
}

bool 
SNMP::end()
{
  m_sock->close();
  m_sock = NULL;
  m_sys = NULL;
  m_mib = NULL;
  return (true);
}

int
SNMP::request(PDU& pdu, uint32_t ms)
{
  // Attempt to receive a request within given time limit
  int res = recv(pdu, ms);
  if (res < 0) return (res);
  
  // Check for root getnext request
  if ((pdu.type == PDU_GET_NEXT) 
      && (pdu.oid.length == 1) 
      && (pdu.oid.name[0] == 0)) {
    const uint8_t* oid = MIB2_SYSTEM::OID;
    memcpy_P(&pdu.oid, oid, pgm_read_byte(oid) + 1);
    pdu.oid.name[pdu.oid.length++] = 0;
  }

  // Match with MIB handlers
  if (!m_sys->is_request(pdu) && !m_mib->is_request(pdu))
    pdu.error_status = NO_SUCH_NAME;
  
  // Send the response value
  return (send(pdu));
}

int 
SNMP::recv(PDU& pdu, uint32_t ms)
{
  uint8_t length;
  uint32_t start;
  uint8_t tag;
  int res;
  int err = -1;

  // Wait for an incoming request
  start = Watchdog::millis();
  while (((res = m_sock->recv(&tag, sizeof(tag), pdu.dest, pdu.port)) < 0) &&
	 ((ms == 0L) || ((Watchdog::millis() - start) < ms)))
    Power::sleep(SLEEP_MODE_IDLE);
  if (res != sizeof(tag)) goto error;

  // Decode the packet and extract elements
  if (tag != SYNTAX_SEQUENCE) goto error;
  if (!read_byte(length)) goto error;
  if (!decode_integer(pdu.version)) goto error;
  if (!decode_string(pdu.community, PDU::COMMUNITY_MAX)) goto error;
  if (!read_byte(pdu.type)) goto error;
  if (!read_byte(length)) goto error;
  if (!decode_integer(pdu.request_id)) goto error;
  if (!decode_integer(pdu.error_status)) goto error;
  if (!decode_integer(pdu.error_index)) goto error;
  if (!decode_sequence(length)) goto error;
  if (!decode_sequence(length)) goto error;
  if (!decode_oid(pdu.oid)) goto error;

  // Check for value to be set (otherwise null)
  if (pdu.type == SNMP::PDU_SET) {
    length = length - (sizeof(pdu.oid.length) + pdu.oid.length + 1);
    if (length > sizeof(pdu.value)) goto error;
    if (read(&pdu.value, length) != length) goto error;
  }
  else {
    pdu.value.encode(SYNTAX_NULL);
  }
  err = 0;

  // Flush any remaining data (could be a sequence of OID:VALUE, ignored)
 error:
  uint8_t buf[32];
  while (available() > 0) read(buf, sizeof(buf));
  return (err);
}

int 
SNMP::send(PDU& pdu)
{
  uint8_t packet_size;
  uint8_t pdu_size;
  int32_t varbind_list_size;
  int32_t varbind_size;

  // Set default values
  pdu.type = SNMP::PDU_RESPONSE;
  if (pdu.value.length == 0 || pdu.error_status != NO_ERROR) {
    pdu.value.encode(SYNTAX_NULL);
  }

  // Calculate size of packet sections
  varbind_size = (pdu.value.length + 2) + (pdu.oid.length + 2);
  varbind_list_size = (varbind_size + 2);
  pdu_size = ((varbind_list_size) + 2) + (3*(sizeof(int32_t) + 2));
  packet_size = (pdu_size + 2) + (strlen(pdu.community) + 2) + (sizeof(int32_t) + 2);
  
  // Create the datagram with all encoded elements
  if (m_sock->datagram(pdu.dest, pdu.port) < 0) goto error;
  if (!encode_sequence(packet_size)) goto error;
  if (!encode_integer(pdu.version)) goto error;
  if (!encode_string(pdu.community)) goto error;
  if (!encode_pdu(pdu.type, pdu_size)) goto error;
  if (!encode_integer(pdu.request_id)) goto error;
  if (!encode_integer(pdu.error_status)) goto error;
  if (!encode_integer(pdu.error_index)) goto error;
  if (!encode_sequence(varbind_list_size)) goto error;
  if (!encode_sequence(varbind_size)) goto error;
  if (!encode_oid(pdu.oid)) goto error;
  if (!encode_value(pdu.value)) goto error;
  
 error:
  // Send the datagram
  return (m_sock->flush());
}

