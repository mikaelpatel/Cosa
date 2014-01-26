/**
 * @file Cosa/Driver/DS1302.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_DRIVER_DS1302_HH__
#define __COSA_DRIVER_DS1302_HH__

#include "Cosa/Pins.hh"
#include "Cosa/Time.hh"

/**
 * Cosa Device Driver for DS1302, Trickle-Charge Timekeeping Chip.
 *
 * @See Also
 * 1. On-line product description, 
 * http://www.maximintegrated.com/datasheet/index.mvp/id/2685
 * 2. Datasheet, http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
 */
class DS1302 {
private:
  /** Read/write address mask */
  static const uint8_t ADDR_MASK = 0x3f;

  /** Read/write bit in command byte */
  enum {
    WRITE = 0x00,
    READ = 0x01
  } __attribute__((packed));

  /** Chip select, asserted high during read and write */
  OutputPin m_cs;
  /** Serial data bidirectional data pin */
  IOPin m_sda;
  /** Clock for synchroized data movement on the serial interface */
  OutputPin m_clk;

  /**
   * Write data to the device. Internal transfer function. Used within
   * a chip select block.
   * @param[in] data to write to the device.
   */
  void write(uint8_t data);

  /**
   * Read data from the device. Internal transfer function. Used within
   * a chip select block. Data direction must be set before calling
   * this function. 
   * @return data.
   */
  uint8_t read();

public:
  /** Start address of clock/calender internal registers */
  static const uint8_t RTC_START = 0;

  /** Start address of static memory */
  static const uint8_t RAM_START = 32;

  /** Static memory size */
  static const size_t RAM_MAX = 31;

  /**
   * Construct device driver for DS1302 Real-Time Clock with the given
   * pins. 
   * @param[in] cs chip select pin (default D4).
   * @param[in] sda serial data pin (default D3).
   * @param[in] clk clock pin (default D2).
   */
  DS1302(Board::DigitalPin cs = Board::D4, 
	 Board::DigitalPin sda = Board::D3,
	 Board::DigitalPin clk = Board::D2) :
    m_cs(cs, 0),
    m_sda(sda, IOPin::OUTPUT_MODE),
    m_clk(clk, 0)
  {
  }

  /**
   * Write given data to the static memory (31 bytes) or
   * clock/calender register on the device.
   * @param[in] addr memory address.
   * @param[in] data to write to the memory address.
   */
  void write(uint8_t addr, uint8_t data);

  /**
   * Read given static memory address on the device and return byte.
   * @param[in] addr memory address on the device.
   */
  uint8_t read(uint8_t addr);

  /**
   * Set write protect-bit according to flag. 
   * @param[in] flag write protect mode.
   */
  void write_protect(bool flag);
  
  /**
   * Burst read memory block from the device starting at address
   * zero(0). Data block is returned in the given buffer.
   * @param[in] buf pointer to buffer to store data read.
   * @param[in] size number of bytes to read (max RAM_MAX(31)).
   */
  void read_ram(void* buf, size_t size);

  /**
   * Burst write data in buffer with given size to the static memory
   * in the device (max 31 bytes). Burst write is always from address 
   * zero(0). 
   * @param[in] buf pointer to memory block to write.
   * @param[in] size number of bytes to write (max RAM_MAX(31)).
   */
  void write_ram(void* buf, size_t size);

  /**
   * Write clock and calender to the device. 
   * @param[in] now time to set.
   */
  void set_time(time_t& now);

  /**
   * Read clock and calender from the device. 
   * @param[in,out] now time structure for return value.
   */
  void get_time(time_t& now);
};

#endif


