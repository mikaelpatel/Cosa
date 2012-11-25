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
#include "Cosa/Watchdog.h"

#include <util/delay_basic.h>

#define DELAY(us) _delay_loop_2((us) << 2)

#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

static TWI* _twi = 0;

bool 
TWI::begin(uint8_t addr, InterruptHandler fn)
{
  _twi = this;
  _handler = fn;
  _addr = addr;

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
TWI::request(uint8_t addr, InterruptHandler fn)
{
  _addr = addr;
  _state = (addr & WRITE_OP) ? MT_STATE : MR_STATE;
  _status = NO_INFO;
  _handler = fn;
  _count = 0;
  _next = 0;
  _ix = 0;
  TWCR = START_CMD;
  return (1);
}

int
TWI::write(uint8_t addr, void* buf, uint8_t size)
{
  _vec[0].buf = (uint8_t*) buf;
  _vec[0].size = size;
  _vec[1].buf = 0;
  _vec[1].size = 0;
  if (!request(addr | WRITE_OP)) return (-1);
  await_request();
  return (_count);
}

int 
TWI::write(uint8_t addr, uint8_t header, void* buf, uint8_t size)
{
  _buf[0] = header;
  _vec[0].buf = _buf;
  _vec[0].size = sizeof(header);
  _vec[1].buf = (uint8_t*) buf;
  _vec[1].size = size;
  _vec[2].buf = 0;
  _vec[2].size = 0;
  if (!request(addr | WRITE_OP)) return (-1);
  await_request();
  return (_count);
}

int 
TWI::write(uint8_t addr, uint16_t header, void* buf, uint8_t size)
{
  _buf[0] = (header >> 8);
  _buf[1] = header;
  _vec[0].buf = _buf;
  _vec[0].size = sizeof(header);
  _vec[1].buf = (uint8_t*) buf;
  _vec[1].size = size;
  _vec[2].buf = 0;
  _vec[2].size = 0;
  if (!request(addr | WRITE_OP)) return (-1);
  await_request();
  return (_count);
}

int
TWI::read(uint8_t addr, void* buf, uint8_t size)
{
  _vec[0].buf = (uint8_t*) buf;
  _vec[0].size = size;
  _vec[1].buf = 0;
  _vec[1].size = 0;
  if (!request(addr | READ_OP)) return (-1);
  await_request();
  return (_count);
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
    _ix = 0;
    _count = 0;
    _next = 0;
    break;
  /**
   * Master Transmitter Mode
   */
  case MT_SLA_ACK:
  case MT_DATA_ACK:
    if (_next == _vec[_ix].size) {
      _ix += 1;
      _next = 0;
    }
    if (_next < _vec[_ix].size) {
      TWDR = _vec[_ix].buf[_next++];
      TWCR = DATA_CMD;
      _count++;
    } 
    else {
      TWCR = STOP_CMD;
      if (_handler != 0) _handler(this);
      loop_until_bit_is_clear(TWCR, TWSTO);
      _state = IDLE_STATE;
    }
    break;
  case MT_SLA_NACK:
  case MT_DATA_NACK:
    TWCR = STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    _state = IDLE_STATE;
    break;
  /**
   * Master Receiver Mode
   */
  case MR_DATA_ACK:
    _vec[_ix].buf[_next++] = TWDR;
    _count++;
  case MR_SLA_ACK:
    if (_next < (_vec[_ix].size - 1)) {
      TWCR = ACK_CMD;
    } 
    else {
      TWCR = NACK_CMD;
    }    
    break; 
  case MR_DATA_NACK:
    _vec[_ix].buf[_next++] = TWDR;
    _count++;
    TWCR = STOP_CMD;
    if (_handler != 0) {
      _handler(this);
      _next = 0;
    }
    loop_until_bit_is_clear(TWCR, TWSTO);
    _state = IDLE_STATE;
    break;      
  case MR_SLA_NACK:
    TWCR = STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    _state = IDLE_STATE;
    break;
  /**
   * Slave Transmitter Mode
   */
  case ST_SLA_ACK:
  case ST_ARB_LOST_SLA_ACK:
    if (_handler != 0) _handler(this);
    _next = 0;
  case ST_DATA_ACK:
    TWDR = _vec[_ix].buf[_next++];
    if (_next < _vec[_ix].size) {
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
    if (_next < _vec[_ix].size){
      _vec[_ix].buf[_next++] = TWDR;
      TWCR = ACK_CMD;
    }
    else {
      TWCR = NACK_CMD;
    }
    break;
  case SR_STOP:
    if (_next < _vec[_ix].size) {
      _vec[_ix].buf[_next] = 0;
    }
    TWCR = STOP_CMD;
    if (_handler != 0) _handler(this);
    loop_until_bit_is_clear(TWCR, TWSTO);
    _state = IDLE_STATE;
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
