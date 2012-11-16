/**
 * @file Cosa/TWI.cpp
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

#include "Cosa/TWI.h"

static TWI* _twi = 0;

bool 
TWI::begin()
{
  if (_twi != 0) return (0);
  _twi = this;
  bit_set(DDRB, SDA_PIN);
  bit_set(DDRB, SCL_PIN);	 
  TWCR = IDLE_CMD;
  return (1);
}

bool 
TWI::end()
{
  if (_twi == 0) return (0);
  _twi = 0;
  bit_clear(DDRB, SDA_PIN);
  bit_clear(DDRB, SCL_PIN);	 
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
  _status = (Status) TWSR;
  switch (_status) {
  case START:
  case REP_START:
    TWDR = _addr;
    TWCR = DATA_CMD;
    break;
  case MT_SLA_ACK:
  case MT_DATA_ACK:
    if (_next < _size) {
      TWDR = _buf[_next++];
      TWCR = DATA_CMD;
    } 
    else {
      TWCR = STOP_CMD;
      if (_callback != 0) _callback(this);
    }
    break;
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
    if (_callback != 0) _callback(this);
    break;      
  case ARB_LOST:
    TWCR = START_CMD;
    break;
  case MT_SLA_NACK:
  case MR_SLA_NACK:
  case MT_DATA_NACK:
  case BUS_ERROR:
  default:     
    _status = (Status) TWSR;
    TWCR = IDLE_CMD; 
  }
}

ISR(TWI_vect) 
{
  if (_twi != 0) _twi->on_bus_event();
}
