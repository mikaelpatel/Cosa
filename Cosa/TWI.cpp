/**
 * @file Cosa/TWI.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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

#include "Cosa/TWI.h"

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

static TWI* _twi = 0;

bool 
TWI::begin(uint8_t addr, Callback fn)
{
  _twi = this;
  _callback = fn;
  _addr = (addr << ADDR_POS);

  // Check for slave mode
  if (fn != 0) {
    TWAR = _addr;
  }

  // Enable internal pullup
  synchronized {
    bit_set(PORTC, SDA);
    bit_set(PORTC, SCL);
  }
  
  // Set clock prescale and bit rate
  bit_clear(TWSR, TWPS0);
  bit_clear(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
  TWCR = IDLE_CMD;
  return (1);
}

bool 
TWI::end()
{
  _twi = 0;
  TWCR = 0;
  return (1);
}

bool
TWI::request(uint8_t addr, void* buf, uint8_t size, Callback fn)
{
  if (is_busy()) return (0);
  _addr = addr;
  _status = NO_INFO;
  _callback = fn;
  _buf = (uint8_t*) buf;
  _size = size;
  _next = 0;
  TWCR = START_CMD;
  return (1);
}

int16_t
TWI::write(uint8_t addr, void* buf, uint8_t size)
{
  if (!write_request(addr, buf, size)) return (-1);
  while (is_busy()); 
  return (_next);
}

int16_t
TWI::read(uint8_t addr, void* buf, uint8_t size)
{
  if (!read_request(addr, buf, size)) return (-1);
  while (is_busy()); 
  return (_next);
}

void
TWI::on_bus_event()
{
  _status = TWI_STATUS(TWSR);
  switch (_status) {
  case START:
  case REP_START:
    TWDR = _addr;
    TWCR = DATA_CMD;
    break;
  case ARB_LOST:
    TWCR = START_CMD;
    _next = 0;
    break;
  /**
   * Master Transmitter Mode
   */
  case MT_SLA_ACK:
  case MT_DATA_ACK:
    if (_next < _size) {
      TWDR = _buf[_next++];
      TWCR = DATA_CMD;
    } 
    else {
      TWCR = STOP_CMD;
      if (_callback != 0)_callback(this);
    }
    break;
  case MT_SLA_NACK:
  case MT_DATA_NACK:
    TWCR = STOP_CMD;
    break;
  /**
   * Master Receiver Mode
   */
  case MR_DATA_ACK:
    _buf[_next++] = TWDR;
  case MR_SLA_ACK:
    if (_next < (_size - 1)) {
      TWCR = ACK_CMD;
    } 
    else {
      TWCR = NACK_CMD;
    }    
    break; 
  case MR_DATA_NACK:
    _buf[_next++] = TWDR;
    TWCR = STOP_CMD;
    if (_callback != 0)_callback(this);
    _next = 0;
    break;      
  case MR_SLA_NACK:
    TWCR = STOP_CMD;
    break;
  /**
   * Slave Transmitter Mode
   */
  case ST_SLA_ACK:
  case ST_ARB_LOST_SLA_ACK:
    if (_callback != 0)_callback(this);
    _next = 0;
  case ST_DATA_ACK:
    TWDR = _buf[_next++];
    if (_next < _size) {
      TWCR = ACK_CMD;
    } 
    else {
      TWCR = NACK_CMD;
    }
    break;
  case ST_DATA_NACK:
  case ST_LAST_DATA:
    TWCR = ACK_CMD;
    break;
  /**
   * Slave Receiver Mode
   */
  case SR_SLA_ACK:
  case SR_GCALL_ACK:
  case SR_ARB_LOST_SLA_ACK:
  case SR_ARB_LOST_GCALL_ACK:
    _next = 0;
    TWCR = ACK_CMD;
    break;
  case SR_DATA_ACK:
  case SR_GCALL_DATA_ACK:
    if (_next < _size){
      _buf[_next++] = TWDR;
      TWCR = ACK_CMD;
    }
    else {
      TWCR = NACK_CMD;
    }
    break;
  case SR_STOP:
    if (_next < _size) {
      _buf[_next] = 0;
    }
    TWCR = STOP_CMD;
    if (_callback != 0) _callback(this);
    TWCR = IDLE_CMD; 
    break;
  case SR_DATA_NACK:
  case SR_GCALL_DATA_NACK:
    TWCR = NACK_CMD;
    break;
  case NO_INFO:
    break;
  case BUS_ERROR:
  default:     
    TWCR = IDLE_CMD; 
  }
}

ISR(TWI_vect) 
{
  if (_twi != 0) _twi->on_bus_event();
}
