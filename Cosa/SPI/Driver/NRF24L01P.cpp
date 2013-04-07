/**
 * @file Cosa/SPI/Driver/NRF24L01P.cpp
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
#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/SPI/Driver/NRF24L01P.hh"
#include <util/delay.h>

// Timing information (ch. 6.1.7, pp. 24)

#define Tpd2stby 4.5
#define Tstby2a 130.0
#define Thce 10.0

NRF24L01P::NRF24L01P(uint8_t channel, 
		     Board::DigitalPin csn, 
		     Board::DigitalPin ce, 
		     Board::ExternalInterruptPin irq) : 
  m_status(0),
  m_channel(channel),
  m_csn(csn, 1),
  m_ce(ce, 0),
  m_irq(irq, ExternalInterruptPin::ON_FALLING_MODE, this),
  m_state(POWER_DOWN_STATE)
{
  begin();
}

uint8_t
NRF24L01P::read(Register reg)
{
  uint8_t res;
  SPI_transaction(m_csn) {
    res = spi.read(R_REGISTER | (REG_MASK & reg));
  }
  return (res);
}

uint8_t 
NRF24L01P::write(Register reg, uint8_t data)
{
  SPI_transaction(m_csn) {
    m_status = spi.write(W_REGISTER | (REG_MASK & reg), data);
  }
  return (m_status);
}

uint8_t 
NRF24L01P::write(Register reg, const void* buffer, uint8_t count)
{
  SPI_transaction(m_csn) {
    m_status = spi.write(W_REGISTER | (REG_MASK & reg), buffer, count);
  }
  return (m_status);
}

void
NRF24L01P::set_powerup_mode()
{
  if (m_state != POWER_DOWN_STATE) return;
  m_ce.clear();
  write(FEATURE, _BV(EN_DPL) | _BV(EN_ACK_PAY) | _BV(EN_DYN_ACK));
  write(RF_CH, m_channel);
  write(RF_SETUP, RF_DR_2MBPS | RF_PWR_0DBM);
  write(SETUP_RETR, (2 << ARD) | (15 << ARC));  
  write(EN_AA, ENAA_PA);
  write(EN_RXADDR, ERX_PA);           
  write(DYNPD, DPL_PA);
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP));
  _delay_ms(Tpd2stby);
  m_state = STANDBY_STATE;
}

void
NRF24L01P::set_receiver_mode(const void* addr, uint8_t width)
{
  if (width > AW_MAX) width = AW_MAX;
  else if (width < 3) width = 3;
  write(SETUP_AW, (width - 2) & 0x3);
  write(RX_ADDR_P1, addr, width);
  uint8_t nr = ((uint8_t*) addr)[width - 1] + 1;
  write(RX_ADDR_P2, nr++);
  write(RX_ADDR_P3, nr++);
  write(RX_ADDR_P4, nr++);
  write(RX_ADDR_P5, nr);
  write(CONFIG, 
	_BV(MASK_TX_DS) | _BV(MASK_MAX_RT) | 
	_BV(EN_CRC) | _BV(CRCO) | 
	_BV(PWR_UP) | 
	_BV(PRIM_RX));
  m_ce.set();
  _delay_us(Tstby2a);
  m_state = RX_STATE;
  flush();
}

void
NRF24L01P::set_transmitter_mode(const void* addr, uint8_t width)
{
  if (width > AW_MAX) width = AW_MAX;
  else if (width < 3) width = 3;
  write(SETUP_AW, (width - 2) & 0x3);
  write(TX_ADDR, addr, width);
  write(RX_ADDR_P0, addr, width);
  write(CONFIG, 
	_BV(MASK_RX_DR) | _BV(MASK_TX_DS) | _BV(MASK_MAX_RT) | 
	_BV(EN_CRC) | _BV(CRCO) | 
	_BV(PWR_UP));
  m_ce.set();
  _delay_us(Tstby2a);
  m_state = TX_STATE;
  flush();
}

void
NRF24L01P::set_standby_mode()
{
  _delay_us(Thce);
  m_ce.clear();
  m_state = STANDBY_STATE;
}

void
NRF24L01P::set_powerdown_mode()
{
  m_ce.clear();
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO));
  m_state = POWER_DOWN_STATE;
}

bool
NRF24L01P::is_available()
{
  uint8_t status = get_status();
  return ((status & RX_P_NO_MASK) != RX_P_NO_MASK);
}

uint8_t
NRF24L01P::recv(void* buffer, uint8_t* pipe)
{
  uint8_t status = get_status();
  uint8_t no = (status & RX_P_NO_MASK);
  if (no == RX_P_NO_MASK) return (0);
  if (pipe != 0) *pipe = no >> RX_P_NO;
  uint8_t count;
  SPI_transaction(m_csn) {
    count = spi.read(R_RX_PL_WID);
  }
  SPI_transaction(m_csn) {
    m_status = spi.read(R_RX_PAYLOAD, buffer, count);
  }
  return (count);
}

bool
NRF24L01P::is_ready()
{
  uint8_t status = get_status();
  return ((status & (_BV(TX_FIFO_FULL))) == 0);
}

bool
NRF24L01P::is_max_retransmit()
{
  uint8_t status = get_status();
  return ((status & _BV(MAX_RT)) != 0);
}

bool
NRF24L01P::is_max_lost()
{
  uint8_t observe = read(OBSERVE_TX);
  return ((observe >> PLOS_CNT) == 0xf);
}

uint8_t
NRF24L01P::ack(const void* buffer, uint8_t count, uint8_t pipe)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(m_csn) {
    m_status = spi.write(W_ACK_PAYLOAD | (PIPE_MASK & pipe), buffer, count);
  }
  return (count);
}

uint8_t
NRF24L01P::ack_P(const void* buffer, uint8_t count, uint8_t pipe)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(m_csn) {
    m_status = spi.write_P(W_ACK_PAYLOAD | (PIPE_MASK & pipe), buffer, count);
  }
  return (count);
}

uint8_t
NRF24L01P::send(const void* buffer, uint8_t count)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(m_csn) {
    m_status = spi.write(W_TX_PAYLOAD, buffer, count);
  }
  return (count);
}

uint8_t
NRF24L01P::send_P(const void* buffer, uint8_t count)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(m_csn) {
    m_status = spi.write_P(W_TX_PAYLOAD, buffer, count);
  }
  return (count);
}

uint8_t 
NRF24L01P::flush()
{
  SPI_transaction(m_csn) {
    m_status = spi.exchange(FLUSH_RX);
    m_status = spi.exchange(FLUSH_TX);
  }
  write(STATUS, 0xf0);
  if (is_max_lost()) {
    write(RF_CH, read(RF_CH));
    m_status = 0xff;
  }
  return (m_status);
}

void 
NRF24L01P::IRQPin::on_interrupt(uint16_t arg)
{ 
  uint8_t status = m_nrf->get_status();
  if (status & _BV(RX_DR)) {
    m_nrf->write(NRF24L01P::STATUS, _BV(RX_DR));
    Event::push(Event::RECEIVE_COMPLETED_TYPE, m_nrf, status);
  }
}
#endif
