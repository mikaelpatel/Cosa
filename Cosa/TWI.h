/**
 * @file Cosa/TWI.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * @section Description
 * Two wire library. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TWI_H__
#define __COSA_TWI_H__

#include "Cosa/Types.h"
#include "Bits.h"
#include "Event.h"

#define TWI_STATUS(x) ((x) >> 3)

class TWI {

public:
  /**
   * Callback function prototype for request completions.
   * @param[in] twi callback environment.
   */
  typedef void (*Callback)(TWI* twi);

protected:
  /**
   * Two wire status codes
   */
  enum {
    /** General Status Codes */
    START = TWI_STATUS(0x08),
    REP_START = TWI_STATUS(0x10),
    ARB_LOST = TWI_STATUS(0x38),
    /** Master Tranmitter Mode*/
    MT_SLA_ACK = TWI_STATUS(0x18),
    MT_SLA_NACK = TWI_STATUS(0x20),
    MT_DATA_ACK = TWI_STATUS(0x28),
    MT_DATA_NACK = TWI_STATUS(0x30),
    /** Master Receiver Mode */
    MR_SLA_ACK = TWI_STATUS(0x40),
    MR_SLA_NACK = TWI_STATUS(0x48),
    MR_DATA_ACK = TWI_STATUS(0x50),
    MR_DATA_NACK = TWI_STATUS(0x58),
    /** Slave Transmitter Mode */
    ST_SLA_ACK = TWI_STATUS(0xA8),
    ST_ARB_LOST_SLA_ACK = TWI_STATUS(0xB0),
    ST_DATA_ACK = TWI_STATUS(0xB8),
    ST_DATA_NACK = TWI_STATUS(0xC0),
    ST_LAST_DATA = TWI_STATUS(0xC8),
    /** Slave Receiver Mode */
    SR_SLA_ACK = TWI_STATUS(0x60),
    SR_ARB_LOST_SLA_ACK = TWI_STATUS(0x68),
    SR_GCALL_ACK = TWI_STATUS(0x70),
    SR_ARB_LOST_GCALL_ACK = TWI_STATUS(0x78),
    SR_DATA_ACK = TWI_STATUS(0x80),
    SR_DATA_NACK = TWI_STATUS(0x88),
    SR_GCALL_DATA_ACK = TWI_STATUS(0x90),
    SR_GCALL_DATA_NACK = TWI_STATUS(0x98),
    SR_STOP = TWI_STATUS(0xA0),
    /** Misc */
    NO_INFO = TWI_STATUS(0xF8),
    BUS_ERROR = TWI_STATUS(0x00)
  };

  /**
   * Addressing and read/write bit 
   */
  enum {
    WRITE_OP = 0x00,
    READ_OP = 0x01
  };
  enum {
    ADDR_POS = 1,
    ADDR_MASK = 0xfe
  };

  /**
   * Commands for TWI hardware
   */
  enum {
    IDLE_CMD =  _BV(TWINT) |_BV(TWEA) | _BV(TWEN) | _BV(TWIE),
    START_CMD = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA) | _BV(TWEN) | _BV(TWIE),
    DATA_CMD = _BV(TWINT) | _BV(TWEN) | _BV(TWIE),
    ACK_CMD = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE),
    NACK_CMD = _BV(TWINT) | _BV(TWEN) | _BV(TWIE),
    STOP_CMD =  _BV(TWINT) | _BV(TWEA) | _BV(TWSTO) | _BV(TWEN) | _BV(TWIE)
  };

  /**
   * Pins used for TWI interface (in port C).
   */
  enum {
    SDA = 4,
    SCL = 5
  };

  Callback _callback;
  uint8_t _status;
  uint8_t _addr;
  uint8_t* _buf;
  uint8_t _size;
  uint8_t _next;

  /**
   * Check if the TWI bus logic is busy
   * @return bool
   */
  bool is_busy() 
  {
    return (bit_is_set(TWCR, TWIE) != 0); 
  }

  /**
   * Get the current status. Wait if the TWI bus logic is busy.
   * @return status
   */
  uint8_t get_status()
  {
    while (is_busy());
    return (_status = TWI_STATUS(TWSR));
  }
  
  /**
   * Initiate a request to the slave. The address field is
   * the TWI address and operation (read/write bit). 
   * Return true(1) if successful otherwise false(0).
   * @param[in] addr slave address and operation.
   * @param[in] buf data buffer to send or receive into.
   * @param[in] size number of bytes in buffer.
   * @param[in] fn callback on completion.
   * @return bool
   */
  bool request(uint8_t addr, void* buf, uint8_t size, Callback fn);

public:
  TWI() :
    _status(NO_INFO),
    _callback(0),
    _addr(0),
    _buf(0),
    _size(0),
    _next(0)
  {
  }

  /**
   * Start TWI bus logic. Default mode is master.
   * @param[in] addr slave address (0..127).
   * @param[in] fn slave callback.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint8_t addr = 0, Callback fn = 0);

  /**
   * Disconnect usage of the TWI bus logic.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Write data to the given slave unit. Returns number of bytes
   * written or negative error code.
   * @param[in] addr slave address (0..127).
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes
   */
  int16_t write(uint8_t addr, void* buf, uint8_t size);

  /**
   * Read data to the given slave unit. Returns number of bytes read
   * or negative error code.
   * @param[in] addr slave address (0..127).
   * @param[in] buf data to write.
   * @param[in] size number of bytes to read.
   * @return number of bytes
   */
  int16_t read(uint8_t addr, void* buf, uint8_t size);

  /**
   * In master mode requests write data to the given slave unit.
   * Returns true(1) if successful request otherwise false(0)
   * @param[in] addr slave address (0..127).
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @param[in] fn callback on completion.
   * @return true(1) if successful otherwise false(0)
   */
  bool write_request(uint8_t addr, void* buf, uint8_t size, Callback fn = 0)
  {
    return (request(((addr << ADDR_POS) | WRITE_OP), buf, size, fn));
  }

  /**
   * In master mode requests read data to the given slave unit.
   * Returns true(1) if successful request otherwise false(0)
   * @param[in] addr slave address (0..127).
   * @param[in] buf data to write.
   * @param[in] size number of bytes to write.
   * @param[in] fn callback on completion.
   * @return true(1) if successful request otherwise false(0)
   */
  bool read_request(uint8_t addr, void* buf, uint8_t size, Callback fn = 0)
  {
    return (request(((addr << ADDR_POS) | READ_OP), buf, size, fn));
  }

  /**
   * Return request buffer.
   * @return buffer pointer.
   */
  void* get_buf() 
  { 
    return (_buf); 
  }

  /**
   * Return number of bytes in request buffer.
   * @return size.
   */
  uint8_t get_size() 
  { 
    return (_next); 
  }

  /**
   * Set buffer and size of buffer (for slave callback).
   * @param[in] buf data to write.
   * @param[in] size max number of bytes to write.
   */
  void set(void* buf, uint8_t size)
  {
    _buf = (uint8_t*) buf;
    _size = size;
  }

  /**
   * Callback function to push event on write completion.
   */
  static void push_write_event(TWI* twi)
  { 
    Event::push(Event::TWI_WRITE_DATA_TYPE, twi, 0);
  }

  /**
   * Callback function to push event on read completion.
   */
  static void push_read_event(TWI* twi)
  { 
    Event::push(Event::TWI_READ_DATA_TYPE, twi, 0);
  }

  /**
   * TWI state machine. Run by interrupt handler.
   */
  void on_bus_event();
};

#endif
