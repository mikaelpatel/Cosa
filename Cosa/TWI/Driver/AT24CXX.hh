/**
 * @file Cosa/TWI/Driver/AT24CXX.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Driver for the AT24CXX 2-Wire Serial EEPROM. See Atmel Product
 * description (Rev. 0336K-SEEPR-7/03), www.atmel.com/images/doc0336.pdf
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TWI_DRIVER_AT24CXX_HH__
#define __COSA_TWI_DRIVER_AT24CXX_HH__

#include "Cosa/TWI.hh"
#include "Cosa/Types.h"
#include <avr/sleep.h>

class AT24CXX : private TWI::Driver {
private:
  static const uint8_t ADDR = 0xa0;
  static const uint8_t POLL_MAX = 2;
  static const uint8_t PAGE_MAX = 32;
  static const uint8_t WRITE_MAX = PAGE_MAX;
  static const uint8_t WRITE_MASK = (WRITE_MAX - 1);
  uint8_t m_addr;

  /**
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @param[in] addr address in rom.
   * @param[in] buf buffer to write rom.
   * @param[in] size number to write.
   * @return bool
   */
  bool poll(uint16_t addr, void* buf = 0, size_t size = 0);

public:
  /**
   * Construct AT24CXX serial TWI EEPROM device access to given
   * chip address.
   * @param[in] addr chip address (0..7)
   */
  AT24CXX(uint8_t addr = 0) : 
    m_addr(ADDR | ((addr & 0x7) << 1))
  {}

  /**
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @return bool
   */
  bool is_ready();

  /**
   * Wait for write to complete. 
   * @param[in] mode of sleep.
   */
  void write_await(uint8_t mode = SLEEP_MODE_IDLE);

  /**
   * Read rom block with the given size into the buffer from the address.
   * Return number of bytes read or negative error code.
   * @param[in] dest buffer to read from rom.
   * @param[in] src address in rom to read from.
   * @param[in] size number of bytes to read.
   * @return number of bytes or negative error code.
   */
  int read(void* dest, uint16_t src, size_t size);

  /**
   * Write rom block at given address with the contents from the buffer.
   * Return number of bytes written or negative error code.
   * @param[in] dest address in rom to read write to.
   * @param[in] src buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  int write(uint16_t dest, void* src, size_t size);
};

/**
 * Symbolic addressing of structures in EEPROM in given section number.
 * @param[in] nr section number.
 */
#ifndef EEPROM
#define	EEPROM(nr) __attribute__((section(".eeprom" #nr)))
#endif

#endif
