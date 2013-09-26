/**
 * @file Cosa/SPI.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_SPI_HH__
#define __COSA_SPI_HH__

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/Pins.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Event.hh"

/**
 * Serial Peripheral Interface (SPI) device class. A device driver should
 * inherit from SPI::Driver and defined SPI commands and higher level
 * functions. The SPI::Driver class supports multiple SPI devices with
 * possible different configuration (clock, bit order, mode) and
 * integrates with both device chip select and possible interrupt pins.
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
   * SPI device driver abstract class. Holds SPI/USI hardware settings 
   * to allow handling of several SPI devices with different clock, mode 
   * and/or bit order. Handles device chip select and disables/enables
   * interrupts during SPI transaction.
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
    /** Chip select pulse width; 
     *  0 for active low logic during the transaction,
     *  1 for active high logic,
     *  2 pulse on end of transaction.
     */
    uint8_t m_pulse;
    
#if defined(__ARDUINO_TINY__)
    /** SPI mode for clock polatity setting */
    const uint8_t m_mode;
    /** USI hardware control register setting */
    uint8_t m_usicr;
#else
    /** SPI hardware control register (SPCR) setting */
    uint8_t m_spcr;
    /** SPI hardware control bit in status register (SPSR) setting */
    uint8_t m_spsr;
#endif

  public:
    /**
     * Construct SPI Device driver with given chip select pin, pulse,
     * clock, mode, and bit order. Zero(0) pulse will give active low
     * chip select during transaction, One(1) acive high, otherwise
     * pulse on end(). 
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
	   Interrupt::Handler* irq = NULL);
  };

  /**
   * SPI slave device support. Allows Arduino/AVR to act as a hardware
   * device on the SPI bus. 
   */
  class Slave : public Interrupt::Handler, public Event::Handler {
    friend void SPI_STC_vect(void);
    friend class SPI;
  private:
    static const uint8_t DATA_MAX = 32;
    uint8_t m_data[DATA_MAX];
    uint8_t m_cmd;
    uint8_t* m_buffer;
    uint8_t m_max;
    uint8_t m_put;
    static Slave* s_device;
  public:
    /**
     * Construct serial peripheral interface for slave.
     * @param[in] buffer with data to received data.
     * @param[in] max size of buffer.
     */
    Slave(void* buffer = NULL, uint8_t max = 0) : 
      m_cmd(0),
      m_buffer((uint8_t*) buffer),
      m_max(max),
      m_put(0)
    {
      if (buffer == 0) {
	m_buffer = m_data;
	m_max = DATA_MAX;
      }
      s_device = this;
    }

    /**
     * Set data receive buffer for package receive mode.
     * @param[in] buffer pointer to buffer.
     * @param[in] max max size of data package.
     */
    void set_buf(void* buffer, uint8_t max) 
    { 
      if (buffer == 0) {
	m_buffer = m_data;
	m_max = DATA_MAX;
      }
      else {
	m_buffer = (uint8_t*) buffer; 
	m_max = max; 
      }
    }

    /**
     * Get data receive buffer for package receive mode.
     * @return buffer pointer to buffer.
     */
    void* get_buf()
    { 
      return (m_buffer);
    }
    
    /**
     * Get number of bytes available in receive buffer.
     * @return number of bytes.
     */
    uint8_t available()
    { 
      return (m_put);
    }

    /**
     * @override
     * Interrupt service on data receive in slave mode.
     * @param[in] data received data.
     */
    virtual void on_interrupt(uint16_t data);
  };

private:
  /** List of attached devices for interrupt disable/enable */
  Driver* m_list;
  /** Current device using the SPI hardware */
  Driver* m_dev;
  
public:
  /**
   * Construct serial peripheral interface for master.
   */
  SPI();

  /**
   * Construct serial peripheral interface for slave.
   */
  SPI(uint8_t mode, Order order);

  /**
   * Start of SPI master interaction block. Initiate SPI hardware 
   * registers, disable SPI interrupt sources and assert chip select
   * pin. Return true(1) if successful otherwise false(0) if the
   * hardware was currently in used.
   * @param[in] dev device driver context.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(Driver* dev);
  
  /**
   * End of SPI master interaction block. Deselect device and 
   * enable SPI interrupt sources.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Exchange data with slave. Should only be used within a SPI
   * transaction; begin()-end() block. Return received value.
   * @param[in] data to send.
   * @return value received.
   */
  uint8_t transfer(uint8_t data)
  {
#if defined(__ARDUINO_TINY__)
    USIDR = data;
    USISR = _BV(USIOIF);
    register uint8_t cntl = m_dev->m_usicr;
    do {
      USICR = cntl;
    } while ((USISR & _BV(USIOIF)) == 0);
    return (USIDR);
#else
    SPDR = data;
    loop_until_bit_is_set(SPSR, SPIF);
    return (SPDR);
#endif
  }

  /**
   * Exchange package with slave. Received data from slave is stored
   * in given buffer. Should only be used within a SPI transaction;
   * begin()-end() block.  
   * @param[in] buffer with data to transfer (send/receive).
   * @param[in] count size of buffer.
   */
  void transfer(void* buffer, size_t count)
  {
    uint8_t* bp = (uint8_t*) buffer;
    while (count--) {
      *bp = transfer(*bp);
      bp += 1;
    }
  }

  /**
   * Exchange package with slave. Received data from slave is stored
   * in given destination buffer. Should only be used within a SPI
   * transaction; begin()-end() block.  
   * @param[in] dst destination buffer for received data.
   * @param[in] src source buffer with data to send.
   * @param[in] count size of buffers.
   */
  void transfer(void* dst, const void* src, size_t count)
  {
    uint8_t* dp = (uint8_t*) dst;
    const uint8_t* sp = (const uint8_t*) src;
    while (count--) *dp++ = transfer(*sp++);
  }
};

/**
 * Singleton instance of the hardware SPI module
 */
extern SPI spi;

#endif
