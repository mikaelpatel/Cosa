/**
 * @file S25FL127S.hh
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

#ifndef COSA_S25FL127S_HH
#define COSA_S25FL127S_HH

#include "Cosa/Types.h"
#include "Cosa/SPI.hh"
#include "Cosa/Flash.hh"

/**
 * Cosa SPANSINO S25FL127S flash device driver class. Implements
 * the Cosa Flash device driver interface with erase, read and
 * write/program flash memory blocks.
 *
 * @section References
 * 1. S25FL127S, 128 Mbit (16 Mbyte) MirrorBit(R) Flash Non-Volatile
 * Memory, CMOS 3.0 Volt Core, Serial Peripheral Interface with
 * Mult-I/O, Data Sheet, Pub.nr. S25127S_00, Rev. 05, Issue Date
 * Nov. 15, 2013.
 */
class S25FL127S : public Flash::Device, protected SPI::Driver {
public:
  /**
   * Default programming page buffer size (pp. 61, 97).
   */
  static const size_t PAGE_MAX = 256;
  static const size_t PAGE_MASK = PAGE_MAX - 1;

  /**
   * Construct S25FL127S device driver with given chip select pin.
   * @param[in] csn chip select pin (default D5/D3).
   */
#if !defined(BOARD_ATTINYX5)
  S25FL127S(Board::DigitalPin csn = Board::D5) :
    Flash::Device(64 * 1024L, 256),
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0, SPI::MSB_ORDER, NULL)
  {}
#else
  S25FL127S(Board::DigitalPin csn = Board::D3) :
    Flash::Device(64 * 1024L, 256),
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
   * erased sector is either 4 or 64 KB. Give sector size 255 to erase
   * chip. The default configuration consists of 16X4 KB sectors from
   * low address followed by 255X64 KB sectors. The highest sector is
   * reserved. Returs zero(0) if successful otherwise an negative
   * error code (EINVAL on illegal sector size, EFAULT if not
   * successful).
   * @param[in] dest destination block byte address to erase.
   * @return zero or negative error code.
   */
  virtual int erase(uint32_t dest, uint8_t size = 4);

  /**
   * @override{Flash::Device}
   * Write flash block at given destination address with the contents
   * of the source buffer. Return number of bytes written or negative
   * error code (EFAULT if not successful).
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
   * written or negative error code (EFAULT is not successful).
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write_P(uint32_t dest, const void* buf, size_t size);

  /**
   * Configuration Register 1 (CR1) bitfields (Table 8.6, pp. 59).
   */
  union config_t {
    uint8_t as_uint8;		//!< As unsigned 8-bit value.
    struct {			//!< As bit-fields.
      uint8_t FREEZE:1;		//!< Lock current statos of BP2-0 bits.
      uint8_t QUAD:1;		//!< Quad I/O operation.
      uint8_t TBPARAM:1;	//!< Parameter Sectors location (0=low,1=high).
      uint8_t BPNV:1;		//!< BP2-0 in status register (0=NV,1=Volatile).
      uint8_t RFU:1;		//!< Reserved for Future Use.
      uint8_t TBPROT:1;		//!< Start of Block Protection (0=high,1=low).
      uint8_t LC:2;		//!< Latency Code (see table 8.7, pp. 60).
    };
    config_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Read device configuration register 1.
   * @return configuration.
   */
  uint8_t read_config()
  {
    return (issue(RDCR));
  }

  /**
   * Status Register 1 (SR1) bitfields (Table 8.5, pp. 57-58).
   */
  union status1_t {
    uint8_t as_uint8;		//!< As unsigned 8-bit value.
    struct {			//!< As bit-fields.
      uint8_t WIP:1;		//!< Write In Progress.
      uint8_t WEL:1;		//!< Write Enable Latch.
      uint8_t BP:3;		//!< Block Protection.
      uint8_t E_ERR:1;		//!< Erase Error Occurred.
      uint8_t P_ERR:1;		//!< Programming Error Occurred.
      uint8_t SRWD:1;		//!< Status Register Write Disable.
    };
    status1_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Read device status register 1.
   * @return status code.
   */
  uint8_t read_status1()
  {
    return (issue(RDSR1));
  }

  /**
   * Status Register 2 (SR2) bitfields (Table 8.8, pp. 61).
   */
  union status2_t {
    uint8_t as_uint8;		//!< As unsigned 8-bit value.
    struct {			//!< As bit-fields.
      uint8_t PS:1;		//!< Program suspend.
      uint8_t ES:1;		//!< Erase suspend.
      uint8_t RFU:3;		//!< Reserved for Future Use.
      uint8_t IO3:1;	        //!< IO3 alternative function (0=RESET,1=HOLD).
      uint8_t PBW:1;		//!< Page Buffer Wrap (0=256B,1=512B).
      uint8_t BES:1;		//!< Block Erase Size (0=64,1=256 kB).
    };
    status2_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Read device status register 2.
   * @return status code.
   */
  uint8_t read_status2()
  {
    return (issue(RDSR2));
  }

protected:
  /**
   * Command Set (table 10.2, pp. 73-74)
   */
  enum Command {
    /** Read Device Identification */
    READ_ID = 0x90,		//!< Read Electronic Manufacturer Signature.
    RDRID = 0x9f, 		//!< Read ID (JEDEC).
    RSFDP = 0x5a,		//!< Read JEDEC Serial Flash Parameters.
    RES = 0xab,			//!< Read Electronic Signature.

    /** Register Access */
    RDSR1 = 0x05,		//!< Read Status Register#1.
    RDSR2 = 0x07,		//!< Read Status Register#2.
    RDCR = 0x35,		//!< Read Configuration Register#1.
    WRR = 0x01,			//!< Write Register (Status-1, Config-1).
    WRDI = 0x04,		//!< Write Disable.
    WREN = 0x06,		//!< Write Enable.
    CLSR = 0x30,		//!< Clear Status Register#1
    ABRD = 0x14,		//!< AutoBoot Register Read.
    ABWR = 0x15,		//!< AutoBoot Register Write.
    BRRD = 0x16,		//!< Bank Register Read.
    BRWR = 0x17,		//!< Bank Register Write.
    BRAC = 0x89,		//!< Bank Register Access.
    DLPRD = 0x41,		//!< Data Learning Pattern Read.
    PNVDLR = 0x43,		//!< Program NV Data Learning Register.
    WVDLR = 0x4A,		//!< Write Volatile Data Learning Register.

    /** Read Flash Array */
    READ = 0x03,		//!< Read (3- or 4-byte address).
    READ4 = 0x13,		//!< Read (4-byte address).
    FAST_READ = 0x0b,		//!< Fast Read (3- or 4-byte address).
    FAST_READ4 = 0x0c,		//!< Fast Read (4-byte address).
    DOR = 0x3b,			//!< Read Dual Out (3- or 4-byte address).
    DOR4 = 0x3c,		//!< Read Dual Out (4-byte address).
    QOR = 0x6b,			//!< Read Quad Out (3- or 4-byte address).
    QOR4 = 0x6c,		//!< Read Quad Out (4-byte address).
    DIOR = 0xbb,	        //!< Dual I/O Read (3- or 4-byte address).
    DIOR4 = 0xbc,		//!< Dual I/O Read (4-byte address).
    QIOR = 0xeb,		//!< Read Quad Out (3- or 4-byte address).
    QIOR4 = 0xec,		//!< Read Quad Out (4-byte address).

    /** Program Flash Array */
    PP = 0x02,			//!< Page Program  (3- or 4-byte address).
    PP4 = 0x12,			//!< Page Program  (4-byte address).
    QPP = 0x32,			//!< Quad Page Program  (3- or 4-byte address).
    QPP4 = 0x34,		//!< Quad Page Program  (4-byte address).
    PGSP = 0x85,		//!< Program Suspend.
    PGRS = 0x8a,		//!< Program Resume.

    /** Erase Flash Array */
    P4E = 0x20,			//!< Parameter 4-kB, sector erase.
    P4E4 = 0x21,		//!< Parameter 4-kB, sector erase (4-byte addr).
    BER = 0x60,			//!< Bulk Erase.
    SER = 0xd8,			//!< Erase 64 kB or 256 kB.
    SER4 = 0xdc,		//!< Erase 64 kB or 256 kB (4-byte addr).
    ERSP = 0x75,		//!< Erase Suspend.
    ERRS = 0x7a,		//!< Erase Resume.

    /** One Time Program Array */
    OTPP = 0x42,		//!< OTP Program.
    OTPR = 0x4b,		//!< OTP Read.

    /** Advanced Sector Protection */
    DYBRD = 0xe0,		//!< DYB Read.
    DYBWR = 0xe1,		//!< DYB Write.
    PPBRD = 0xe2,		//!< PPB Read
    PPBP = 0xe3,		//!< PPB Program.
    PPBE = 0xe4,		//!< PPB Erase.
    ASPRD = 0x2b,		//!< ASP Read.
    ASPP = 0x2f,		//!< ASP Program.
    PLBRD = 0xa7,		//!< PPB Lock Bit Read.
    PLBWR = 0xa6,		//!< PPB Lock Bit Write.
    PASSRD = 0xe7,		//!< Password Read.
    PASSP = 0xe8,		//!< Password Program.
    PASSU = 0xe9, 		//!< Password unlock.

    /** Reset */
    RESET = 0xf0,		//!< Software Reset.
    MBR = 0xff			//!< Mode Bit Reset.
  } __attribute__((packed));

  /** Manufacturer code */
  static const uint8_t MANUFACTURER = 0x01;

  /** Device code */
  static const uint8_t DEVICE = 0x17;

  /**
   * Issue given command and return result.
   * @param[in] cmd command code.
   * @return result.
   */
  uint8_t issue(Command cmd);

  /** Latest status; is_ready() call */
  status1_t m_status;
};

#endif
