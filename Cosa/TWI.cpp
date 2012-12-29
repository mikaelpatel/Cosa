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

#include "Cosa/TWI.hh"
#include "Cosa/Watchdog.hh"

/**
 * Default Two-write clock: 100 KHz
 */
#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif

TWI twi;

bool 
TWI::begin(Caso* target, uint8_t addr)
{
  m_target = target;
  m_addr = addr;

  // Check for slave mode and set device address
  if (target != 0 && addr != 0) {
    TWAR = m_addr;
  } 
  else {
    // Enable internal pullup
    synchronized {
      bit_set(PORTC, SDA);
      bit_set(PORTC, SCL);
    }
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
  m_target = 0;
  TWCR = 0;
  return (1);
}

bool
TWI::request(uint8_t addr)
{
  m_addr = addr;
  m_state = (addr & WRITE_OP) ? MT_STATE : MR_STATE;
  m_status = NO_INFO;
  m_count = 0;
  m_next = 0;
  m_ix = 0;
  TWCR = START_CMD;
  return (1);
}

bool
TWI::write_request(uint8_t addr, void* buf, uint8_t size)
{
  m_vec[0].buf = (uint8_t*) buf;
  m_vec[0].size = size;
  m_vec[1].buf = 0;
  m_vec[1].size = 0;
  return (request(addr | WRITE_OP));
}

bool
TWI::write_request(uint8_t addr, uint8_t header, void* buf, uint8_t size)
{
  m_buf[0] = header;
  m_vec[0].buf = m_buf;
  m_vec[0].size = sizeof(header);
  m_vec[1].buf = (uint8_t*) buf;
  m_vec[1].size = size;
  m_vec[2].buf = 0;
  m_vec[2].size = 0;
  return (request(addr | WRITE_OP));
}

bool
TWI::write_request(uint8_t addr, uint16_t header, void* buf, uint8_t size)
{
  m_buf[0] = (header >> 8);
  m_buf[1] = header;
  m_vec[0].buf = m_buf;
  m_vec[0].size = sizeof(header);
  m_vec[1].buf = (uint8_t*) buf;
  m_vec[1].size = size;
  m_vec[2].buf = 0;
  m_vec[2].size = 0;
  return (request(addr | WRITE_OP));
}

bool
TWI::read_request(uint8_t addr, void* buf, uint8_t size)
{
  m_vec[0].buf = (uint8_t*) buf;
  m_vec[0].size = size;
  m_vec[1].buf = 0;
  m_vec[1].size = 0;
  return (request(addr | READ_OP));
}

int
TWI::await_completed(uint8_t mode)
{
  do {
    cli();
    set_sleep_mode(mode);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  } while (m_state > IDLE_STATE);
  return (m_count);
}

ISR(TWI_vect) 
{
  twi.m_status = TWI_STATUS(TWSR);
  switch (twi.m_status) {
  case TWI::START:
  case TWI::REP_START:
    TWDR = twi.m_addr;
    TWCR = TWI::DATA_CMD;
    break;
  case TWI::ARB_LOST:
    TWCR = TWI::IDLE_CMD;
    twi.m_state = TWI::ERROR_STATE;
    break;
  /**
   * Master Transmitter Mode
   */
  case TWI::MT_SLA_ACK:
  case TWI::MT_DATA_ACK:
    if (twi.m_next == twi.m_vec[twi.m_ix].size) {
      twi.m_ix += 1;
      twi.m_next = 0;
    }
    if (twi.m_next < twi.m_vec[twi.m_ix].size) {
      TWDR = twi.m_vec[twi.m_ix].buf[twi.m_next++];
      TWCR = TWI::DATA_CMD;
      twi.m_count++;
      break;
    } 
    if (twi.m_target != 0) 
      Event::push(Event::WRITE_COMPLETED_TYPE, twi.m_target, &twi);
  case TWI::MT_SLA_NACK:
  case TWI::MT_DATA_NACK:
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_state = TWI::IDLE_STATE;
    break;
  /**
   * Master Receiver Mode
   */
  case TWI::MR_DATA_ACK:
    twi.m_vec[twi.m_ix].buf[twi.m_next++] = TWDR;
    twi.m_count++;
  case TWI::MR_SLA_ACK:
    if (twi.m_next < (twi.m_vec[twi.m_ix].size - 1)) {
      TWCR = TWI::ACK_CMD;
    } 
    else {
      TWCR = TWI::NACK_CMD;
    }    
    break; 
  case TWI::MR_DATA_NACK:
    twi.m_vec[twi.m_ix].buf[twi.m_next++] = TWDR;
    twi.m_count++;
    if (twi.m_target != 0) {
      Event::push(Event::READ_COMPLETED_TYPE, twi.m_target, &twi);
      twi.m_next = 0;
    }
  case TWI::MR_SLA_NACK:
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    twi.m_state = TWI::IDLE_STATE;
    break;
  /**
   * Slave Transmitter Mode
   */
  case TWI::ST_SLA_ACK:
  case TWI::ST_ARB_LOST_SLA_ACK:
    twi.m_state = TWI::ST_STATE;
    twi.m_next = 0;
    twi.m_ix = 0;
    twi.m_vec[twi.m_ix].size = 4;
  case TWI::ST_DATA_ACK:
    TWDR = twi.m_vec[twi.m_ix].buf[twi.m_next++];
    if (twi.m_next < twi.m_vec[twi.m_ix].size) {
      TWCR = TWI::ACK_CMD;
    } 
    else {
      TWCR = TWI::NACK_CMD;
    }
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
    twi.m_state = TWI::SR_STATE;
    twi.m_next = 0;
    TWCR = TWI::ACK_CMD;
    break;
  case TWI::SR_DATA_ACK:
  case TWI::SR_GCALL_DATA_ACK:
    if (twi.m_next < twi.m_vec[twi.m_ix].size) {
      twi.m_vec[twi.m_ix].buf[twi.m_next++] = TWDR;
      TWCR = TWI::ACK_CMD;
    }
    else {
      TWCR = TWI::NACK_CMD;
    }
    break;
  case TWI::SR_STOP:
    TWCR = TWI::STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    if (twi.m_target != 0) 
      Event::push(Event::SERVICE_REQUEST_TYPE, twi.m_target, &twi);
    twi.m_state = TWI::IDLE_STATE;
    TWCR = TWI::IDLE_CMD; 
    break;
  case TWI::SR_DATA_NACK:
  case TWI::SR_GCALL_DATA_NACK:
    TWCR = TWI::NACK_CMD;
    break;
  case TWI::NO_INFO:
    break;
  case TWI::BUS_ERROR:
  default:     
    TWCR = TWI::IDLE_CMD; 
  }
}



