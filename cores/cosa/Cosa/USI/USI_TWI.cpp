/**
 * @file Cosa/USI_TWI.cpp
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

#include "Cosa/Board.hh"
#if defined(BOARD_ATTINY)
#include "Cosa/USI/TWI.hh"

#if defined(USE_FAST_MODE)
// TWI timing constants (us) for fast mode (100-400 kHz)
#define T2 ((((I_CPU * 1300) / 10000) + 1) / 4)
#define T4 ((((I_CPU * 600) / 10000) + 1) / 4)
#else
// TWI timing constants (us) for standard mode (100 kHz)
#define T2 ((((I_CPU * 4700) / 10000) + 1) / 4)
#define T4 ((((I_CPU * 4000) / 10000) + 1) / 4)
#endif

TWI twi  __attribute__ ((weak));

void
TWI::Slave::write_buf(void* buf, size_t size)
{
  twi.m_vec[WRITE_IX].buf = buf;
  twi.m_vec[WRITE_IX].size = size;
}

void
TWI::Slave::read_buf(void* buf, size_t size)
{
  twi.m_vec[READ_IX].buf = buf;
  twi.m_vec[READ_IX].size = size;
}

void
TWI::Slave::begin()
{
  twi.m_dev = this;
  twi.state(TWI::IDLE);
  synchronized {
    USICR = TWI::CR_START_MODE;
    USISR = TWI::SR_CLEAR_ALL;
  }
}

ISR(USI_START_vect)
{
  if (twi.state() != TWI::IDLE) return;
  twi.mode(IOPin::INPUT_MODE);
  USICR = TWI::CR_TRANSFER_MODE;
  USISR = TWI::SR_CLEAR_ALL;
  twi.state(TWI::START_CHECK);
}

ISR(USI_OVF_vect)
{
  switch (twi.state()) {
    /**
     * Transaction Start Mode
     */
  case TWI::START_CHECK:
    {
      uint8_t addr = USIDR;
      if ((addr & TWI::ADDR_MASK) != twi.m_dev->m_addr) goto restart;
      if (addr & TWI::READ_OP) {
	twi.state(TWI::READ_REQUEST);
	twi.buf(TWI::READ_IX);
      }
      else {
	twi.state(TWI::WRITE_REQUEST);
	twi.buf(TWI::WRITE_IX);
      }
      USIDR = 0;
      twi.mode(IOPin::OUTPUT_MODE);
      USISR = TWI::SR_CLEAR_ACK;
    }
    break;

    /**
     * Slave Transmitter Mode
     */
  case TWI::ACK_CHECK:
    if (USIDR) goto restart;

  case TWI::READ_REQUEST:
    {
      uint8_t data;
      if (!twi.get(data)) goto restart;
      USIDR = data;
      twi.mode(IOPin::OUTPUT_MODE);
      USISR = TWI::SR_CLEAR_DATA;
      twi.state(TWI::READ_COMPLETED);
    }
    break;

  case TWI::READ_COMPLETED:
    twi.mode(IOPin::INPUT_MODE);
    USIDR = 0;
    USISR = TWI::SR_CLEAR_ACK;
    twi.state(TWI::ACK_CHECK);
    break;

    /**
     * Slave Receiver Mode
     */
  case TWI::WRITE_REQUEST:
    twi.mode(IOPin::INPUT_MODE);
    USISR = TWI::SR_CLEAR_DATA;
    twi.state(TWI::WRITE_COMPLETED);
    DELAY(20);
    if (USISR & _BV(USIPF)) {
      USICR = TWI::CR_SERVICE_MODE;
      USISR = TWI::SR_CLEAR_ALL;
      twi.m_dev->on_completion(Event::WRITE_COMPLETED_TYPE, twi.m_count);
      twi.state(TWI::SERVICE_REQUEST);
    }
    break;

  case TWI::WRITE_COMPLETED:
    {
      uint8_t data = USIDR;
      USIDR = (twi.put(data) ? 0x00 : 0x80);
      twi.mode(IOPin::OUTPUT_MODE);
      USISR = TWI::SR_CLEAR_ACK;
      twi.state(TWI::WRITE_REQUEST);
    }
    break;

  restart:
  default:
    twi.mode(IOPin::INPUT_MODE);
    USICR = TWI::CR_START_MODE;
    USISR = TWI::SR_CLEAR_DATA;
    twi.state(TWI::IDLE);
  }
}

void
TWI::Slave::on_event(uint8_t type, uint16_t value)
{
  if (type != Event::WRITE_COMPLETED_TYPE) return;
  void* buf = twi.m_vec[WRITE_IX].buf;
  size_t size = value;
  on_request(buf, size);
  twi.state(IDLE);
  synchronized {
    USICR = TWI::CR_START_MODE;
    USISR = TWI::SR_CLEAR_DATA;
  }
}

TWI::TWI() :
  m_sda((Board::DigitalPin) Board::SDA, IOPin::INPUT_MODE, true),
  m_scl((Board::DigitalPin) Board::SCL, IOPin::OUTPUT_MODE, true),
  m_state(IDLE),
  m_next(0),
  m_last(0),
  m_count(0),
  m_dev(0),
  m_busy(false)
{
  for (uint8_t ix = 0; ix < VEC_MAX; ix++) {
    m_vec[ix].buf = 0;
    m_vec[ix].size = 0;
  }
}

bool
TWI::start()
{
  // Release SCL to ensure that (repeated) start can be performed
  m_scl.set();
  while (!m_scl.is_set())
    ;
  DELAY(T4);

  // Generate the start condition
  m_sda.clear();
  DELAY(T4);
  m_scl.clear();
  m_sda.set();

  // Verify start condition
  return ((USISR & _BV(USISIF)) != 0);
}

uint8_t
TWI::transfer(uint8_t data, uint8_t bits)
{
  // Setup data and number of bits to be clocked
  uint8_t SR = SR_CLEAR_ALL;
  if (bits == 1) SR |= (0x0E << USICNT0);
  USIDR = data;
  USISR = SR;

  // Clock bits onto the bus using software strobe
  do {
    DELAY(T2);
    USICR = CR_DATA_MODE;
    while (!m_scl.is_set())
      ;
    DELAY(T4);
    USICR = CR_DATA_MODE;
  } while (!(USISR & _BV(USIOIF)));
  DELAY(T2);

  // Read received data and release bus
  uint8_t res = USIDR;
  USIDR = 0xff;
  mode(IOPin::OUTPUT_MODE);
  return (res);
}

bool
TWI::stop()
{
  // Release SCL and signal stop. Assume SCL/SDA are both output
  m_sda.clear();
  m_scl.set();
  while (!m_scl.is_set())
    ;
  DELAY(T4);
  m_sda.set();
  DELAY(T2);

  // Verify stop condition
  return ((USISR & _BV(USIPF)) != 0);
}

int
TWI::request(uint8_t op)
{
  bool is_read = (op & READ_OP);
  uint8_t* next = (uint8_t*) m_vec[0].buf;
  uint8_t* last = next + m_vec[0].size;
  int count = 0;

  // Send start condition and write address
  if (!start()) return (EFAULT);
  m_scl.clear();
  transfer(m_dev->m_addr | is_read);
  mode(IOPin::INPUT_MODE);
  if (transfer(0, 1)) goto nack;

  // Read or write data
  for (uint8_t ix = 1; next != 0; ix++) {
    while (next != last) {
      count += 1;
      if (is_read) {
	mode(IOPin::INPUT_MODE);
	*next++ = transfer(0);
	transfer((next != last) ? 0x00 : 0xff, 1);
      }
      else {
	m_scl.clear();
	transfer(*next++);
	mode(IOPin::INPUT_MODE);
	if (transfer(0, 1)) goto nack;
      }
    }
    next = (uint8_t*) m_vec[ix].buf;
    last = next + m_vec[ix].size;
  }

 nack:
  if (!stop()) return (EFAULT);
  return (count);
}

void
TWI::acquire(TWI::Driver* dev)
{
  // Acquire the device driver. Wait is busy. Synchronized update
  uint8_t key = lock(m_busy);

  // Set the current device driver
  m_dev = dev;

  // Power up the module
  powerup();

  // Release level data and init mode
  USIDR = 0xff;
  USICR = CR_INIT_MODE;
  USISR = SR_CLEAR_ALL;
  mode(IOPin::OUTPUT_MODE);
  unlock(key);
}

void
TWI::release()
{
  // Check if an asynchronious read/write was issued
  if (UNLIKELY((m_dev == NULL) || (m_dev->is_async()))) return;

  // Put into idle state
  synchronized {
    m_dev = NULL;
    m_busy = false;
    USICR = 0;
  }

  // Power down the module
  powerdown();
}

int
TWI::write(void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(WRITE_OP));
}

int
TWI::write(uint8_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(WRITE_OP));
}

int
TWI::write(uint16_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = (header >> 8);
  m_header[1] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(WRITE_OP));
}

int
TWI::read(void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(READ_OP));
}
#endif
