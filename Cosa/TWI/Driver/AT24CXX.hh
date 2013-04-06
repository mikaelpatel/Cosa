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
  static const uint16_t PAGE_MAX = 32;
  static const uint16_t WRITE_MAX = PAGE_MAX;
  static const uint16_t WRITE_MASK = (WRITE_MAX - 1);
  uint8_t m_addr;

  /**
   * Return true(1) if the device is ready, write cycle is completed,
   * otherwise false(0).
   * @param[in] addr address in rom.
   * @param[in] buf buffer to write rom.
   * @param[in] size number to write.
   * @return bool
   */
  bool poll(void* addr, void* buf = 0, size_t size = 0);

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
  int read(void* dest, void* src, size_t size);
  int read(unsigned char* dest, void* src) { return read(dest, src, sizeof(unsigned char)); }
  int read(unsigned short* dest, void* src) { return read(dest, src, sizeof(unsigned short)); }
  int read(unsigned int* dest, void* src) { return read(dest, src, sizeof(unsigned int)); }
  int read(unsigned long* dest, void* src) { return read(dest, src, sizeof(unsigned long)); }
  int read(char* dest, void* src) { return read(dest, src, sizeof(char)); }
  int read(short* dest, void* src) { return read(dest, src, sizeof(short)); }
  int read(int* dest, void* src) { return read(dest, src, sizeof(int)); }
  int read(long* dest, void* src) { return read(dest, src, sizeof(long)); }
  int read(float* dest, void* src) { return read(dest, src, sizeof(float)); }
  int read(double* dest, void* src) { return read(dest, src, sizeof(double)); }

  /**
   * Write rom block at given address with the contents from the buffer.
   * Return number of bytes written or negative error code.
   * @param[in] dest address in rom to read write to.
   * @param[in] src buffer to write to rom.
   * @param[in] size number of bytes to write.
   * @return number of bytes or negative error code.
   */
  int write(void* dest, void* src, size_t size);
  int write(void* dest, unsigned char* src) { return write(dest, src, sizeof(unsigned char)); }
  int write(void* dest, unsigned short* src) { return write(dest, src, sizeof(unsigned short));}
  int write(void* dest, unsigned int* src) { return write(dest, src, sizeof(unsigned int)); }
  int write(void* dest, unsigned long* src) { return write(dest, src, sizeof(unsigned long)); }
  int write(void* dest, char* src) { return write(dest, src, sizeof(char)); }
  int write(void* dest, short* src) { return write(dest, src, sizeof(short));}
  int write(void* dest, int* src) { return write(dest, src, sizeof(int)); }
  int write(void* dest, long* src) { return write(dest, src, sizeof(long)); }
  int write(void* dest, float* src) { return write(dest, src, sizeof(float)); }
  int write(void* dest, double* src) { return write(dest, src, sizeof(double)); }
};

/**
 * Symbolic addressing of structures in EEPROM in given section number.
 * @param[in] nr section number.
 */
#ifndef EEPROM
#define	EEPROM(nr) __attribute__((section(".eeprom" #nr)))
#endif

#endif
