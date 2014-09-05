/**
 * @file Cosa/TWI.hh
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

#ifndef COSA_TWI_HH
#define COSA_TWI_HH

#include "Cosa/Types.h"
#if defined(BOARD_ATTINY)
#include "Cosa/USI/TWI.hh"
#else
#include "Cosa/Event.hh"

/**
 * Two wire library. Support for the I2C/TWI bus Master and Slave
 * device drivers. Single-ton, twi, holds bus interaction state.
 * Supporting classes TWI::Driver for device drivers, TWI::Slave
 * for slave devices. 
 *
 * @section Circuit
 * TWI slave circuit with internal pullup resistors (4K7). Note that
 * Tiny uses USI but the software interface is the same.
 * @code
 *                         TWI Slave
 *                       +------------+
 * (A4/SDA)------------1-|SDA         |
 * (A5/SCL)------------2-|SCL         |
 * (EXTn)--------------3-|IRQ(opt)    |
 * (VCC)---------------4-|VCC         |
 * (GND)---------------5-|GND         |
 *                       +------------+
 * @endcode
 */
class TWI {
public:
  /** Default Two-Wire Interface clock: 100 KHz. */
  static const uint32_t DEFAULT_FREQ = 100000L;

  /** Max Two-Wire Interface clock: 444.444 KHz. */
  static const uint32_t MAX_FREQ = (F_CPU / (16 + 2*10));

  /**
   * Device drivers are friends and may have callback/event handler
   * for completion events. 
   */
  class Driver : public Event::Handler {
  public:
    /**
     * Construct TWI driver with given bus address.
     * @param[in] addr bus address (7-bit LSB).
     */
   Driver(uint8_t addr) : Event::Handler(), m_addr(addr << 1) {}

  protected:
    /** Device bus address. */
    int8_t m_addr;

    /** Allow access. */
    friend class TWI;
    friend void TWI_vect(void);
  };

  /**
   * Slave device abstraction. Should set up buffers for request and
   * response and handle incoming requests. See also USI/TWI.hh for
   * definition for ATtiny devices.
   */
  class Slave : public Driver {
  public:
    /**
     * Construct slave with given address.
     * @param[in] addr slave address.
     */
    Slave(uint8_t addr) : Driver(addr) {}

    /**
     * Set read (result) buffer. Must be called before starting TWI.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    void set_read_buf(void* buf, size_t size);

    /**
     * Set write (argument) buffer. Must be called before starting TWI.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    void set_write_buf(void* buf, size_t size);
    
    /**
     * Start TWI bus logic for the slave device. Returns true(1) if
     * successful otherwise false(0). 
     * @return true(1) if successful otherwise false(0).
     */
    bool begin();

    /**
     * @override TWI::Slave
     * Service request callback when a write has been completed, i.e.,
     * an argument block as been written. Must be defined by sub-class.
     * Must handle write-read and write-write sequences. The device will 
     * become ready after the completion of the function. 
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    virtual void on_request(void* buf, size_t size) = 0;

  protected:
    /**
     * Internal index in io-vector for read/write buffers.
     */
    static const uint8_t WRITE_IX = 0;
    static const uint8_t READ_IX = 1;

    /**
     * @override Event::Handler
     * Filter Event::WRITE_COMPLETED_TYPE(size) and calls on_request()
     * with given write block as argument. The device is marked as ready
     * when the request has been completed and a possible result block
     * is available.
     * @param[in] type the event type.
     * @param[in] value the event value.
     */
    virtual void on_event(uint8_t type, uint16_t value);

    /** Allow access. */
    friend class TWI;
    friend void TWI_vect(void);
  };
  
  /** 
   * Construct two-wire instance. This is actually a single-ton on
   * current supported hardware, i.e. there can only be one unit.
   */
  TWI() :
    m_target(NULL),
    m_state(IDLE_STATE),
    m_status(NO_INFO),
    m_ix(0),
    m_next(NULL),
    m_last(NULL),
    m_count(0),
    m_dev(NULL),
    m_freq(((F_CPU / DEFAULT_FREQ) - 16) / 2)
  {
    for (uint8_t ix = 0; ix < VEC_MAX; ix++) {
      m_vec[ix].buf = 0;
      m_vec[ix].size = 0;
    }
  }

  /**
   * Start TWI logic for a device transaction block. Use given event
   * handler for completion events. Returns true(1) if successful
   * otherwise false(0).
   * @param[in] dev device.
   * @param[in] target receiver of events on requests (default NULL).
   * @return true(1) if successful otherwise false(0).
   */
  bool begin(TWI::Driver* dev, Event::Handler* target = NULL);

  /**
   * Stop usage of the TWI bus logic. 
   * @return true(1) if successful otherwise false(0).
   */
  bool end();

  /**
   * Issue a write data request to the current driver. Return
   * true(1) if successful otherwise(0). 
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return bool
   */
  bool write_request(void* buf, size_t size);

  /**
   * Issue a write data request to the current driver with given
   * byte header/command. Return true(1) if successful otherwise(0).
   * @param[in] header to write before buffer.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return bool
   */
  bool write_request(uint8_t header, void* buf, size_t size);

  /**
   * Issue a write data request to the current driver with given
   * header/command. Return true(1) if successful otherwise(0).
   * @param[in] header to write before buffer.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return bool
   */
  bool write_request(uint16_t header, void* buf, size_t size);

  /**
   * Issue a read data request to the current driver. Return true(1)
   * if successful otherwise(0). 
   * @param[in] buf data to read.
   * @param[in] size number of bytes to read.
   * @return number of bytes
   */
  bool read_request(void* buf, size_t size);

  /**
   * Write data to the current driver. Returns number of bytes
   * written or negative error code.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int write(void* buf, size_t size)
    __attribute__((always_inline))
  {
    if (!write_request(buf, size)) return (-1);
    return (await_completed());
  }

  /**
   * Write data to the current driver with given byte header. Returns
   * number of bytes written or negative error code. 
   * @param[in] header to write before buffer.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int write(uint8_t header, void* buf = 0, size_t size = 0)
    __attribute__((always_inline))
  {
    if (!write_request(header, buf, size)) return (-1);
    return (await_completed());
  }

  /**
   * Write data to the current driver with given header. Returns
   * number of bytes written or negative error code.
   * @param[in] header to write before buffer.
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int write(uint16_t header, void* buf = 0, size_t size = 0)
    __attribute__((always_inline))
  {
    if (!write_request(header, buf, size)) return (-1);
    return (await_completed());
  }

  /**
   * Read data to the current driver. Returns number of bytes read or
   * negative error code. 
   * @param[in] buf data to write.
   * @param[in] size number of bytes to read.
   * @return number of bytes
   */
  int read(void* buf, size_t size)
    __attribute__((always_inline))
  {
    if (!read_request(buf, size)) return (-1);
    return (await_completed());
  }

  /**
   * Await issued request to complete. Returns number of bytes 
   * or negative error code.
   */
  int await_completed();

  /**
   * Set bus frequency for device access. Does not adjust for
   * cpu frequency scaling. Compile-time cpu frequency used.
   * Should be called before starting the device driver; begin().
   * @param[in] hz bus frequency.
   */
  void set_freq(uint32_t hz)
    __attribute__((always_inline))
  {
    m_freq = (hz < MAX_FREQ) ? (((F_CPU / hz) - 16) / 2) : 10;
  }

private:
  /**
   * Two Wire state and status codes.
   */
  enum State {
    ERROR_STATE,		//!< Error state.
    IDLE_STATE,			//!< Idle state.
    MT_STATE,			//!< Master transmit state.
    MR_STATE,			//!< Master receive state.
    ST_STATE,			//!< Slave transmit state.
    SR_STATE			//!< Slave receive state.
  } __attribute__((packed));

  /**
   * Macro to generate more compact status number sequence.
   * And improve compiler code generation.
   */
# define TWI_STATUS(x) ((x) >> 3)

  /**
   * Two Wire AVR hardware status codes as presented to the interrupt
   * service routine.
   */
  enum {
    /** General Status Codes. */
    START = TWI_STATUS(0x08),
    REP_START = TWI_STATUS(0x10),
    ARB_LOST = TWI_STATUS(0x38),

    /** Master Tranmitter Mode. */
    MT_SLA_ACK = TWI_STATUS(0x18),
    MT_SLA_NACK = TWI_STATUS(0x20),
    MT_DATA_ACK = TWI_STATUS(0x28),
    MT_DATA_NACK = TWI_STATUS(0x30),

    /** Master Receiver Mode. */
    MR_SLA_ACK = TWI_STATUS(0x40),
    MR_SLA_NACK = TWI_STATUS(0x48),
    MR_DATA_ACK = TWI_STATUS(0x50),
    MR_DATA_NACK = TWI_STATUS(0x58),

    /** Slave Transmitter Mode. */
    ST_SLA_ACK = TWI_STATUS(0xA8),
    ST_ARB_LOST_SLA_ACK = TWI_STATUS(0xB0),
    ST_DATA_ACK = TWI_STATUS(0xB8),
    ST_DATA_NACK = TWI_STATUS(0xC0),
    ST_LAST_DATA = TWI_STATUS(0xC8),

    /** Slave Receiver Mode. */
    SR_SLA_ACK = TWI_STATUS(0x60),
    SR_ARB_LOST_SLA_ACK = TWI_STATUS(0x68),
    SR_GCALL_ACK = TWI_STATUS(0x70),
    SR_ARB_LOST_GCALL_ACK = TWI_STATUS(0x78),
    SR_DATA_ACK = TWI_STATUS(0x80),
    SR_DATA_NACK = TWI_STATUS(0x88),
    SR_GCALL_DATA_ACK = TWI_STATUS(0x90),
    SR_GCALL_DATA_NACK = TWI_STATUS(0x98),
    SR_STOP = TWI_STATUS(0xA0),

    /** Misc. */
    NO_INFO = TWI_STATUS(0xF8),
    BUS_ERROR = TWI_STATUS(0x00)
  } __attribute__((packed));

  /**
   * Address mask and read/write bit.
   */
  enum {
    WRITE_OP = 0x00,		//!< Write operation.
    READ_OP = 0x01,		//!< Read operation.
    ADDR_MASK = 0xfe		//!< Address mask.
  } __attribute__((packed));

  /** Default argument for start. */
  static const uint8_t NEXT_IX = 255;

  /**
   * Commands for TWI hardware.
   */
  enum Command {
    NULL_CMD = 0,
    IDLE_CMD = _BV(TWEA) | _BV(TWEN) | _BV(TWIE),
    START_CMD = _BV(TWINT) | _BV(TWEA) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE),
    DATA_CMD = _BV(TWINT) | _BV(TWEN) | _BV(TWIE),
    ACK_CMD =  _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE),
    NACK_CMD = _BV(TWINT) | _BV(TWEN) | _BV(TWIE),
    STOP_CMD =  _BV(TWINT) | _BV(TWEA) | _BV(TWSTO) | _BV(TWEN) | _BV(TWIE)
  } __attribute__((packed));

  /**
   * Device state, data buffers, current device and target.
   */
  static const uint8_t HEADER_MAX = 4;
  static const uint8_t VEC_MAX = 4;
  uint8_t m_header[HEADER_MAX];
  iovec_t m_vec[VEC_MAX];
  Event::Handler* m_target;
  volatile State m_state;
  volatile uint8_t m_status;
  volatile uint8_t m_ix;
  volatile uint8_t* m_next;
  volatile uint8_t* m_last;
  volatile int m_count;
  uint8_t m_addr;
  Driver* m_dev;
  uint8_t m_freq;
  
  /**
   * Start block transfer. Setup internal buffer pointers.
   * Part of the TWI ISR state machine.
   * @param[in] state next state.
   * @param[in] ix io vector index.
   */
  void isr_start(State state, uint8_t ix);

  /**
   * Write next byte to hardware device and issue command. Returns
   * true if byte was written else false. Part of the TWI ISR state machine.
   * @param[in] cmd command to write with byte.
   * @return bool
   */
  bool isr_write(Command cmd);

  /**
   * Read next byte from hardware device and store into buffer.
   * Part of the TWI ISR state machine.
   * Return true if byte was stored else false.
   * @param[in] cmd command to write (default is no command, NULL_CMD).
   * @return bool
   */
  bool isr_read(Command cmd = NULL_CMD);

  /**
   * Stop block transfer and step to given state. If error sets count to
   * negative error code(-1).  Part of the TWI ISR state machine.
   * @param[in] state to step to.
   * @param[in] type of event to push (default no event).
   */
  void isr_stop(State state, uint8_t type = Event::NULL_TYPE);

  /**
   * Initiate a request to the device. Return true(1) if successful
   * otherwise false(0).  
   * @param[in] op slave operation.
   * @return bool
   */
  bool request(uint8_t op);

  /** Interrupt Sevice Routine. */
  friend void TWI_vect(void);
};
#endif

/**
 * Two-wire hardware interface module. Singleton on Arduino hardware.
*/
extern TWI twi;
#endif
