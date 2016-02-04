/**
 * @file CosaDS2482.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2016, Mikael Patel
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

#ifndef COSA_DS2482_HH
#define COSA_DS2482_HH

#include "Cosa/TWI.hh"

/**
 * TWI Device Driver for DS2482 I2C to 1-Wire Bridge. Low level device
 * functions.
 *
 * @section References
 * 1. DS2482-100, Single Channel 1-Wire Master, Rev. 10, 1/15.
 * 2. DS2482-800, 8-Channel 1-Wire Master, Rev. 5, 12/14.
 */
class DS2482 : private TWI::Driver {
public:
  /**
   * Construct DS2482 TWI device driver.
   * @param[in] subaddr unit address (0..3, default 0).
   */
  DS2482(uint8_t subaddr = 0) : TWI::Driver(0x18 | (subaddr & 0x03)) {}

  /**
   * Global reset of device state machine logic. Returns true if
   * successful otherwise false.
   * @return bool.
   */
  bool device_reset();

  /**
   * Configure one wire bus master with given parameters. Returns true
   * if successful otherwise false.
   * @param[in] apu active pull-up (default true).
   * @param[in] spu strong pull-up (default false).
   * @param[in] iws one wire speed (default false).
   * @return bool.
   */
  bool device_config(bool apu = true, bool spu = false, bool iws = false);

  /**
   * Device Registers, pp. 5. Valid Pointer Codes, pp. 10.
   */
  enum Register {
    STATUS_REGISTER = 0xf0,
    READ_DATA_REGISTER = 0xe1,
    CHANNEL_SELECTION_REGISTER = 0xd2,
    CONFIGURATION_REGISTER = 0xc3
  } __attribute__((packed));

  /**
   * Set the read pointer to the specified register. Return register
   * value or negative error code.
   * @param[in] addr register address.
   * @return register value or negative error code.
   */
  int set_read_pointer(Register addr);

  /**
   * Select given channel (DS2482-800). Return true if successful
   * otherwise false.
   * @param[in] chan channel number (0..7).
   * @return bool.
   */
  bool channel_select(uint8_t chan);

  /**
   * Generates one wire reset/presence detect cycle. Should be used to
   * initiate or end a one wire communication sequence. Return true if
   * successful otherwise false.
   * @return bool.
   */
  bool one_wire_reset();

  /**
   * Read a single bit from one wire bus. Returns bit value (0 or 1)
   * or a negative error code.
   * @return bit or negative error code.
   */
  int one_wire_read_bit();

  /**
   * Write a single bit to one wire bus. Returns true if successful
   * otherwise false.
   * @param[in] value bit to write.
   * @return bool.
   */
  bool one_wire_write_bit(bool value);

  /**
   * Write a byte to one wire bus. Returns true if successful
   * otherwise false.
   * @param[in] value byte to write.
   * @return bool.
   */
  bool one_wire_write_byte(uint8_t value);

  /**
   * Read byte from one wire bus. Returns byte value or a negative
   * error code.
   * @return byte or negative error code.
   */
  int one_wire_read_byte();

  /**
   * Generate two read-time slots and one write-time slot.
   * Returns status value (dir.nid.id) or a negative
   * error code.
   * @return status or negative error code.
   */
  int one_wire_triplet(bool direction = false);

protected:
  /**
   * Function Commands, pp. 9-15.
   */
  enum {
    DEVICE_RESET = 0xf0,	//!< Device Reset.
    SET_READ_POINTER = 0xe1,	//!< Set Read Pointer.
    WRITE_CONGIFURATION = 0xd2,	//!< Write Configuration.
    CHANNEL_SELECT = 0xc3,	//!< Channel Select.
    ONE_WIRE_RESET = 0xb4,	//!< 1-Wire Reset.
    ONE_WIRE_SINGLE_BIT = 0x87,	//!< 1-Wire Single Bit.
    ONE_WIRE_WRITE_BYTE = 0xa5,	//!< 1-Wire Write Byte.
    ONE_WIRE_READ_BYTE = 0x96,	//!< 1-Wire Read Byte.
    ONE_WIRE_TRIPLET = 0x78	//!< 1-Wire Triplet.
  } __attribute__((packed));

  /**
   * Status Register, bit-fields, pp. 8-9.
   */
  union status_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access (little endian).
      uint8_t IWB:1;		//!< 1-Wire Busy.
      uint8_t PPD:1;		//!< Presence-Pulse Detect.
      uint8_t SD:1;		//!< Short Detected.
      uint8_t LL:1;		//!< Logic Level.
      uint8_t RST:1;		//!< Device Reset.
      uint8_t SBR:1;		//!< Single Bit Result.
      uint8_t TSB:1;		//!< Triplet Second Bit.
      uint8_t DIR:1;		//!< Branch Direction Taken.
    };
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Configuration Register, bit-fields, pp. 5-6.
   */
  union config_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access (little endian).
      uint8_t APU:1;		//!< Active Pullup.
      uint8_t ZERO:1;		//!< Always Zero(0).
      uint8_t SPU:1;		//!< Strong Pullup.
      uint8_t IWS:1;		//!< 1-Wire Speed.
      uint8_t COMP:4;		//!< Complement of lower 4-bits.
    };
    operator uint8_t()
    {
      return (as_uint8);
    }
    config_t()
    {
      as_uint8 = 0;
    }
  };

  /** Number of one-wire polls */
  static const int POLL_MAX = 20;
};

#endif
