/**
 * @file Cosa/Soft/SPI.hh
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

#ifndef COSA_SOFT_SPI_HH
#define COSA_SOFT_SPI_HH

#include "Cosa/Types.h"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Interrupt.hh"

/**
 * Software version some of the AVR hardware.
 */
namespace Soft {

  /**
   * Soft Serial Peripheral Interface (SPI) device class.
   */
  class SPI {
  public:
    /** Clock selectors. */
    enum Clock {
      DIV2_CLOCK = 0x04,	//!< Divide system clock by 2.
      DIV4_CLOCK = 0x00,	//!< Divide system clock by 4.
      DIV8_CLOCK = 0x05,	//!< Divide system clock by 8.
      DIV16_CLOCK = 0x01,	//!< Divide system clock by 16.
      DIV32_CLOCK = 0x06,	//!< Divide system clock by 32.
      DIV64_CLOCK = 0x02,	//!< Divide system clock by 64.
      DIV128_CLOCK = 0x03,	//!< Divide system clock by 128.
      DEFAULT_CLOCK = DIV4_CLOCK //!< Default clock rate.
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
      ACTIVE_HIGH = 1,	   	//!< Active high logic during transaction.
      PULSE_LOW = 2, 	   	//!< Pulse low on end of transaction.
      PULSE_HIGH = 3,	   	//!< Pulse high on end of transaction.
      DEFAULT_PULSE = ACTIVE_LOW //!< Default is low logic.
    } __attribute__((packed));

    /**
     * SPI device driver abstract class. Holds SPI state to allow
     * handling of several SPI devices with different clock, mode and/or
     * bit order.
     */
    class Driver {
      friend class SPI;
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
	     Clock clock = DEFAULT_CLOCK,
	     uint8_t mode = 0,
	     Order order = MSB_ORDER,
	     Interrupt::Handler* irq = NULL);

      /**
       * Calculate SPI clock rate (scale factor) for given frequency.
       * @param[in] freq device max frequency (in Hz).
       * @return clock rate.
       */
      static Clock clock(uint32_t freq)
	__attribute__((always_inline))
      {
	if (freq >= (F_CPU / 2)) return (SPI::DIV2_CLOCK);
	if (freq >= (F_CPU / 4)) return (SPI::DIV4_CLOCK);
	if (freq >= (F_CPU / 8)) return (SPI::DIV8_CLOCK);
	if (freq >= (F_CPU / 16)) return (SPI::DIV16_CLOCK);
	if (freq >= (F_CPU / 32)) return (SPI::DIV32_CLOCK);
	if (freq >= (F_CPU / 64)) return (SPI::DIV64_CLOCK);
	return (SPI::DIV128_CLOCK);
      }

      /**
       * Calculate SPI clock rate (scale factor) for given clock
       * cycle time in nano seconds.
       * @param[in] ns min device clock cycle time.
       * @return clock rate.
       */
      static Clock cycle(uint16_t ns)
	__attribute__((always_inline))
      {
	if (ns <= (1000000L / (F_CPU/2000L))) return (SPI::DIV2_CLOCK);
	if (ns <= (1000000L / (F_CPU/4000L))) return (SPI::DIV4_CLOCK);
	if (ns <= (1000000L / (F_CPU/8000L))) return (SPI::DIV8_CLOCK);
	if (ns <= (1000000L / (F_CPU/16000L))) return (SPI::DIV16_CLOCK);
	if (ns <= (1000000L / (F_CPU/32000L))) return (SPI::DIV32_CLOCK);
	if (ns <= (1000000L / (F_CPU/64000L))) return (SPI::DIV64_CLOCK);
	return (SPI::DIV128_CLOCK);
      }

      /**
       * Set SPI master clock rate.
       * @param[in] clock rate.
       */
      void set_clock(Clock rate);

      /**
       * Set SPI master clock frequency.
       * @param[in] freq device max frequency (in Hz).
       */
      void set_clock(uint32_t freq)
        __attribute__((always_inline))
      {
	set_clock(clock(freq));
      }

    protected:
      Driver* m_next;		//!< List of drivers.
      Interrupt::Handler* m_irq;//!< Interrupt handler.
      OutputPin m_cs;		//!< Device chip select pin.
      Pulse m_pulse;		//!< Chip select pulse mode.
      uint8_t m_mode;		//!< Mode for phase and transition.
      Order m_order;		//!< Data direction; bit order.
      uint8_t m_data;		//!< Data to transfer.
    };

  public:
    /**
     * Construct soft serial peripheral interface master.
     */
    SPI(Board::DigitalPin miso,
	Board::DigitalPin mosi,
	Board::DigitalPin sck) :
      m_list(0),
      m_busy(false),
      m_dev(0),
      m_miso(miso),
      m_mosi(mosi, 0),
      m_sck(sck, 0)
    {}

    /**
     * Attach given SPI device driver context.
     * @param[in] dev device driver context.
     * @return true(1) if successful otherwise false(0)
     */
    bool attach(Driver* dev);

    /**
     * Acquire the SPI device driver. Initiate SPI hardware registers
     * and disable SPI interrupt sources. The function will yield until
     * the device driver has been acquired. Interrupts from SPI devices
     * are disabled until the device driver is released. Used in the
     * below format for a device driver:
     * @code
     * spi.acquire(this)
     *   spi.begin();
     *     res = spi.transfer(data);
     *   spi.end();
     * spi.release();
     * @endcode
     * Each transfer that requires chip select should be enclosed in
     * a block with begin() and end(). There may be several transfer
     * blocks in a transaction. The transaction is terminated with
     * release().
     * @param[in] dev device driver context.
     */
    void acquire(Driver* dev);

    /**
     * Release the SPI device driver. Enable SPI interrupt sources.
     */
    void release();

    /**
     * Mark the beginning of a transfer block. Select the device by
     * asserting the chip select pin according to the pulse pattern.
     * Used in the format:
     * @code
     * spi.acquire(this)
     *   spi.begin();
     *     res = spi.transfer(data);
     *   spi.end();
     * spi.release();
     * @endcode
     * The transfer block should be terminated with end(). Typically
     * the transfer block is a command (read/write) with a possible
     * parameter block.
     */
    void begin()
      __attribute__((always_inline))
    {
      if (m_dev->m_pulse < PULSE_LOW) m_dev->m_cs.toggle();
    }

    /**
     * Mark the end of a transfer block. Deselect the device chip
     * according to the pulse pattern.
     */
    void end()
      __attribute__((always_inline))
    {
      m_dev->m_cs.toggle();
      if (m_dev->m_pulse > ACTIVE_HIGH) m_dev->m_cs.toggle();
    }

    /**
     * Start exchange data with slave. Should only be used within a SPI
     * transaction; begin()-end() block.
     * @param[in] data to send.
     */
    void transfer_start(uint8_t data)
    __attribute__((always_inline))
    {
      m_dev->m_data = data;
    }

    /**
     * Wait for exchange with slave. Should only be used within a SPI
     * transaction; begin()-end() block. Return received value.
     * @return value received.
     */
    uint8_t transfer_await()
      __attribute__((always_inline))
    {
      return (transfer(m_dev->m_data));
    }

    /**
     * Next data to exchange with slave. Should only be used within a SPI
     * transaction; begin()-end() block.
     * @param[in] data to send.
     * @return value received.
     */
    uint8_t transfer_next(uint8_t data)
      __attribute__((always_inline))
    {
      uint8_t res = transfer_await();
      transfer_start(data);
      return (res);
    }

    /**
     * Exchange data with slave. Slave select must be done before exchange
     * of data. Returns always zero(0) in soft variant.
     * @param[in] data to send.
     * @return zero
     */
    uint8_t transfer(uint8_t data);

    /**
     * Exchange package with slave. Received data from slave is stored
     * in given buffer. Should only be used within a SPI transfer;
     * begin()-end() block.
     * @param[in] buf with data to transfer (send/receive).
     * @param[in] count size of buffer.
     */
    void transfer(void* buf, size_t count);

    /**
     * Exchange package with slave. Received data from slave is stored
     * in given destination buffer. Should only be used within a SPI
     * transfer; begin()-end() block.
     * @param[in] dst destination buffer for received data.
     * @param[in] src source buffer with data to send.
     * @param[in] count size of buffers.
     */
    void transfer(void* dst, const void* src, size_t count)
    {
      if (UNLIKELY(count == 0)) return;
      uint8_t* dp = (uint8_t*) dst;
      const uint8_t* sp = (const uint8_t*) src;
      do *dp++ = transfer(*sp++); while (--count);
    }

    /**
     * Read package from the device slave. Should only be used within a
     * SPI transfer; begin()-end() block.
     * @param[in] buf buffer for read data.
     * @param[in] count number of bytes to read.
     */
    void read(void* buf, size_t count)
    {
      if (UNLIKELY(count == 0)) return;
      uint8_t* bp = (uint8_t*) buf;
      do *bp++ = transfer(0x00); while (--count);
    }

    /**
     * Write package to the device slave. Should only be used within a
     * SPI transaction; begin()-end() block.
     * @param[in] buf buffer with data to write.
     * @param[in] count number of bytes to write.
     */
    void write(const void* buf, size_t count)
    {
      if (UNLIKELY(count == 0)) return;
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
      if (UNLIKELY(count == 0)) return;
      do transfer(pgm_read_byte(buf++)); while (--count);
    }

    /**
     * Write null terminated io buffer vector to the device slave.
     * Should only be used  within a SPI transfer; begin()-end() block.
     * @param[in] vec null terminated io buffer vector pointer.
     */
    void write(const iovec_t* vec)
      __attribute__((always_inline))
    {
      for (const iovec_t* vp = vec; vp->buf != NULL; vp++)
	write(vp->buf, vp->size);
    }

  private:
    Driver* m_list;		//!< Attached devices interrupt disable/enable.
    volatile bool m_busy;	//!< SPI resource is busy.
    Driver* m_dev;		//!< Current device using the SPI pins.
    InputPin m_miso;		//!< Master Input Slave Output pin.
    OutputPin m_mosi;		//!< Master Output Slave Input pin.
    OutputPin m_sck;		//!< Serial Clock pin.
  };

  extern SPI spi;
};
#endif
