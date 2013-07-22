/**
 * @file Cosa/Soft/SPI.hh
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

#ifndef __COSA_SOFT_SPI_HH__
#define __COSA_SOFT_SPI_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"

/**
 * Software version some of the AVR hardware.
 */
namespace Soft {

  /**
   * Soft Serial Peripheral Interface (SPI) device class. 
   */
  class SPI {
  public:
    enum Clock {
      DIV4_CLOCK = 0x00,
      DIV16_CLOCK = 0x01,
      DIV64_CLOCK = 0x02,
      DIV128_CLOCK = 0x03,
      DIV2_2X_CLOCK = 0x04,
      DIV8_2X_CLOCK = 0x05,
      DIV32_2X_CLOCK = 0x06,
      DIV64_2X_CLOCK = 0x07,
      MASTER_CLOCK = 0x08,
      DEFAULT_CLOCK = 0x00
    } __attribute__((packed));
  
    enum Direction {
      MSB_FIRST = 0, 
      LSB_FIRST = 1
    } __attribute__((packed));

  private:
    Pin::Direction m_direction;
    OutputPin m_mosi;
    OutputPin m_sck;
  
  public:
    /**
     * Construct soft serial peripheral interface master.
     */
    SPI(Board::DigitalPin mosi, Board::DigitalPin sck) : 
      m_direction(Pin::MSB_FIRST),
      m_mosi(mosi, 0),
      m_sck(sck, 0)
    {
    }
  
    /**
     * Start master/slave serial send/receive block. 
     * @param[in] clock mode.
     * @param[in] mode data/clock sampling mode.
     * @param[in] direction data bit order.
     * @return true(1) if successful otherwise false(0)
     */
    bool begin(Clock clock = DEFAULT_CLOCK, 
	       uint8_t mode = 0, 
	       Direction direction = MSB_FIRST)
    {
      m_direction = (Pin::Direction) direction;
      return (true);
    }

    /**
     * Exchange data with slave. Slave select must be done before exchange
     * of data. Returns always zero(0) in soft variant.
     * @param[in] data to send.
     * @return zero
     */
    uint8_t exchange(uint8_t data)
    {
      m_mosi.write(data, m_sck, m_direction);
      return (0);
    }

    /**
     * Send given data to slave. Allow output operator syntax.
     * @param[in] data to send.
     * @return spi.
     */
    SPI& operator<<(uint8_t data)
    {
      exchange(data);
      return (*this);
    }

    /**
     * Exchange package with slave. Received data from slave is stored in
     * given buffer. Slave selection is done for package.
     * @param[in] buffer with data to exchange (send/receive).
     * @param[in] count size of buffer.
     */
    void exchange(void* buffer, uint8_t count)
    {
      uint8_t* bp = (uint8_t*) buffer;
      while (count--) exchange(*bp++);
    }

    /**
     * Exchange package in program memory to slave. Received data 
     * from slave is ignored. Slave selection is done for package.
     * @param[in] buffer with data in program memory.
     * @param[in] count size of buffer.
     */
    void exchange_P(const void* buffer, uint8_t count)
    {
      uint8_t* bp = (uint8_t*) buffer;
      while (count--) exchange(pgm_read_byte(bp++));
    }


    /**
     * Write data to slave device; send address/command and send data.
     * @param[in] cmd command.
     * @param[in] data to write.
     * @return status.
     */
    uint8_t write(uint8_t cmd, uint8_t data)
    {
      uint8_t status = exchange(cmd);
      exchange(data);
      return (status);
    }

    /**
     * Write data to slave device; send address/command and send data
     * from buffer.
     * @param[in] cmd command.
     * @param[in] buffer with data to send.
     * @param[in] count size of buffer.
     * @return status.
     */
    uint8_t write(uint8_t cmd, const void* buffer, uint8_t count)
    {
      exchange(cmd);
      exchange((void*) buffer, count);
      return (0);
    }
  
    /**
     * Write data to slave device; send address/command and send data 
     * from buffer in program memory.
     * @param[in] cmd command.
     * @param[in] buffer with data to send.
     * @param[in] count size of buffer.
     * @return status.
     */
    uint8_t write_P(uint8_t cmd, const void* buffer, uint8_t count)
    {
      exchange(cmd);
      exchange_P(buffer, count);
      return (0);
    }
  
    /**
     * End of master interaction.
     * @return true(1) if successful otherwise false(0)
     */
    bool end()
    {
      return (true);
    }

  };
};

#endif
