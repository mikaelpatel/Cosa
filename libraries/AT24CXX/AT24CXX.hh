/**
 * @file AT24CXX.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_AT24CXX_HH
#define COSA_AT24CXX_HH

#include "Cosa/Types.h"
#include "Cosa/TWI.hh"
#include "Cosa/EEPROM.hh"

/**
 * Driver for the AT24CXX 2-Wire Serial EEPROM. Allows page write and
 * block read. Supports device AT24C32 (8K) to AT24C512 (64K). Default
 * AT24CXX device is AT24C32.
 *
 * @section Circuit
 * The TinyRTC with DS1307 also contains a 24C32 EEPROM.
 * @code
 *                       TinyRTC(24C32)
 *                       +------------+
 *                     1-|SQ          |
 *                     2-|DS        DS|-1
 * (A5/SCL)------------3-|SCL      SCL|-2
 * (A4/SDA)------------4-|SDA      SDA|-3
 * (VCC)---------------5-|VCC      VCC|-4
 * (GND)---------------6-|GND      GND|-5
 *                     7-|BAT         |
 *                       +------------+
 * @endcode
 */
class AT24CXX : private TWI::Driver, public EEPROM::Device {
public:
  /**
   * Number of bytes on device.
   */
  const size_t SIZE;

  /**
   * Number of bytes in max write page size.
   */
  const uint16_t PAGE_MAX;

  /**
   * Construct AT24CXX serial TWI EEPROM device access to given
   * chip address, page and memory size.
   * @param[in] subaddr chip address (0..7, default 0).
   * @param[in] size in Kbits (default 32).
   * @param[in] page_max size of memory page (default 32 byte).
   */
  AT24CXX(uint8_t subaddr = 0,
	  const size_t size = 32,
	  const uint16_t page_max = 32) :
    TWI::Driver(0x50 | (subaddr & 0x07)),
    EEPROM::Device(),
    SIZE((size / CHARBITS) * 1024),
    PAGE_MAX(page_max),
    WRITE_MAX(page_max),
    WRITE_MASK(page_max - 1)
  {}

  /**
   * @override{EEPROM::Device}
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @return bool
   */
  virtual bool is_ready();

  /**
   * @override{EEPROM::Device}
   * Read rom block with the given size into the buffer from the address.
   * Return number of bytes read or negative error code.
   * @param[in] dest buffer to read from rom.
   * @param[in] src address in rom to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  virtual int read(void* dest, const void* src, size_t size);

  /**
   * @override{EEPROM::Device}
   * Write rom block at given address with the contents from the buffer.
   * Return number of bytes written or negative error code.
   * @param[in] dest address in rom to read write to.
   * @param[in] src buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  virtual int write(void* dest, const void* src, size_t size);

private:
  static const uint8_t POLL_MAX = 3;
  const uint16_t WRITE_MAX;
  const uint16_t WRITE_MASK;

  /**
   * Initiate TWI communication with memory device for access of
   * given memory address. If buffer is not null perform a write
   * to page. The given size must not exceed the page. Return true(1)
   * if the device is ready, write cycle is completed, otherwise
   * false(0).
   * @param[in] addr address in rom.
   * @param[in] buf buffer to write rom (default null(0)).
   * @param[in] size number to write (default zero(0)).
   * @return bool
   */
  bool poll(const void* addr, const void* buf = NULL, size_t size = 0);
};

/**
 * The AT24C32 provides 32,768 bits of serial electrically erasable
 * and programmable read only memory (EEPROM) organized as 4096 words
 * of 8 bits each. 32-Byte page write mode.
 *
 * See Atmel Product description (Rev. 0336K-SEEPR-7/03),
 * www.atmel.com/images/doc0336.pdf
 */
class AT24C32 : public AT24CXX {
public:
  AT24C32(uint8_t addr = 0) : AT24CXX(addr, 32, 32) {}
};

/**
 * The AT24C64 provides 65,536 bits of serial electrically erasable
 * and programmable read only memory (EEPROM) organized as 8192 words
 * of 8 bits each. 32-Byte page write mode.
 *
 * See Atmel Product description (Rev. 0336K-SEEPR-7/03),
 * www.atmel.com/images/doc0336.pdf
 */
class AT24C64 : public AT24CXX {
public:
  AT24C64(uint8_t addr = 0) : AT24CXX(addr, 64, 32) {}
};

/**
 * The AT24C128 provides 131,072 bits of serial electrically erasable
 * and programmable read only memory (EEPROM) organized as 16,384
 * words of 8 bits each. 64-Byte page write mode.
 *
 * See Atmel Product description (Rev. 0670T–SEEPR–3/07),
 * http://www.atmel.com/Images/doc0670.pdf
 */
class AT24C128 : public AT24CXX {
public:
  AT24C128(uint8_t addr = 0) : AT24CXX(addr, 128, 64) {}
};

/**
 * The AT24C256 provides 262,144 bits of serial electrically erasable
 * and programmable read only memory (EEPROM) organized as
 * 32,768 words of 8 bits each. 64-Byte page write mode.
 *
 * See Atmel Product description (Rev. 0670T–SEEPR–3/07),
 * http://www.atmel.com/Images/doc0670.pdf
 */
class AT24C256 : public AT24CXX {
public:
  AT24C256(uint8_t addr = 0) : AT24CXX(addr, 256, 64) {}
};

/**
 * The AT24C512 provides 524,288 bits of serial electrically erasable
 * and programmable read only memory (EEPROM) organized as 65,536
 * words of 8 bits each. 128-Byte page write mode.
 *
 * See Atmel Product description (Rev. 1116O–SEEPR–1/07),
 * http://www.atmel.com/Images/doc1116.pdf
 */
class AT24C512 : public AT24CXX {
public:
  AT24C512(uint8_t addr = 0) : AT24CXX(addr, 512, 128) {}
};
#endif
