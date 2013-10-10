/**
 * @file Cosa/Wireless/Driver/NRF24L01P.cpp
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

#include "Cosa/Wireless/Driver/NRF24L01P.hh"
#if !defined(__ARDUINO_TINYX5__)

#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"
#include "Cosa/RTC.hh"
#include <util/delay.h>

void
NRF24L01P::powerup()
{
  if (m_state != POWER_DOWN_STATE) return;
  m_ce.clear();

  // Start SPI interaction block
  spi.begin(this);
  
  // Setup hardware features, channel, bitrate, retransmission, dynamic payload
  write(FEATURE, (_BV(EN_DPL) | _BV(EN_ACK_PAY) | _BV(EN_DYN_ACK)));
  write(RF_CH, m_channel);
  write(RF_SETUP, (RF_DR_2MBPS | RF_PWR_0DBM));
  write(SETUP_RETR, ((2 << ARD) | (15 << ARC)));
  write(DYNPD, DPL_PA);

  // Setup hardware receive pipes (0:ack, 1:device, 2:broadcast)
  addr_t rx_addr(0,0);
  write(SETUP_AW, AW_3BYTES);
  write(RX_ADDR_P0);
  write(&rx_addr, sizeof(rx_addr));
  rx_addr = m_addr;
  write(RX_ADDR_P1);
  write(&rx_addr, sizeof(rx_addr));
  rx_addr.device = 0;
  write(RX_ADDR_P2);
  write(&rx_addr, sizeof(rx_addr));
  write(EN_RXADDR, (_BV(ERX_P0) | _BV(ERX_P1) | _BV(ERX_P2))); 

  // Auto-acknowledgement on device pipe
  write(EN_AA, _BV(ENAA_P1));

  // Setup configuration for powerup and clear interrupts
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO)  | _BV(PWR_UP)));
  write(STATUS, (_BV(RX_DR)  | _BV(TX_DS) | _BV(MAX_RT)));

  // End of SPI block
  spi.end();
  
  _delay_ms(Tpd2stby_ms);
  m_state = STANDBY_STATE;
}

void
NRF24L01P::set_receiver_mode()
{
  // Check already in receive mode
  if (m_state == RX_STATE) return;

  // Fix: Should be handled by the interrupt handler on TX_DS interrupt
  if (m_state == TX_STATE) Watchdog::delay(16);

  // Configure primary receiver mode
  spi.begin(this);
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP) | _BV(PRIM_RX)));
  spi.end();

  m_ce.set();
  if (m_state == STANDBY_STATE) _delay_us(Tstby2a_us);
  m_state = RX_STATE;
}

void
NRF24L01P::set_transmit_mode(uint8_t dest)
{
  // Setup primary transmit address and acknowledge address
  addr_t tx_addr(m_addr.network, dest);
  spi.begin(this);
  write(TX_ADDR);
  write(&tx_addr, sizeof(tx_addr));
  write(RX_ADDR_P0);
  write(&tx_addr, sizeof(tx_addr));  
  if (m_state != TX_STATE) {
    bool state = m_ce.is_set();
    m_ce.clear();
    write(CONFIG, (_BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP)));
    m_ce.pulse(10);
    DELAY(10);
    m_ce.set(state);
  }
  spi.end();

  // Wait for the transmitter to become active
  if (m_state == STANDBY_STATE) _delay_us(Tstby2a_us);
  m_state = TX_STATE;
}

void
NRF24L01P::standby()
{
  m_ce.clear();
  _delay_us(Thce_us);
  m_state = STANDBY_STATE;
}

void
NRF24L01P::powerdown()
{
  m_ce.clear();
  spi.begin(this);
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO)));
  spi.end();
  m_state = POWER_DOWN_STATE;
}

int
NRF24L01P::send(uint8_t dest, const void* buf, size_t size)
{
  // Check buffer and payload size
  if ((buf == NULL) || (size > PAYLOAD_MAX)) return (-1);

  // Check if previous send did not succeed
  if (is_max_retransmit()) {
    spi.begin(this);
    write(STATUS, _BV(MAX_RT));
    write(FLUSH_TX);
    spi.end();
  }

  // Wait for room before setting transmit destination
  while (!room()) Power::sleep(SLEEP_MODE_IDLE);
  set_transmit_mode(dest);

  // Write source address and payload to the transmit fifo
  spi.begin(this);
  write(W_TX_PAYLOAD);
  write(m_addr.device);
  write(buf, size);
  spi.end();
  
  // Return size of payload actually sent
  return (size);
}

int
NRF24L01P::recv(uint8_t& src, void* buf, size_t size, uint32_t ms)
{
  // Check if there is data available on any pipe
  uint32_t start = RTC::millis();
  while (!available()) {
    if (ms != 0 && (RTC::since(start) > ms)) return (-2);
    Power::sleep(SLEEP_MODE_IDLE);
  } 

  // Read the payload size
  uint8_t count;
  spi.begin(this);
  count = read(R_RX_PL_WID);
  spi.end();

  // Check for payload error from device (Tab. 20, pp. 51, R_RX_PL_WID)
  if ((count > PAYLOAD_MAX) || (count > size)) {
    write(FLUSH_RX);
    return (-1);
  }
  size = count - 1;

  // Read the payload
  spi.begin(this);
  write(R_RX_PAYLOAD);
  src = read();
  read(buf, size);
  write(STATUS, _BV(RX_DR));
  spi.end();
  return (size);
}

void 
NRF24L01P::IRQPin::on_interrupt(uint16_t arg)
{ 
  // Interrupts will wake any sleep
}

// Output operators for bitfield status registers
IOStream& operator<<(IOStream& outs, NRF24L01P::status_t status)
{
  outs << PSTR("RX_DR = ") << status.rx_dr 
       << PSTR(", TX_DS = ") << status.tx_ds
       << PSTR(", MAX_RT = ") << status.max_rt
       << PSTR(", RX_P_NO = ") << status.rx_p_no
       << PSTR(", TX_FULL = ") << status.tx_full;
  return (outs);
}

IOStream& operator<<(IOStream& outs, NRF24L01P::observe_tx_t observe)
{
  outs << PSTR("PLOS_CNT = ") << observe.plos_cnt
       << PSTR(", ARC_CNT = ") << observe.arc_cnt;
  return (outs);
}

IOStream& operator<<(IOStream& outs, NRF24L01P::fifo_status_t fifo)
{
  outs << PSTR("RX_EMPTY = ") << fifo.rx_empty
       << PSTR(", RX_FULL = ") << fifo.rx_full
       << PSTR(", TX_EMPTY = ") << fifo.tx_empty 
       << PSTR(", TX_FULL = ") << fifo.tx_full 
       << PSTR(", TX_REUSE = ") << fifo.tx_reuse;
  return (outs);
}

#endif
