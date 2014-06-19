/**
 * @file Cosa/SPI/Driver/S25FL127S.hh
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

#ifndef COSA_SPI_DRIVER_S25FL127S_HH
#define COSA_SPI_DRIVER_S25FL127S_HH

#include "Cosa/Types.h"
#include "Cosa/SPI.hh"

/**
 * Cosa SPANSINO S25FL127S flash device driver class. Implements disk
 * driver connect/disconnect, erase, read and write block.
 * 
 * @section References
 * 1. S25FL127S, 128 Mbit (16 Mbyte) MirrorBit(R) Flash Non-Volatile
 * Memory, CMOS 3.0 Volt Core, Serial Peripheral Interface with
 * Mult-I/O, Data Sheet, Pub.nr. S25127S_00, Rev. 05, Issue Date
 * Nov. 15, 2013.
 * 
 */
class S25FL127S : private SPI::Driver {
public:
  /**
   * Construct S25FL127S device driver with given chip select pin. 
   * @param[in] csn chip select pin (default D5/D3).
   */
#if defined(BOARD_ATTINYX5)
  S25FL127S(Board::DigitalPin csn = Board::D3) :
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0, SPI::MSB_ORDER, NULL)
  {}
#else
  S25FL127S(Board::DigitalPin csn = Board::D5) :
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV2_CLOCK, 0, SPI::MSB_ORDER, NULL)
  {}
#endif

  /**
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @return bool
   */
  bool is_ready();

  /**
   * Read flash block with the given size into the buffer from the
   * source address. Return number of bytes read or negative error
   * code. 
   * @param[in] dest buffer to read from flash into.
   * @param[in] src address in flash to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  int read(void* dest, uint32_t src, size_t size);

  /**
   * Erase given 4 Kbyte block. Returs zero(0) if successful otherwise an
   * negative error code(-1).
   * @param[in] dest destination block to erase.
   * @return zero or negative error code.
   */
  int erase(uint32_t dest);

  /**
   * Write flash block at given destination address with the contents
   * of the source buffer. Return number of bytes written or negative
   * error code. 
   * @param[in] dest address in flash to write to.
   * @param[in] src buffer to write to flash.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  int write(uint32_t dest, const void* src, size_t size);

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
    P4E4 = 0x21,		//!< Parameter 4-kB, sector erase.
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
  
  /**
   * Status Register#1 (SR1) bitfields (Table 8.5, pp. 57-58).
   */
  union status_t {
    uint8_t as_uint8;		//!< As unsigned 8-bit value.
    struct {			//!< As bit-fields.
      uint8_t WIP:1;		//!< Write In Progress.
      uint8_t WEL:1;		//!< Write Enable Latch.
      uint8_t BP:3;		//!< Block Protection.
      uint8_t E_ERR:1;		//!< Erase Error Occurred.
      uint8_t P_ERR:1;		//!< Programming Error Occurred.
      uint8_t SRWD:1;		//!< Status Register Write Disable.
    };
  };
  
  /** Latest status; is_ready() call */
  status_t m_status;
};

#endif
