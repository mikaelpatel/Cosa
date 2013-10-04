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
    /** Clock selectors */
    enum Clock {
      DIV4_CLOCK = 0x00,
      DIV16_CLOCK = 0x01,
      DIV64_CLOCK = 0x02,
      DIV128_CLOCK = 0x03,
      DIV2_2X_CLOCK = 0x04,
      DIV8_2X_CLOCK = 0x05,
      DIV32_2X_CLOCK = 0x06,
      DIV64_2X_CLOCK = 0x07,
      DEFAULT_CLOCK = DIV4_CLOCK
    } __attribute__((packed));

    /** Bit order selectors */
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
      /** List of drivers */
      Driver* m_next;
      /** Interrupt handler */
      Interrupt::Handler* m_irq;
      /** Device chip select pin */
      OutputPin m_cs;
      /** Chip select pulse mode; 
       *  0 for active low logic during the transaction,
       *  1 for active high logic,
       *  2 pulse at end of transaction.
       */
      uint8_t m_pulse;
      /** Data direction; bit order */
      Pin::Direction m_direction;
      
    public:
      /**
       * Construct SPI Device driver with given chip select pin, pulse,
       * clock, mode, and bit order. Zero(0) pulse will give active low
       * chip select during transaction, One(1) acive high, otherwise
       * pulse width on end(). 
       * @param[in] cs chip select pin.
       * @param[in] pulse chip select pulse mode (default active low, 0).
       * @param[in] clock SPI hardware setting (default DIV4_CLOCK).
       * @param[in] mode SPI mode for phase and transition (0..3, default 0).
       * @param[in] order bit order (default MSB_ORDER).
       * @param[in] irq interrupt handler (default null).
       */
      Driver(Board::DigitalPin cs, 
	     uint8_t pulse = 0,
	     Clock clock = DEFAULT_CLOCK, 
	     uint8_t mode = 0, 
	     Order order = MSB_ORDER,
	     Interrupt::Handler* irq = 0) :
	m_irq(irq),
	m_cs(cs, (pulse == 0)),
	m_pulse(pulse),
	m_order(order)
      {
	m_next = spi.m_list;
	spi.m_list = this;
      }
    };

  private:
    /** List of attached devices for interrupt disable/enable */
    Driver* m_list;
    /** Current device using the SPI hardware */
    Driver* m_dev;
    /** Master Output Slave Input pin */
    OutputPin m_mosi;
    /** Serial Clock pin */
    OutputPin m_sck;
  
  public:
    /**
     * Construct soft serial peripheral interface master.
     */
    SPI(Board::DigitalPin mosi, Board::DigitalPin sck) : 
      m_list(0),
      m_dev(0),
      m_mosi(mosi, 0),
      m_sck(sck, 0)
    {
    }
  
    /**
     * Start of SPI master interaction block. Sisable SPI interrupt
     * sources and assert chip select pin. Return true(1) if
     * successful otherwise false(0) if the hardware was currently in
     * used. 
     * @param[in] dev device driver context.
     * @return true(1) if successful otherwise false(0)
     */
    bool begin(Driver* dev)
    {
      synchronized {
	if (m_dev != 0) synchronized_return (false);
	// Acquire the driver controller
	m_dev = dev;
	// Enable device
	if (dev->m_pulse < 2) dev->m_cs.toggle();
	// Disable all interrupt sources on SPI bus
	for (dev = spi.m_list; dev != 0; dev = dev->m_next)
	  if (dev->m_irq) dev->m_irq->disable();
      }
      return (true);
    }
  
    /**
     * End of SPI master interaction block. Deselect device and 
     * enable SPI interrupt sources.
     * @return true(1) if successful otherwise false(0)
     */
    bool end() 
    {
      synchronized {
	if (m_dev == 0) synchronized_return (false);
	// Disable the device or give pulse if required
	m_dev->m_cs.toggle();
	if (m_dev->m_pulse > 1) m_dev->m_cs.toggle();
	// Enable the bus devices with interrupts
	for (Driver* dev = spi.m_list; dev != 0; dev = dev->m_next)
	  if (dev->m_irq != 0) dev->m_irq->enable();
	// Release the driver controller
	m_dev = 0;
      }
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
     * Write package to the device slave. Should only be used within a
     * SPI transaction; begin()-end() block.  
     * @param[in] buf buffer with data to write.
     * @param[in] count number of bytes to write.
     */
    void write(const void* buf, size_t count)
    {
      if (count == 0) return;
      const uint8_t* bp = (const uint8_t*) buf;
      do transfer(*bp++); while (--count);
    }

    /**
     * Write package to the device slave. Should only be used within a
     * SPI transaction; begin()-end() block.  
     * @param[in] buf buffer with data to write.
     * @param[in] count number of bytes to write.
     */
    void write_P(const uint8_t* buf, size_t count)
    {
      if (count == 0) return;
      do transfer(pgm_read_byte(buf++)); while (--count);
    }
  }
};
#endif
