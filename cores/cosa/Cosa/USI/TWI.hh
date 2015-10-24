/**
 * @file Cosa/USI/TWI.hh
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

#ifndef COSA_USI_TWI_HH
#define COSA_USI_TWI_HH

#include "Cosa/Types.h"
#if defined(BOARD_ATTINY)
#include "Cosa/IOPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/Power.hh"
/**
 * Two wire library. Support for ATtiny I2C/TWI bus master and slave
 * devices using the USI hardware support. See also Cosa/TWI.hh. The
 * public interface should be maintained the same for portability.
 * Note: The internal pullup resistors on the USI pins are active.
 * External pullup resistors (4K7 ohm) are required for longer
 * wires and/or higher loads.
 */
class TWI {
public:
  /**
   * Device drivers are friends and may have callback/event handler
   * for completion events.
   */
  class Driver {
  public:
    /**
     * Construct TWI driver with given bus address.
     * @param[in] addr bus address (7-bit LSB).
     */
   Driver(uint8_t addr) :
     m_addr(addr << 1),
     m_async(false)
    {}

    /**
     * Return true(1) if the request is asyncrhonous otherwise false(0).
     */
    bool is_async() const
    {
      return (m_async);
    }

    /**
     * Set synchronous request mode.
     */
    void sync_request()
    {
      m_async = false;
    }

    /**
     * Set asynchronous request mode.
     */
    void async_request()
    {
      m_async = true;
    }

    /**
     * @override{TWI::Driver}
     * Service completion callback when a read/write has been
     * completed.
     * @param[in] type event code.
     * @param[in] count number of bytes in request.
     */
    virtual void on_completion(uint8_t type, int count)
    {
      UNUSED(type);
      UNUSED(count);
    }

  protected:
    /** Device bus address. */
    uint8_t m_addr;

    /** Asynchron mode. */
    bool m_async;

    /** Allow access. */
    friend class TWI;
    friend void ::USI_START_vect(void);
    friend void ::USI_OVF_vect(void);
  };

  /**
   * USI/TWI slave device. Handles incoming requests from TWI master;
   * performs address matching, byte data transfer with ack/nack,
   * and device callback.
   */
  class Slave : public Driver, public Event::Handler {
  public:
    /**
     * Construct slave with given address.
     * @param[in] addr slave address.
     */
    Slave(uint8_t addr) : TWI::Driver(addr), Event::Handler() {}

    /**
     * Set read (result) buffer. Must be called before starting
     * TWI.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    void read_buf(void* buf, size_t size);

    /**
     * Set write (argument) buffer. Must be called before starting
     * TWI.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    void write_buf(void* buf, size_t size);

    /**
     * Start the slave device.
     */
    void begin();

    /**
     * @override{TWI::Driver}
     * Service completion callback when a read/write has been
     * completed.
     * @param[in] type event code.
     * @param[in] count number of bytes in request.
     */
    virtual void on_completion(uint8_t type, int count)
    {
      Event::push(type, this, count);
    }

    /**
     * @override{TWI::Slave}
     * Service request callback when a write has been completed,
     * i.e., an argument block as been written. Must be defined by
     * sub-class. Must handle write-read and write-write
     * sequences. The device will become ready after the completion
     * of the function.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    virtual void on_request(void* buf, size_t size) = 0;

  protected:
    /**
     * Filter Event::WRITE_COMPLETED_TYPE(size) and calls on_request()
     * with given write block as argument. The device is marked as
     * ready when the request has been completed and a possible
     * result block is available.
     * @param[in] type the event type.
     * @param[in] value the event value.
     */
    virtual void on_event(uint8_t type, uint16_t value);

    /** Allow access. */
    friend void ::USI_START_vect(void);
    friend void ::USI_OVF_vect(void);
  };

  /**
   * Construct two-wire instance. This is actually a single-ton on
   * current supported hardware, i.e. there can only be one unit.
   */
  TWI();

  /**
   * Start TWI logic for a device transaction block.
   * @param[in] dev device.
   */
  void acquire(TWI::Driver* dev);

  /**
   * Stop usage of the TWI bus logic.
   * @return true(1) if successful otherwise false(0).
   */
  void release();

  /**
   * Write data to the current driver. Returns number of bytes written
   * or negative error code.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int write(void* buf, size_t size);

  /**
   * Write data to the current driver with given byte header. Returns
   * number of bytes written or negative error code.
   * @param[in] header to write before buffer.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int write(uint8_t header, void* buf = 0, size_t size = 0);

  /**
   * Write data to the current driver with given header. Returns
   * number of bytes written or negative error code.
   * @param[in] header to write before buffer.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int write(uint16_t header, void* buf = 0, size_t size = 0);

  /**
   * Read data to the current driver. Returns number of bytes read
   * or negative error code.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to read.
   * @return number of bytes
   */
  int read(void* buf, size_t size);

  /**
   * Set bus frequency (not implemented for USI).
   * @param[in] hz bus frequency.
   */
  void set_freq(uint32_t hz)
  {
    UNUSED(hz);
  }

  /**
   * Powerup USI.
   */
  void powerup()
  {
    power_usi_enable();
  }

  /**
   * Powerdown USI.
   */
  void powerdown()
  {
    power_usi_disable();
  }

private:
  /**
   * USI TWI slave states.
   */
  enum State {
    // Idle, waiting for start condition
    IDLE,
    // Check start condition
    START_CHECK,
    // Slave transmission states (Master read operation)
    READ_REQUEST,
    READ_COMPLETED,
    ACK_CHECK,
    // Slave receiver states (Master write operation)
    WRITE_REQUEST,
    WRITE_COMPLETED,
    // Slave service state (Response to write)
    SERVICE_REQUEST
  } __attribute__((packed));

  /**
   * Address mask and read/write bit.
   */
  enum {
    WRITE_OP = 0x00,		//!< Write operation.
    READ_OP = 0x01,		//!< Read operation.
    ADDR_MASK = 0xfe		//!< Address mask.
  } __attribute__((packed));

  /**
   * Status and Commands for USI hardware.
   */
  enum {
    // Clear all interrupt flags
    SR_CLEAR_ALL = _BV(USISIF) | _BV(USIOIF) | _BV(USIPF) | _BV(USIDC),
    // Clear all flags, except Start Condition.
    SR_CLEAR = _BV(USIOIF) | _BV(USIPF) | _BV(USIDC),
    // Clear flags. Set USI counter to shift 1 bit (2 edges)
    SR_CLEAR_ACK = SR_CLEAR | (0x0E << USICNT0),
    // Clear flags. Set USI counter to shift 8 bits (16 edges)
    SR_CLEAR_DATA = SR_CLEAR | (0x0 << USICNT0),
    // Set USI TWI mode(0). External clock source
    CR_SERVICE_MODE = _BV(USIWM1) |  _BV(USICS1),
    // Enable start condition. Set USI TWI mode(0). External clock source
    CR_START_MODE = _BV(USISIE) | _BV(USIWM1) |  _BV(USICS1),
    // Enable start and overflow. Set USI TWI mode(1). External clock
    CR_TRANSFER_MODE = _BV(USISIE) |  _BV(USIOIE) | _BV(USIWM1) | _BV(USIWM0)
                     | _BV(USICS1),
    // Master initialization. Software clock strobe
    CR_INIT_MODE =  _BV(USIWM1) | _BV(USICS1) | _BV(USICLK),
    // Master data transfer. Software clock strobe
    CR_DATA_MODE = _BV(USIWM1) | _BV(USICS1) | _BV(USICLK) | _BV(USITC)
  } __attribute__((packed));

  /**
   * Device state, data buffers and target.
   */
  static const uint8_t HEADER_MAX = 4;
  static const uint8_t VEC_MAX = 4;
  static const uint8_t WRITE_IX = 0;
  static const uint8_t READ_IX = 1;
  uint8_t m_header[HEADER_MAX];
  iovec_t m_vec[VEC_MAX];
  IOPin m_sda;
  IOPin m_scl;
  volatile State m_state;
  volatile uint8_t* m_next;
  volatile uint8_t* m_last;
  volatile int m_count;
  Driver* m_dev;
  volatile bool m_busy;

  /**
   * Get current driver state.
   * @return state
   */
  State state() const
  {
    return (m_state);
  }

  /**
   * Set driver state.
   * @param[in] state.
   */
  void state(State state)
  {
    m_state = state;
  }

  /**
   * Set data (SDA) pin input/output mode.
   * @param[in] mode.
   */
  void mode(IOPin::Mode mode)
  {
    m_sda.mode(mode);
    if (mode == IOPin::INPUT_MODE) m_sda.set();
  }

  /**
   * Set read/write buffer (WRITE(0), READ(1)).
   * @param[in] ix buffer index.
   */
  void buf(uint8_t ix)
  {
    if (ix > VEC_MAX) return;
    m_next = (uint8_t*) m_vec[ix].buf;
    m_last = m_next + m_vec[ix].size;
    m_count = 0;
  }

  /**
   * Return number of byte in buffer.
   * @return bytes.
   */
  uint8_t available()
  {
    return (m_last - m_next);
  }

  /**
   * Write data to buffer. Return true if not full otherwise
   * false.
   * @param[in] data to write.
   * @return bool
   */
  bool put(uint8_t data)
  {
    *m_next++ = data;
    m_count += 1;
    return (m_next < m_last);
  }

  /**
   * Read data into write buffer. Return true if successful
   * otherwise if empty false.
   * @param[out] data read.
   * @return bool
   */
  bool get(uint8_t& data)
  {
    if (m_next == m_last) return (false);
    data = *m_next++;
    m_count += 1;
    return (true);
  }

  /**
   * Generate a TWI start condition. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  bool start();

  /**
   * Transfer data to the TWI bus. Data in USI data register.
   * @param[in] data to transfer.
   * @param[in] bits to transfer (default CHARBITS).
   * @return data
   */
  uint8_t transfer(uint8_t data, uint8_t bits = CHARBITS);

  /**
   * Generate a TWI stop condition. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  bool stop();

  /**
   * Initiate a request to the device. The address field is the bus
   * address with operation (read/write bit). Return number of bytes
   * transfered or negative error code(-1).
   * @param[in] op slave operation request.
   * @return number of bytes or negative error code.
   */
  int request(uint8_t op);

  /** Allow access. */
  friend void ::USI_START_vect(void);
  friend void ::USI_OVF_vect(void);
};
#endif
#endif
