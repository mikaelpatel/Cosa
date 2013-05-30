/**
 * @file Cosa/TWI.cpp
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

#include "Cosa/Board.hh"
#include "Cosa/TWI.hh"

TWI twi  __attribute__ ((weak));

#if defined(__ARDUINO_TINY__)

bool 
TWI::begin(Device* target, uint8_t addr)
{
  if (target == 0 || addr == 0) return (false);
  m_target = target;
  m_addr = addr;
  m_state = IDLE;
  synchronized {
    USICR = CR_START_MODE;
    USISR = SR_CLEAR_ALL;
  }
  return (true);
}

bool 
TWI::end()
{
  m_target = 0;
  m_addr = 0;
  m_state = IDLE;
  USICR = 0;
  USISR = 0;
  return (true);
}

ISR(USI_START_vect) 
{
  if (twi.get_state() != TWI::IDLE) return;
  twi.set_mode(IOPin::INPUT_MODE);
  USICR = TWI::CR_TRANSFER_MODE;
  USISR = TWI::SR_CLEAR_ALL;
  twi.set_state(TWI::START_CHECK);
}

ISR(USI_OVF_vect) 
{
  switch (twi.get_state()) {
    /**
     * Transaction Start Mode
     */
  case TWI::START_CHECK:
    {
      uint8_t addr = USIDR;
      if ((addr & TWI::ADDR_MASK) != twi.m_addr) goto restart;
      if (addr & TWI::READ_OP) {
	twi.set_state(TWI::READ_REQUEST);
	twi.set_buf(TWI::READ_IX);
      }
      else {
	twi.set_state(TWI::WRITE_REQUEST);
	twi.set_buf(TWI::WRITE_IX);
      }
      USIDR = 0;
      twi.set_mode(IOPin::OUTPUT_MODE);
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
      twi.set_mode(IOPin::OUTPUT_MODE);
      USISR = TWI::SR_CLEAR_DATA;
      twi.set_state(TWI::READ_COMPLETED);
    }
    break;

  case TWI::READ_COMPLETED:
    twi.set_mode(IOPin::INPUT_MODE);
    USIDR = 0;
    USISR = TWI::SR_CLEAR_ACK;
    twi.set_state(TWI::ACK_CHECK);
    break;

    /**
     * Slave Receiver Mode
     */
  case TWI::WRITE_REQUEST:
    twi.set_mode(IOPin::INPUT_MODE);
    USISR = TWI::SR_CLEAR_DATA;
    twi.set_state(TWI::WRITE_COMPLETED);
    DELAY(20);
    if (USISR & _BV(USIPF)) {
      USICR = TWI::CR_SERVICE_MODE;
      USISR = TWI::SR_CLEAR_ALL;
      Event::push(Event::WRITE_COMPLETED_TYPE, twi.m_target, twi.m_count);
      twi.set_state(TWI::SERVICE_REQUEST);
    }
    break;
    
  case TWI::WRITE_COMPLETED:
    {
      uint8_t data = USIDR;
      USIDR = (twi.put(data) ? 0x00 : 0x80);
      twi.set_mode(IOPin::OUTPUT_MODE);
      USISR = TWI::SR_CLEAR_ACK;
      twi.set_state(TWI::WRITE_REQUEST);
    }
    break;

  restart:
  default:
    twi.set_mode(IOPin::INPUT_MODE);
    USICR = TWI::CR_START_MODE;
    USISR = TWI::SR_CLEAR_DATA;
    twi.set_state(TWI::IDLE);
  }
}

void 
TWI::Device::on_event(uint8_t type, uint16_t value)
{
  if (type != Event::WRITE_COMPLETED_TYPE) return;
  void* buf = twi.m_vec[WRITE_IX].buf;
  size_t size = value;
  on_request(buf, size);
  twi.set_state(IDLE);
  synchronized {
    USICR = TWI::CR_START_MODE;
    USISR = TWI::SR_CLEAR_DATA;
  }
}

#else

#include "Cosa/Bits.h"
#include "Cosa/Power.hh"

/**
 * Default Two-Wire Interface clock: 100 KHz
 */
#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

bool 
TWI::begin(Event::Handler* target, uint8_t addr)
{
  m_target = target;
  m_addr = addr;

  // Check for slave mode and set device address
  if (addr != 0) {
    if (target == 0) return (false);
    TWAR = m_addr;
  } 
  else {
    // Enable internal pullup
    synchronized {
      bit_set(PORTC, Board::SDA);
      bit_set(PORTC, Board::SCL);
    }
  }
  
  // Set clock prescale and bit rate
  bit_clear(TWSR, TWPS0);
  bit_clear(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
  TWCR = IDLE_CMD;
  return (true);
}

bool 
TWI::end()
{
  m_target = 0;
  TWCR = 0;
  TWAR = 0;
  return (true);
}

bool
TWI::request(uint8_t addr)
{
  m_addr = addr;
  m_state = (addr & READ_OP) ? MR_STATE : MT_STATE;
  m_status = NO_INFO;
  m_next = (uint8_t*) m_vec[0].buf;
  m_last = m_next + m_vec[0].size;
  m_ix = 0;
  m_count = 0;
  TWCR = START_CMD;
  return (true);
}

bool
TWI::write_request(uint8_t addr, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | WRITE_OP));
}

bool
TWI::write_request(uint8_t addr, uint8_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | WRITE_OP));
}

bool
TWI::write_request(uint8_t addr, uint16_t header, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  m_header[0] = (header >> 8);
  m_header[1] = header;
  iovec_arg(vp, m_header, sizeof(header));
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | WRITE_OP));
}

bool
TWI::read_request(uint8_t addr, void* buf, size_t size)
{
  iovec_t* vp = m_vec;
  iovec_arg(vp, buf, size);
  iovec_end(vp);
  return (request(addr | READ_OP));
}

void 
TWI::start(State state, uint8_t ix)
{
  if (ix == NEXT_IX) {
    m_ix += 1;
    ix = m_ix;
  }
  else m_count = 0;
  m_next = (uint8_t*) m_vec[ix].buf;
  m_last = m_next + m_vec[ix].size;
  m_state = state;
}

void 
TWI::stop(State state, uint8_t type)
{
  TWCR = TWI::STOP_CMD;
  loop_until_bit_is_clear(TWCR, TWSTO);
  if (state == TWI::ERROR_STATE) m_count = -1;
  m_state = state;
  if (type != Event::NULL_TYPE && m_target != 0)
    Event::push(type, m_target, m_count);
}

bool
TWI::write(uint8_t cmd)
{
  if (m_next == m_last) return (false);
  TWDR = *m_next++;
  TWCR = cmd;
  m_count += 1;
  return (true);
}

bool
TWI::read(uint8_t cmd)
{
  if (m_next == m_last) return (false);
  *m_next++ = TWDR;
  m_count += 1;
  if (cmd != 0) TWCR = cmd;
  return (true);
}

int
TWI::await_completed(uint8_t mode)
{
  while (m_state > IDLE_STATE) Power::sleep(mode);
  return (m_count);
}

ISR(TWI_vect) 
{
  twi.m_status = TWI_STATUS(TWSR);
  switch (twi.m_status) {
    /**
     * Transaction Start Mode
     */
  case TWI::START:
  case TWI::REP_START:
    // Write device address
    TWDR = twi.m_addr;
    TWCR = TWI::DATA_CMD;
    break;
  case TWI::ARB_LOST:
    // Lost arbitration
    TWCR = TWI::IDLE_CMD;
    twi.m_state = TWI::ERROR_STATE;
    twi.m_count = -1;
    break;
    
    /**
     * Master Transmitter Mode
     */
  case TWI::MT_SLA_ACK:
  case TWI::MT_DATA_ACK:
    if (twi.m_next == twi.m_last) twi.start(TWI::MT_STATE);
    if (twi.write(TWI::DATA_CMD)) break;
  case TWI::MT_DATA_NACK: 
    twi.stop(TWI::IDLE_STATE, Event::WRITE_COMPLETED_TYPE);
    break;
  case TWI::MT_SLA_NACK: 
    twi.stop(TWI::ERROR_STATE, Event::ERROR_TYPE);
    break;

    /**
     * Master Receiver Mode
     */
  case TWI::MR_DATA_ACK:
    twi.read();
  case TWI::MR_SLA_ACK:
    TWCR = (twi.m_next < (twi.m_last - 1)) ? TWI::ACK_CMD : TWI::NACK_CMD;
    break; 
  case TWI::MR_DATA_NACK:
    twi.read();
    twi.stop(TWI::IDLE_STATE, Event::READ_COMPLETED_TYPE);
    break;
  case TWI::MR_SLA_NACK: 
    twi.stop(TWI::ERROR_STATE, Event::ERROR_TYPE);
    break;

    /**
     * Slave Transmitter Mode
     */
  case TWI::ST_SLA_ACK:
  case TWI::ST_ARB_LOST_SLA_ACK:
    twi.start(TWI::ST_STATE, TWI::Device::READ_IX);
  case TWI::ST_DATA_ACK:
    if (twi.write(TWI::ACK_CMD)) break;
    TWCR = TWI::NACK_CMD;
    break;
  case TWI::ST_DATA_NACK:
  case TWI::ST_LAST_DATA:
    TWCR = TWI::ACK_CMD;
    twi.m_state = TWI::IDLE_STATE;
    break;

    /**
     * Slave Receiver Mode
     */
  case TWI::SR_SLA_ACK:
  case TWI::SR_GCALL_ACK:
  case TWI::SR_ARB_LOST_SLA_ACK:
  case TWI::SR_ARB_LOST_GCALL_ACK:
    twi.start(TWI::SR_STATE, TWI::Device::WRITE_IX);
    TWCR = TWI::ACK_CMD;
    break;
  case TWI::SR_DATA_ACK:
  case TWI::SR_GCALL_DATA_ACK:
    if (twi.read(TWI::ACK_CMD)) break;
  case TWI::SR_DATA_NACK:
  case TWI::SR_GCALL_DATA_NACK:
    TWCR = TWI::NACK_CMD;
    break;
  case TWI::SR_STOP:
    twi.stop(TWI::IDLE_STATE, Event::WRITE_COMPLETED_TYPE);
    TWAR = 0;
    break;

  case TWI::NO_INFO:
    break;

  case TWI::BUS_ERROR: 
    twi.stop(TWI::ERROR_STATE);
    break;
    
  default:     
    TWCR = TWI::IDLE_CMD; 
  }
}

void 
TWI::Device::on_event(uint8_t type, uint16_t value)
{
  if (type != Event::WRITE_COMPLETED_TYPE) return;
  void* buf = twi.m_vec[WRITE_IX].buf;
  size_t size = value;
  on_request(buf, size);
  TWAR = twi.m_addr;
}

#endif

void 
TWI::Device::set_write_buf(void* buf, size_t size)
{
  twi.m_vec[WRITE_IX].buf = buf;
  twi.m_vec[WRITE_IX].size = size;
}

void 
TWI::Device::set_read_buf(void* buf, size_t size)
{
  twi.m_vec[READ_IX].buf = buf;
  twi.m_vec[READ_IX].size = size;
}

