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
    /** Clock selectors. MASTER_CLOCK for slave mode is not supported */
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
      DEFAULT_CLOCK = DIV4_CLOCK
    } __attribute__((packed));

    /** Bitorder selectors */
    enum Order {
      MSB_ORDER = 0, 
      LSB_ORDER = 1,
      DEFAULT_ORDER = MSB_ORDER
    } __attribute__((packed));
  
    /**
     * SPI device driver abstract class. Holds SPI/USI state to allow 
     * handling of several SPI devices with different clock, mode and/or
     * bit order. 
     */
    class Driver {
      friend class SPI;
    protected:
      /** Device chip select pin; initiated high for active low logic */
      OutputPin m_cs;
      /** Mutual exclusion flag. Require lock during transaction */
      const bool m_mutex;
      /** Data direction; bit order */
      Pin::Direction m_direction;
      
    public:
      /**
       * Construct SPI Device driver with given chip select pin, clock, 
       * mode, and bit order. The chip select pin is initiated for 
       * active low logic. 
       * @param[in] cs chip select pin.
       * @param[in] flag lock during transaction (default false).
       * @param[in] clock SPI hardware setting (default DIV4_CLOCK).
       * @param[in] mode SPI mode for phase and transition (0..3, default 0).
       * @param[in] order bit order (default MSB_ORDER).
       */
      Driver(Board::DigitalPin cs, 
	     bool flag = false, 
	     Clock clock = DEFAULT_CLOCK, 
	     uint8_t mode = 0, 
	     Order order = MSB_ORDER);
    };

  private:
    Driver* m_dev;
    uint8_t m_key;
    OutputPin m_mosi;
    OutputPin m_sck;
  
  public:
    /**
     * Construct soft serial peripheral interface master.
     */
    SPI(Board::DigitalPin mosi, Board::DigitalPin sck) : 
      m_dev(0),
      m_key(0),
      m_mosi(mosi, 0),
      m_sck(sck, 0)
    {
    }
  
    /**
     * Start master serial interaction block. 
     * @param[in] dev device driver context.
     * @return true(1) if successful otherwise false(0)
     */
    bool begin(Driver* dev)
    {
      // Sanity check that the SPI hardware is free
      if (m_dev != 0) return (false);
  
      // Initiate the SPI interaction; disable interrupt and enable device
      if (dev->m_mutex) m_key = lock();
      m_dev = dev;
      m_dev->m_cs.toggle();
      return (true);
    }
  
    /**
     * End of master interaction.
     * @return true(1) if successful otherwise false(0)
     */
    bool end() 
    {
      // Sanity check device driver reference
      if (m_dev == 0) return (false);

      // Release SPI hardware and enable interrups
      bool flag = m_dev->m_mutex;
      m_dev->m_cs.toggle();
      m_dev = 0;
      if (flag) unlock(m_key);
      return (true);
    }

    /**
     * Exchange data with slave. Slave select must be done before exchange
     * of data. Returns always zero(0) in soft variant.
     * @param[in] data to send.
     * @return zero
     */
    uint8_t transfer(uint8_t data)
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
      transfer(data);
      return (*this);
    }

    /**
     * Exchange package with slave. Received data from slave is stored in
     * given buffer. Slave selection is done for package.
     * @param[in] buffer with data to exchange (send/receive).
     * @param[in] count size of buffer.
     */
    void transfer(void* buffer, uint8_t count)
    {
      uint8_t* bp = (uint8_t*) buffer;
      while (count--) {
	*bp = transfer(*bp);
	bp += 1;
      }
    }

    /**
     * Write data to slave device.
     * @param[in] data to write.
     * @return status.
     */
    uint8_t write(uint8_t data)
    {
      return (transfer(data));
    }
    
    /**
     * Write data to slave device; send address/command and send data.
     * @param[in] cmd command.
     * @param[in] data to write.
     * @return status.
     */
    uint8_t write(uint8_t cmd, uint8_t data)
    {
      uint8_t status = transfer(cmd);
      transfer(data);
      return (status);
    }

    /**
     * Write data buffer to slave device.
     * @param[in] buffer with data to send.
     * @param[in] count size of buffer.
     */
    void write(const void* buffer, uint8_t count)
    {
      uint8_t* bp = (uint8_t*) buffer; 
      while (count--) transfer(*bp++);
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
      uint8_t status = transfer(cmd);
      write(buffer, count);
      return (status);
    }
  };
};

#endif
