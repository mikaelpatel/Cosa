/**
 * @file Cosa/TWI/AT23CXX.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Driver for the AT24CXX 2-Wire Serial EEPROM.
 * See Atmel Product description (Rev. 0336K-SEEPR-7/03).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TWI_AT24CXX_HH__
#define __COSA_TWI_AT24CXX_HH__

#include "Cosa/TWI.hh"

class AT24CXX : private TWI::Driver {

private:
  static const uint8_t ADDR = 0xa0;
  uint8_t _addr;

public:
  /**
   * Construct AT24CXX serial TWI EEPROM device access to given
   * chip address.
   * @param[in] addr chip address (0..7)
   */
  AT24CXX(uint8_t addr = 0) : _addr(ADDR | (addr & 0xe)) {}

  /**
   * Read rom block with the given size into the buffer from the address.
   * Return number of bytes read or negative error code.
   * @param[in] buf buffer to read from rom.
   * @param[in] size number of bytes to read.
   * @param[in] addr address in rom to read from.
   * @return number of bytes or negative error code.
   */
  int read(void* buf, uint8_t size, uint16_t addr);

  /**
   * Write rom block at given address with the contents from the buffer.
   * Return number of bytes written or negative error code.
   * @param[in] buf buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @param[in] addr address in rom to read write to.
   * @return number of bytes or negative error code.
   */
  int write(void* buf, uint8_t size, uint16_t addr);
};

#endif
