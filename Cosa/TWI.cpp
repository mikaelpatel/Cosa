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
TWI::begin(Thing* target, uint8_t addr)
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

void
TWI::on_bus_event()
{
  m_status = TWI_STATUS(TWSR);
  switch (m_status) {
  case START:
  case REP_START:
    TWDR = m_addr;
    TWCR = DATA_CMD;
    break;
  case ARB_LOST:
    TWCR = IDLE_CMD;
    m_state = ERROR_STATE;
    break;
  /**
   * Master Transmitter Mode
   */
  case MT_SLA_ACK:
  case MT_DATA_ACK:
    if (m_next == m_vec[m_ix].size) {
      m_ix += 1;
      m_next = 0;
    }
    if (m_next < m_vec[m_ix].size) {
      TWDR = m_vec[m_ix].buf[m_next++];
      TWCR = DATA_CMD;
      m_count++;
      break;
    } 
    if (m_target != 0) 
      Event::push(Event::WRITE_COMPLETED_TYPE, m_target, &twi);
  case MT_SLA_NACK:
  case MT_DATA_NACK:
    TWCR = STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    m_state = IDLE_STATE;
    break;
  /**
   * Master Receiver Mode
   */
  case MR_DATA_ACK:
    m_vec[m_ix].buf[m_next++] = TWDR;
    m_count++;
  case MR_SLA_ACK:
    if (m_next < (m_vec[m_ix].size - 1)) {
      TWCR = ACK_CMD;
    } 
    else {
      TWCR = NACK_CMD;
    }    
    break; 
  case MR_DATA_NACK:
    m_vec[m_ix].buf[m_next++] = TWDR;
    m_count++;
    if (m_target != 0) {
      Event::push(Event::READ_COMPLETED_TYPE, m_target, &twi);
      m_next = 0;
    }
  case MR_SLA_NACK:
    TWCR = STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    m_state = IDLE_STATE;
    break;
  /**
   * Slave Transmitter Mode
   */
  case ST_SLA_ACK:
  case ST_ARB_LOST_SLA_ACK:
    m_state = ST_STATE;
    m_next = 0;
    m_ix = 0;
    m_vec[m_ix].size = 4;
  case ST_DATA_ACK:
    TWDR = m_vec[m_ix].buf[m_next++];
    if (m_next < m_vec[m_ix].size) {
      TWCR = ACK_CMD;
    } 
    else {
      TWCR = NACK_CMD;
    }
    break;
  case ST_DATA_NACK:
  case ST_LAST_DATA:
    TWCR = ACK_CMD;
    m_state = IDLE_STATE;
    break;
  /**
   * Slave Receiver Mode
   */
  case SR_SLA_ACK:
  case SR_GCALL_ACK:
  case SR_ARB_LOST_SLA_ACK:
  case SR_ARB_LOST_GCALL_ACK:
    m_state = SR_STATE;
    m_next = 0;
    TWCR = ACK_CMD;
    break;
  case SR_DATA_ACK:
  case SR_GCALL_DATA_ACK:
    if (m_next < m_vec[m_ix].size) {
      m_vec[m_ix].buf[m_next++] = TWDR;
      TWCR = ACK_CMD;
    }
    else {
      TWCR = NACK_CMD;
    }
    break;
  case SR_STOP:
    TWCR = STOP_CMD;
    loop_until_bit_is_clear(TWCR, TWSTO);
    if (m_target != 0) 
      Event::push(Event::SERVICE_REQUEST_TYPE, m_target, &twi);
    m_state = IDLE_STATE;
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
  twi.on_bus_event();
}

