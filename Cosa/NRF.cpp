/**
 * @file Cosa/NRF.cpp
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
 * SPI driver for the nRF24L01 Single Chip 2.4GHz Transceiver.
 * See nRF24L01+ Product Specification (Rev. 1.0)
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/NRF.h"
#include <util/delay.h>

// Timing information (ch. 6.1.7, pp. 24)

#define Tpd2stby 4.5
#define Tstby2a 130.0
#define Thce 10.0

NRF::NRF(uint8_t channel, uint8_t csn, uint8_t ce, uint8_t irq) : 
  SPI(), 
  _status(0),
  _channel(channel),
  _csn(csn, 1),
  _ce(ce, 0),
  _irq(irq, InterruptPin::ON_FALLING_MODE),
  _state(POWER_DOWN_STATE)
{
  begin(SPI::DIV4_CLOCK, 0, SPI::MSB_FIRST);
}

uint8_t
NRF::read(Register reg)
{
  uint8_t res;
  SPI_transaction(_csn) {
    res = SPI::read(R_REGISTER | (REG_MASK & reg));
  }
  return (res);
}

uint8_t 
NRF::write(Register reg, uint8_t data)
{
  SPI_transaction(_csn) {
    _status = SPI::write(W_REGISTER | (REG_MASK & reg), data);
  }
  return (_status);
}

uint8_t 
NRF::write(Register reg, const void* buffer, uint8_t count)
{
  SPI_transaction(_csn) {
    _status = SPI::write(W_REGISTER | (REG_MASK & reg), buffer, count);
  }
  return (_status);
}

void 
NRF::set_interrupt(InterruptPin::Callback fn)
{
  _irq.set(fn, this);
  if (fn != 0) _irq.enable();
}

void
NRF::set_powerup_mode()
{
  if (_state != POWER_DOWN_STATE) return;
  _ce.clear();
  write(FEATURE, _BV(EN_DPL) | _BV(EN_ACK_PAY) | _BV(EN_DYN_ACK));
  write(RF_CH, _channel);
  write(RF_SETUP, RF_DR_2MBPS | RF_PWR_0DBM);
  write(SETUP_RETR, (2 << ARD) | (15 << ARC));  
  write(EN_AA, ENAA_PA);
  write(EN_RXADDR, ERX_PA);           
  write(DYNPD, DPL_PA);
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP));
  _delay_ms(Tpd2stby);
  _state = STANDBY_STATE;
}

void
NRF::set_receiver_mode(const void* addr, uint8_t width)
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
  _ce.set();
  _delay_us(Tstby2a);
  _state = RX_STATE;
  flush();
}

void
NRF::set_transmitter_mode(const void* addr, uint8_t width)
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
  _ce.set();
  _delay_us(Tstby2a);
  _state = TX_STATE;
  flush();
}

void
NRF::set_standby_mode()
{
  _delay_us(Thce);
  _ce.clear();
  _state = STANDBY_STATE;
}

void
NRF::set_powerdown_mode()
{
  _ce.clear();
  write(CONFIG, _BV(EN_CRC) | _BV(CRCO));
  _state = POWER_DOWN_STATE;
}

bool
NRF::is_available()
{
  uint8_t status = get_status();
  return ((status & RX_P_NO_MASK) != RX_P_NO_MASK);
}

uint8_t
NRF::recv(void* buffer, uint8_t* pipe)
{
  uint8_t status = get_status();
  uint8_t no = (status & RX_P_NO_MASK);
  if (no == RX_P_NO_MASK) return (0);
  if (pipe != 0) *pipe = no >> RX_P_NO;
  uint8_t count;
  SPI_transaction(_csn) {
    count = SPI::read(R_RX_PL_WID);
  }
  SPI_transaction(_csn) {
    _status = SPI::read(R_RX_PAYLOAD, buffer, count);
  }
  return (count);
}

bool
NRF::is_ready()
{
  uint8_t status = get_status();
  return ((status & (_BV(TX_FIFO_FULL))) == 0);
}

bool
NRF::is_max_retransmit()
{
  uint8_t status = get_status();
  return ((status & _BV(MAX_RT)) != 0);
}

bool
NRF::is_max_lost()
{
  uint8_t observe = read(OBSERVE_TX);
  return ((observe >> PLOS_CNT) == 0xf);
}

uint8_t
NRF::ack(const void* buffer, uint8_t count, uint8_t pipe)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(_csn) {
    _status = SPI::write(W_ACK_PAYLOAD | (PIPE_MASK & pipe), buffer, count);
  }
  return (count);
}

uint8_t
NRF::ack_P(const void* buffer, uint8_t count, uint8_t pipe)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(_csn) {
    _status = SPI::write_P(W_ACK_PAYLOAD | (PIPE_MASK & pipe), buffer, count);
  }
  return (count);
}

uint8_t
NRF::send(const void* buffer, uint8_t count)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(_csn) {
    _status = SPI::write(W_TX_PAYLOAD, buffer, count);
  }
  return (count);
}

uint8_t
NRF::send_P(const void* buffer, uint8_t count)
{
  if (!is_ready()) return (0);
  if (count > PAYLOAD_MAX) count = PAYLOAD_MAX;
  SPI_transaction(_csn) {
    _status = SPI::write_P(W_TX_PAYLOAD, buffer, count);
  }
  return (count);
}

uint8_t 
NRF::flush()
{
  SPI_transaction(_csn) {
    _status = SPI::exchange(FLUSH_RX);
    _status = SPI::exchange(FLUSH_TX);
  }
  write(STATUS, 0xf0);
  if (is_max_lost()) {
    write(RF_CH, read(RF_CH));
    _status = 0xff;
  }
  return (_status);
}

void 
NRF::push_event(InterruptPin* pin, void* env)
{ 
  NRF* nrf = (NRF*) env;
  uint8_t status = nrf->get_status();
  if (status & _BV(RX_DR)) {
    nrf->write(STATUS, _BV(RX_DR));
    Event::push(Event::NRF_RECEIVE_DATA_TYPE, nrf, status);
  }
}
