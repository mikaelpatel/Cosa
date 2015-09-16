/**
 * @file W25X40CL.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_W25X40CL_HH
#define COSA_W25X40CL_HH

#include "Cosa/Types.h"
#include "Cosa/SPI.hh"
#include "Cosa/Flash.hh"

/**
 * Cosa Winbond W25X40CL flash device driver class. Implements
 * the Cosa Flash device driver interface with erase, read and
 * write/program flash memory blocks.
 *
 * @section References
 * 1. Winbond W25X40CL, spiflash, 2.5/3/3.3V 4M-bit, Serial Flash
 * Memory with 4KB Sectors and Dual I/O SPI, Rev. E, Publication
 * Release Date: October 15, 2012.
 */
class W25X40CL : public Flash::Device, protected SPI::Driver {
public:
  /**
   * Default programming page buffer size (pp. 61, 97).
   */
  static const size_t PAGE_MAX = 256;
  static const size_t PAGE_MASK = PAGE_MAX - 1;

  /**
   * Construct W25X40CL device driver with given chip select pin.
   * @param[in] csn chip select pin (default D15/D3).
   */
#if !defined(BOARD_ATTINY)
  W25X40CL(Board::DigitalPin csn = Board::D15) :
    Flash::Device(4 * 1024L, 128),
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0, SPI::MSB_ORDER, NULL)
  {}
#else
  W25X40CL(Board::DigitalPin csn = Board::D3) :
    Flash::Device(4 * 1024L, 128),
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0, SPI::MSB_ORDER, NULL)
  {}
#endif

  /**
   * @override{Flash::Device}
   * Initiate the flash memory device driver and check for valid
   * identification. Return true(1) if the successful otherwise
   * false(0).
   * @return bool
   */
  virtual bool begin();

  /**
   * @override{Flash::Device}
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @return bool
   */
  virtual bool is_ready();

  /**
   * @override{Flash::Device}
   * Read flash block with the given size into the buffer from the
   * source address. Return number of bytes read or negative error
   * code.
   * @param[in] dest buffer to read from flash into.
   * @param[in] src address in flash to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  virtual int read(void* dest, uint32_t src, size_t size);

  /**
   * @override{Flash::Device}
   * Erase given flash block for given byte address. The size of the
   * erased sector/block is either 4/32/64 KB or 255 for chip
   * erase. The flash memory consists of 128 X 4 KB sectors. The
   * highest sector is reserved. Returs zero(0) if successful
   * otherwise an negative error code (EINVAL if illegal sector size).
   * @param[in] dest destination block byte address to erase.
   * @param[in] size of sector to erase in Kbyte (Default 4 KByte).
   * @return zero or negative error code.
   */
  virtual int erase(uint32_t dest, uint8_t size = 4);

  /**
   * @override{Flash::Device}
   * Write flash block at given destination address with the contents
   * of the source buffer. Return number of bytes written or negative
   * error code.
   * @param[in] dest address in flash to write to.
   * @param[in] src buffer to write to flash.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  virtual int write(uint32_t dest, const void* src, size_t size);

  /**
   * @override{Flash::Device}
   * Write flash block at given destination address with contents
   * of the source buffer in program memory. Return number of bytes
   * written or negative error code.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write_P(uint32_t dest, const void* buf, size_t size);

  /**
   * Status Register (S0) bitfields (Chap. 8.1 Status Register, pp. 11-12).
   */
  union status_t {
    uint8_t as_uint8;		//!< As unsigned 8-bit value.
    struct {			//!< As bit-fields.
      uint8_t BUSY:1;		//!< Erase/Write In Progress.
      uint8_t WEL:1;		//!< Write Enable Latch.
      uint8_t BP:3;		//!< Block Protect Bits.
      uint8_t TB:1;		//!< Top/Bottom Protect.
      uint8_t RESERVED:1;	//!< Reserved.
      uint8_t SRP:1;		//!< Status Register Protect.
    };
    status_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Read device status register.
   * @return status code.
   */
  uint8_t read_status()
  {
    return (issue(RDSR));
  }

protected:
  /**
   * Instruction Set (chap. 8.2.2, pp. 15)
   */
  enum Command {
    WREN = 0x06,		//!< Write Enable.
    WREVSR = 0x50,		//!< Write Enable for Volatile Status Register.
    WRDI = 0x04,		//!< Write Disable.
    RDSR = 0x05,		//!< Read Status Register.
    WRR = 0x01,			//!< Write Status Register.
    READ = 0x03,		//!< Read Data.
    FRD = 0x0b,			//!< Fast Read.
    FRDDIO = 0xbb,	        //!< Fast Read Dual.
    PP = 0x02,			//!< Page Program.
    SER = 0x20,			//!< Sector Erase (4 kB).
    B32ER = 0x52,		//!< Block Erase (32 kB).
    B64ER = 0xd8,		//!< Block Erase (64kB).
    CER = 0x60,			//!< Chip Erase.
    PWD = 0xb9,			//!< Power-down
    RLPWD = 0xab,		//!< Release Power-down/Device ID.
    RDID = 0x90,		//!< Read Manufacturer/Device ID.
    RDIDDIO = 0x92,		//!< Read Manufacturer/Device ID Dual I/O.
    RDJID = 0x9f, 		//!< Read JEDEC ID.
    RDUID = 0x4b		//!< Read Unique ID.
  } __attribute__((packed));

  /** Manufacturer code */
  static const uint8_t MANUFACTURER = 0xef;

  /** Device code */
  static const uint8_t DEVICE = 0x12;

  /**
   * Issue given command and return result.
   * @param[in] cmd command code.
   * @return result.
   */
  uint8_t issue(Command cmd);

  /** Latest status; is_ready() call */
  status_t m_status;
};

#endif
