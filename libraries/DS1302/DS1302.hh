/**
 * @file DS1302.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_DS1302_HH
#define COSA_DS1302_HH

#include "Cosa/OutputPin.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/Time.hh"

/**
 * Cosa Device Driver for DS1302, Trickle-Charge Timekeeping Chip.
 *
 * @section Circuit
 * @code
 *                         DS1302/RTC
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (D2)----------------3-|CLK         |
 * (D3)----------------4-|DAT         |
 * (D4)----------------5-|RST         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. On-line product description,
 * http://www.maximintegrated.com/datasheet/index.mvp/id/2685
 * 2. Datasheet, http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
 */
class DS1302 {
public:
  /** Start address of clock/calender internal registers. */
  static const uint8_t RTC_START = 0;

  /** Static memory size. */
  static const size_t RAM_MAX = 31;

  /** Start address of static memory. */
  static const uint8_t RAM_START = 32;

  /** End address of static memory. */
  static const uint8_t RAM_END = RAM_START + RAM_MAX;

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
  {}

  /*
   * Low level RTC access function. Read data from the clock register
   * or static memory on device.
   * @param[in] addr device address.
   * @return data.
   */
  uint8_t read(uint8_t addr);

  /*
   * Low level RTC access function. Write given data to the clock
   * register or static memory on device.
   * @param[in] addr device address.
   * @param[in] data to write.
   */
  void write(uint8_t addr, uint8_t data);

  /**
   * Set write protect-bit according to flag.
   * @param[in] flag write protect mode.
   */
  void write_protect(bool flag)
    __attribute__((always_inline))
  {
    write(WP, flag ? 0x80 : 0x00);
  }

  /**
   * Read clock and calender from the device.
   * @param[in,out] now time structure for return value.
   */
  void get_time(time_t& now);

  /**
   * Write clock and calender to the device.
   * @param[in] now time to set.
   */
  void set_time(time_t& now);

  /**
   * Read given static memory address on the device and return byte.
   * @param[in] addr memory address on the device (0..RAM_MAX-1).
   */
  uint8_t read_ram(uint8_t addr)
    __attribute__((always_inline))
  {
    return (read(RAM_START | (addr & ADDR_MASK)));
  }

  /**
   * Write given data to the static memory (31 bytes). Requires
   * handling of write protect (write_protect).
   * @param[in] addr memory address (0..RAM_MAX-1).
   * @param[in] data to write to the memory address.
   */
  void write_ram(uint8_t addr, uint8_t data)
    __attribute__((always_inline))
  {
    write(RAM_START | (addr & ADDR_MASK), data);
  }

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
   * zero(0) and includes handling of write protect.
   * @param[in] buf pointer to memory block to write.
   * @param[in] size number of bytes to write (max RAM_MAX(31)).
   */
  void write_ram(void* buf, size_t size);

protected:
  /** Write protect register. */
  static const uint8_t WP = 0x07;

  /** Command byte. */
  enum {
    WRITE = 0x80,		//!< Read/write bit in write mode.
    READ = 0x81,		//!< Read/write bit in read mode.
    RTC_BURST = 0xbe,		//!< RTC register burst transfer.
    RAM_BURST = 0xfe,		//!< RAM burst transfer.
    ADDR_MASK = 0x3f		//!< Mask address bits.
  } __attribute__((packed));

  OutputPin m_cs;		//!< Chip select, asserted high.
  IOPin m_sda;			//!< Serial data, bidirectional.
  OutputPin m_clk;		//!< Clock for synchronized data.

  /**
   * Low level read data from the device. Internal transfer
   * function. Used within a chip select block. Data direction must be
   * set before calling this function.
   * @return data.
   * @note atomic
   */
  uint8_t read();

  /**
   * Write low level data to the device. Internal transfer
   * function. Used within a chip select block.
   * @param[in] data to write to the device.
   * @note atomic
   */
  void write(uint8_t data);
};

#endif


