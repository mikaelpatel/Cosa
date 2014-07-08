/**
 * @file Cosa/SPI.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#ifndef COSA_SPI_HH
#define COSA_SPI_HH

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/OutputPin.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Event.hh"

/**
 * Serial Peripheral Interface (SPI) device class. A device driver should
 * inherit from SPI::Driver and defined SPI commands and higher level
 * functions. The SPI::Driver class supports multiple SPI devices with
 * possible different configuration (clock, bit order, mode) and
 * integrates with both device chip select and possible interrupt pins.
 *
 * @section Circuit
 * SPI slave circuit with chip select and interrupt pin. Note that Tiny
 * uses USI but the software interface is the same but MOSI/MISO pins 
 * are DI/DO. Do not confuse with SPI chip programming pins on Tiny.
 * @code
 *                          SPI Slave
 *                       +------------+
 * (Dn)----------------1-|CSN         |
 * (D11/MOSI)----------2-|MOSI        |
 * (D12/MISO)----------3-|MISO        |
 * (D13/SCK)-----------4-|SCK         |
 * (EXTn)--------------5-|IRQ(opt)    |
 * (VCC)---------------6-|VCC         |
 * (GND)---------------7-|GND         |
 *                       +------------+
 * @endcode
 */
class SPI {
public:
  /** Clock selectors. */
  enum Clock {
    DIV2_CLOCK = 0x04,
    DIV4_CLOCK = 0x00,
    DIV8_CLOCK = 0x05,
    DIV16_CLOCK = 0x01,
    DIV32_CLOCK = 0x06,
    DIV64_CLOCK = 0x02,
    DIV128_CLOCK = 0x03,
    DEFAULT_CLOCK = DIV4_CLOCK
  } __attribute__((packed));

  /** Bit order selectors. */
  enum Order {
    MSB_ORDER = 0, 		//!< Most significant bit first.
    LSB_ORDER = 1,		//!< Least significant bit first.
    DEFAULT_ORDER = MSB_ORDER	//!< Default is MSB.
  } __attribute__((packed));
  
  /** Chip select mode. */
  enum Pulse {
    ACTIVE_LOW = 0,	   	//!< Active low logic during transaction.
    ACTIVE_HIGH = 1,	   	//!< Active high logic.
    PULSE_LOW = 2, 	   	//!< Pulse low on end of transaction.
    PULSE_HIGH = 3,	   	//!< Pulse high.
    DEFAULT_PULSE = ACTIVE_LOW	//!< Default is low logic.
  } __attribute__((packed));

  /**
   * SPI device driver abstract class. Holds SPI/USI hardware settings 
   * to allow handling of several SPI devices with different clock, mode 
   * and/or bit order. Handles device chip select and disables/enables
   * interrupts during SPI transaction.
   */
  class Driver {
  public:
    /**
     * Construct SPI Device driver with given chip select pin, pulse,
     * clock, mode, and bit order. 
     * @param[in] cs chip select pin.
     * @param[in] pulse chip select pulse mode (default ACTIVE_LOW).
     * @param[in] clock SPI hardware setting (default DIV4_CLOCK).
     * @param[in] mode SPI mode for phase and transition (0..3, default 0).
     * @param[in] order bit order (default MSB_ORDER).
     * @param[in] irq interrupt handler (default null).
     */
    Driver(Board::DigitalPin cs, 
	   Pulse pulse = DEFAULT_PULSE,
	   Clock rate = DEFAULT_CLOCK, 
	   uint8_t mode = 0, 
	   Order order = MSB_ORDER,
	   Interrupt::Handler* irq = NULL);
    
    /**
     * Set SPI master clock rate.
     * @param[in] clock rate.
     */
    void set_clock(Clock rate);

  protected:
    Driver* m_next;		//!< List of drivers.
    Interrupt::Handler* m_irq;	//!< Interrupt handler.
    OutputPin m_cs;		//!< Device chip select pin.
    Pulse m_pulse;		//!< Chip select pulse width.
#if defined(USICR)
    const uint8_t m_cpol;	//!< Clock polatity (CPOL) setting.
    uint8_t m_usicr;		//!< USI hardware control register setting.
    uint8_t m_data;		//!< Data register for asynchron transfer.
#else
    uint8_t m_spcr;		//!< SPI/SPCR hardware control register setting.
    uint8_t m_spsr;		//!< SPI/SPSR hardware status register.
#endif
    friend class SPI;
  };

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
   * Attach given SPI device driver context.
   * @param[in] dev device driver context.
   * @return true(1) if successful otherwise false(0)
   */
  bool attach(Driver* dev);
  
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

#if defined(USIDR)
  /**
   * Exchange data with slave. Should only be used within a SPI
   * transaction; begin()-end() block. Return received value.
   * @param[in] data to send.
   * @return value received.
   */
  uint8_t transfer(uint8_t data)
  {
    USIDR = data;
    USISR = _BV(USIOIF);
    register uint8_t cntl = m_dev->m_usicr;
    do {
      USICR = cntl;
    } while ((USISR & _BV(USIOIF)) == 0);
    return (USIDR);
  }

  /**
   * Start exchange data with slave. Should only be used within a SPI
   * transaction; begin()-end() block. 
   * @param[in] data to send.
   */
  void transfer_start(uint8_t data) __attribute__((always_inline))
  {
    m_dev->m_data = data;
  }

  /**
   * Wait for exchange with slave. Should only be used within a SPI
   * transaction; begin()-end() block. Return received value.
   * @return value received.
   */
  uint8_t transfer_await() __attribute__((always_inline))
  {
    return (transfer(m_dev->m_data));
  }

  /**
   * Next data to exchange with slave. Should only be used within a SPI
   * transaction; begin()-end() block. 
   * @param[in] data to send.
   * @return value received.
   */
  uint8_t transfer_next(uint8_t data) __attribute__((always_inline))
  {
    uint8_t res = transfer_await();
    transfer_start(data);
    return (res);
  }

#else
  /**
   * Exchange data with slave. Should only be used within a SPI
   * transaction; begin()-end() block. Return received value.
   * @param[in] data to send.
   * @return value received.
   */
  uint8_t transfer(uint8_t data) __attribute__((always_inline))
  {
    SPDR = data;
    loop_until_bit_is_set(SPSR, SPIF);
    return (SPDR);
  }

  /**
   * Start exchange data with slave. Should only be used within a SPI
   * transaction; begin()-end() block. 
   * @param[in] data to send.
   */
  void transfer_start(uint8_t data) __attribute__((always_inline))
  {
    SPDR = data;
  }

  /**
   * Wait for exchange with slave. Should only be used within a SPI
   * transaction; begin()-end() block. Return received value.
   * @return value received.
   */
  uint8_t transfer_await() __attribute__((always_inline))
  {
    loop_until_bit_is_set(SPSR, SPIF);
    return (SPDR);
  }

  /**
   * Next data to exchange with slave. Should only be used within a SPI
   * transaction; begin()-end() block. 
   * @param[in] data to send.
   * @return value received.
   */
  uint8_t transfer_next(uint8_t data) __attribute__((always_inline))
  {
    loop_until_bit_is_set(SPSR, SPIF);
    uint8_t res = SPDR;
    SPDR = data;
    return (res);
  }
#endif

  /**
   * Exchange package with slave. Received data from slave is stored
   * in given buffer. Should only be used within a SPI transaction;
   * begin()-end() block.  
   * @param[in] buf with data to transfer (send/receive).
   * @param[in] count size of buffer.
   */
  void transfer(void* buf, size_t count);

  /**
   * Exchange package with slave. Received data from slave is stored
   * in given destination buffer. Should only be used within a SPI
   * transaction; begin()-end() block.  
   * @param[in] dst destination buffer for received data.
   * @param[in] src source buffer with data to send.
   * @param[in] count size of buffers.
   */
  void transfer(void* dst, const void* src, size_t count);

  /**
   * Read package from the device slave. Should only be used within a
   * SPI transaction; begin()-end() block.  
   * @param[in] buf buffer for read data.
   * @param[in] count number of bytes to read.
   */
  void read(void* buf, size_t count);

  /**
   * Write package to the device slave. Should only be used within a
   * SPI transaction; begin()-end() block.  
   * @param[in] buf buffer with data to write.
   * @param[in] count number of bytes to write.
   */
  void write(const void* buf, size_t count);

  /**
   * Write package to the device slave. Should only be used within a
   * SPI transaction; begin()-end() block.  
   * @param[in] buf buffer with data to write.
   * @param[in] count number of bytes to write.
   */
  void write_P(const void* buf, size_t count);

  /**
   * Write null terminated io buffer vector to the device slave. 
   * Should only be used  within a SPI transaction; begin()-end() block.  
   * @param[in] vec null terminated io buffer vector pointer.
   */
  void write(const iovec_t* vec);

  /**
   * SPI slave device support. Allows Arduino/AVR to act as a hardware
   * device on the SPI bus. 
   */
  class Slave : public Interrupt::Handler, public Event::Handler {
  public:
    /**
     * Construct serial peripheral interface for slave.
     * @param[in] buf with data to received data.
     * @param[in] max size of buffer.
     */
    Slave(void* buf = NULL, uint8_t max = 0) : 
      m_cmd(0),
      m_buf((uint8_t*) buf),
      m_max(max),
      m_put(0)
    {
      if (buf == NULL) {
	m_buf = m_data;
	m_max = DATA_MAX;
      }
      s_device = this;
    }

    /**
     * Set data receive buffer for package receive mode.
     * @param[in] buf pointer to buffer.
     * @param[in] max max size of data package.
     */
    void set_buf(void* buf, uint8_t max) 
    { 
      if (buf == NULL) {
	m_buf = m_data;
	m_max = DATA_MAX;
      }
      else {
	m_buf = (uint8_t*) buf; 
	m_max = max; 
      }
    }

    /**
     * Get data receive buffer for package receive mode.
     * @return buf pointer to buffer.
     */
    void* get_buf() const
    { 
      return (m_buf);
    }
    
    /**
     * Get number of bytes available in receive buffer.
     * @return number of bytes.
     */
    uint8_t available() const
    { 
      return (m_put);
    }

    /**
     * @override Interrupt::Handler
     * Interrupt service on data receive in slave mode.
     * @param[in] data received data.
     */
    virtual void on_interrupt(uint16_t data);

  protected:
    static const uint8_t DATA_MAX = 32;
    uint8_t m_data[DATA_MAX];
    uint8_t m_cmd;
    uint8_t* m_buf;
    uint8_t m_max;
    uint8_t m_put;
    static Slave* s_device;
    friend void SPI_STC_vect(void);
    friend class SPI;
  };

private:
  Driver* m_list;		//!< List of attached device drivers.
  Driver* m_dev;		//!< Current device driver.
};

/**
 * Singleton instance of the hardware SPI module.
 */
extern SPI spi;

#endif
