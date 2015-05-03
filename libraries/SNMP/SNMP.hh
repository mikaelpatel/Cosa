/**
 * @file SNMP.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_SNMP_HH
#define COSA_SNMP_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"
#include "Cosa/IOStream.hh"

class SNMP {
public:
  /** ASN.1 Basic Encoding Rules (BER) Tags. */
  enum {
    ASN_BER_BASE_UNIVERSAL = 0x0,
    ASN_BER_BASE_APPLICATION = 0x40,
    ASN_BER_BASE_CONTEXT = 0x80,
    ASN_BER_BASE_PUBLIC = 0xC0,
    ASN_BER_BASE_PRIMITIVE = 0x0,
    ASN_BER_BASE_CONSTRUCTOR = 0x20
  } __attribute__((packed));

  /** SNMP Protocol Data Unit Operation Tags. */
  enum {
    PDU_GET = ASN_BER_BASE_CONTEXT | ASN_BER_BASE_CONSTRUCTOR | 0,
    PDU_GET_NEXT = ASN_BER_BASE_CONTEXT | ASN_BER_BASE_CONSTRUCTOR | 1,
    PDU_RESPONSE = ASN_BER_BASE_CONTEXT | ASN_BER_BASE_CONSTRUCTOR | 2,
    PDU_SET = ASN_BER_BASE_CONTEXT | ASN_BER_BASE_CONSTRUCTOR | 3,
    PDU_TRAP = ASN_BER_BASE_CONTEXT | ASN_BER_BASE_CONSTRUCTOR | 4
  } __attribute__((packed));

  /** SNMP Trap Tags. */
  enum {
    TRAP_COLD_START = 0,
    TRAP_WARM_START = 1,
    TRAP_LINK_DOWN = 2,
    TRAP_LINK_UP = 3,
    TRAP_AUTHENTICATION_FAIL = 4,
    TRAP_EGP_NEIGHBORLOSS = 5,
    TRAP_ENTERPRISE_SPECIFIC = 6
  } __attribute__((packed));

  /** SNMP Value Tags (VALUE::syntax). */
  enum SYNTAX {
    SYNTAX_SEQUENCE = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_CONSTRUCTOR | 0x10,
    SYNTAX_BOOL = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_PRIMITIVE | 1,
    SYNTAX_INT = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_PRIMITIVE | 2,
    SYNTAX_BITS = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_PRIMITIVE | 3,
    SYNTAX_OCTETS = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_PRIMITIVE | 4,
    SYNTAX_NULL = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_PRIMITIVE | 5,
    SYNTAX_OID = ASN_BER_BASE_UNIVERSAL | ASN_BER_BASE_PRIMITIVE | 6,
    SYNTAX_INT32 = SYNTAX_INT,
    SYNTAX_IP_ADDRESS = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 0,
    SYNTAX_COUNTER = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 1,
    SYNTAX_GAUGE = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 2,
    SYNTAX_TIME_TICKS = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 3,
    SYNTAX_OPAQUE = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 4,
    SYNTAX_NSAPADDR = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 5,
    SYNTAX_COUNTER64 = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 6,
    SYNTAX_UINT32 = ASN_BER_BASE_APPLICATION | ASN_BER_BASE_PRIMITIVE | 7,
  } __attribute__((packed));

  /** Error codes (PDU::error_status). */
  enum {
    NO_ERROR = 0,
    TOO_BIG = 1,
    NO_SUCH_NAME = 2,
    BAD_VALUE = 3,
    READ_ONLY = 4,
    GEN_ERR = 5
  };

  /** Object Identity (PDU::oid). */
  struct OID {
    static const size_t NAME_MAX = 32;
    uint8_t length;
    uint8_t name[NAME_MAX];
    /**
     * Find out how well ia given OID byte array matches to this one
     * @param[in] coid byte array of OID to match against
     * @param[in] flag if true and result is uint then uint is subsys of OID,
     *              otherwise the uint is length of passed in OID
     * return 0 if identical, -1 if earlier, -2 if later, uint if a child of.
     */
    int match(const uint8_t* coid, bool flag = true);
  };

  /** Object Value in Basic Encoding Rule (ASN.1 BER). */
  struct VALUE {
    static const uint8_t DATA_MAX = 64;
    uint8_t syntax;
    uint8_t length;
    uint8_t data[DATA_MAX];
    bool encode(SYNTAX syn, const char* value, size_t size);
    bool encode_P(SYNTAX syn, const char* value, size_t size);
    bool encode(SYNTAX syn, int16_t value);
    bool encode(SYNTAX syn, int32_t value);
    bool encode(SYNTAX syn, uint32_t value);
    bool encode(SYNTAX syn, const uint8_t* value);
    bool encode(SYNTAX syn, bool value);
    bool encode(SYNTAX syn);
  };

  /** SNMP Protocol Data Unit (PDU). */
  struct PDU {
    static const uint8_t COMMUNITY_MAX = 16;
    uint8_t dest[INET::IP_MAX];
    uint16_t port;
    int32_t version;
    char community[COMMUNITY_MAX];
    uint8_t type;
    int32_t request_id;
    int32_t error_status;
    int32_t error_index;
    OID oid;
    VALUE value;
  };

  /**
   * Abstract MIB handler. Should hold object identity root and
   * answer request query.
   */
  class MIB {
  public:
    /**
     * @override SNMP::MIB
     * Return object identity root for given mib.
     */
    virtual const uint8_t* get_oid() = 0;

    /**
     * @override SNMP::MIB
     * Check if the given protocol data unit is a request to the mib.
     * Returns true and value for SNMP::GET in given protocol data
     * unit, otherwise false.
     * @param[in,out] pdu protocol data unit.
     * @return bool
     */
    virtual bool is_request(PDU& pdu) = 0;
  };

  /**
   * Mandatory SNMP MIB MIB-2 System OID(1.3.6.1.2.1.1.n).
   */
  class MIB2_SYSTEM : public MIB {
  public:
    /**
     * Construct mib-2 system mib with given static data.
     * @param[in] descr description string (in program memory).
     * @param[in] contact contact string (in program memory).
     * @param[in] name device name string (in program memory).
     * @param[in] location device location string (in program memory).
     */
    MIB2_SYSTEM(const char* descr,
		const char* contact,
		const char* name,
		const char* location,
        const uint8_t* sysoid=ARDUINO_MIB_OID) :
      m_descr(descr),
      m_contact(contact),
      m_name(name),
      m_location(location),
      m_sysoid(sysoid)
    {
    }

    /**
     * @override SNMP::MIB
     * Return object identity root for MIB-2 SYSTEM.
     */
    virtual const uint8_t* get_oid()
    {
      return (OID);
    }

    /**
     * @override SNMP::MIB
     * Handle SNMP MIB-2 System objects SNMP requests. Returns true and
     * value for SNMP::GET in given protocol data unit, otherwise false.
     * @param[in,out] pdu protocol data unit.
     * @return bool
     */
    virtual bool is_request(PDU& pdu);

  private:
    static const uint8_t OID[] PROGMEM;
    const char* m_descr;
    const char* m_contact;
    const char* m_name;
    const char* m_location;
    const uint8_t* m_sysoid;
    enum {
      sysDescr = 1,		//!< DisplayString(0..255), read-only, mandatory.
      sysObjectID = 2,		//!< OID, read-only, mandatory.
      sysUpTime = 3,		//!< TimeTicks, read-only, mandatory.
      sysContact = 4,		//!< DisplayString(0..255), read-write, mandatory.
      sysName = 5,		//!< DisplayString(0..255), read-write, mandatory.
      sysLocation = 6,		//!< DisplayString(0..255), read-write, mandatory.
      sysServices = 7		//!< Integer(0..127), read-only, mandatory.
    } __attribute__((packed));
    friend class SNMP;
  };

  /** Arduino MIB OID(1.3.6.1.4.1.36582). */
  static const uint8_t ARDUINO_MIB_OID[] PROGMEM;

  /** The SNMP Agent standard port. */
  static const uint16_t PORT = 161;

  /**
   * Start SNMP agent with the given socket (UDP::PORT). Returns true
   * if successful otherwise false.
   * @param[in] sock connection-less socket on SNMP::PORT.
   * @param[in] sys mib-2 system.
   * @param[in] mib application mib.
   * @return bool
   */
  bool begin(Socket* sock, MIB2_SYSTEM* sys, MIB* mib);

  /**
   * Stop SNMP agent. Returns true if successful otherwise false.
   * @return bool
   */
  bool end();

  /**
   * Receive SNMP protocol data unit (PDU) request within given time
   * limit in milli-seconds, process and send response. Returns zero
   * and data in given PDU otherwise a negative error code.
   * @param[in,out] pdu protocol unit.
   * @param[in] ms time-out period in milli-seconds (Default BLOCK).
   * @return zero if successful otherwise a negative error code.
   */
  int request(PDU& pdu, uint32_t ms = 0L);

protected:
  /**
   * Receive SNMP protocol data unit (PDU) request within given time
   * limit in milli-seconds. Returns zero and data in given PDU
   * otherwise a negative error code.
   * @param[in,out] pdu protocol unit.
   * @param[in] ms time-out period in milli-seconds (Default BLOCK).
   * @return zero if successful otherwise a negative error code.
   */
  int recv(PDU& pdu, uint32_t ms = 0L);

  /**
   * Send SNMP protocol data unit (PDU) response. Application should
   * fill in error status and index, and value for PDU_GET on matching
   * OID. For PDU_SET the OID should be matched and the application
   * should set error status if READ_ONLY.
   * @param[in] pdu protocol unit.
   * @return zero if successful otherwise a negative error code.
   */
  int send(PDU& pdu);

  bool read_byte(uint8_t& value);
  bool read_tag(uint8_t expect, uint8_t& length);

  bool decode_null();
  bool decode_integer(int32_t& res);
  bool decode_string(char* buf, size_t count);
  bool decode_sequence(uint8_t& length);
  bool decode_oid(OID& oid);

  bool encode_null();
  bool encode_integer(int32_t res);
  bool encode_string(const char* buf);
  bool encode_sequence(int32_t count);
  bool encode_oid(OID& oid);
  bool encode_pdu(uint8_t type, uint8_t size);
  bool encode_value(VALUE& value);

  int available()
  {
    return (m_sock->available());
  }

  int read(void* buf, size_t size)
  {
    return (m_sock->read(buf, size));
  }

  int write(const void* buf, size_t size)
  {
    return (m_sock->write(buf, size));
  }

  int write_P(const void* buf, size_t size)
  {
    return (m_sock->write_P(buf, size));
  }

  /** Connection-less socket for incoming requests. */
  Socket* m_sock;

  /** Attached MIB to service. */
  MIB2_SYSTEM* m_sys;
  MIB* m_mib;
};

/**
 * Print given Object Identity path to given output stream.
 * @param[in] outs output stream.
 * @param[in] oid object identity structure.
 * @return output stream.
 */
IOStream& operator<<(IOStream& outs, SNMP::OID& oid);

/**
 * Print given SNMP Protocol Data Unit (PDU) to given output stream.
 * @param[in] outs output stream.
 * @param[in] pdu protocol data unit.
 * @return output stream.
 */
IOStream& operator<<(IOStream& outs, SNMP::PDU& pdu);

#endif
