/**
 * @file Cosa/USI_TWI.cpp
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

#include "Cosa/USI/TWI.hh"
#if defined(__ARDUINO_TINY__)

TWI twi  __attribute__ ((weak));

void 
TWI::Slave::set_write_buf(void* buf, size_t size)
{
  twi.m_vec[WRITE_IX].buf = buf;
  twi.m_vec[WRITE_IX].size = size;
}

void 
TWI::Slave::set_read_buf(void* buf, size_t size)
{
  twi.m_vec[READ_IX].buf = buf;
  twi.m_vec[READ_IX].size = size;
}

bool 
TWI::Slave::begin()
{
  twi.set_slave(this);
  twi.set_state(TWI::IDLE);
  synchronized {
    USICR = TWI::CR_START_MODE;
    USISR = TWI::SR_CLEAR_ALL;
  }
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
TWI::Slave::on_event(uint8_t type, uint16_t value)
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
#endif
