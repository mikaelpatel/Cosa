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
#include "Cosa/Trace.hh"

uint8_t 
NRF24L01P::read(Command cmd)
{
  spi.begin(this);
  m_status = spi.transfer(cmd);
  uint8_t res = spi.transfer(0);
  spi.end();
  return (res);
}

void 
NRF24L01P::read(Command cmd, void* buf, size_t size)
{
  spi.begin(this);
  m_status = spi.transfer(cmd);
  spi.read(buf, size);
  spi.end();
}

void 
NRF24L01P::write(Command cmd)
{
  spi.begin(this);
  m_status = spi.transfer(cmd);
  spi.end();
}

void 
NRF24L01P::write(Command cmd, uint8_t data)
{
  spi.begin(this);
  m_status = spi.transfer(cmd);
  spi.transfer(data);
  spi.end();
}

void 
NRF24L01P::write(Command cmd, const void* buf, size_t size)
{
  spi.begin(this);
  m_status = spi.transfer(cmd);
  spi.write(buf, size);
  spi.end();
}

NRF24L01P::status_t
NRF24L01P::read_status()
{
  spi.begin(this);
  m_status = spi.transfer(NOP);
  spi.end();
  return (m_status);
}

void
NRF24L01P::powerup()
{
  if (m_state != POWER_DOWN_STATE) return;
  m_ce.clear();

  // Setup configuration for powerup and clear interrupts
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO)  | _BV(PWR_UP)));
  _delay_ms(Tpd2stby_ms);
  m_state = STANDBY_STATE;

  // Flush status
  write(STATUS, (_BV(RX_DR)  | _BV(TX_DS) | _BV(MAX_RT)));
  write(FLUSH_TX);
  write(FLUSH_RX);
}

void
NRF24L01P::set_receiver_mode()
{
  // Check already in receive mode
  if (m_state == RX_STATE) return;

  // Configure primary receiver mode
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP) | _BV(PRIM_RX)));
  m_ce.set();
  if (m_state == STANDBY_STATE) _delay_us(Tstby2a_us);
  m_state = RX_STATE;
}

void
NRF24L01P::set_transmit_mode(uint8_t dest)
{
  // Setup primary transmit address
  addr_t tx_addr(m_addr.network, dest);
  write(TX_ADDR, &tx_addr, sizeof(tx_addr));
  write(RX_ADDR_P0, &tx_addr, sizeof(tx_addr));  

  // Trigger the transmitter mode
  if (m_state != TX_STATE) {
    m_ce.clear();
    write(CONFIG, (_BV(EN_CRC) | _BV(CRCO) | _BV(PWR_UP)));
    m_ce.set();
  }

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
  Watchdog::delay(32);
  m_ce.clear();
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO)));
  m_state = POWER_DOWN_STATE;
}

bool 
NRF24L01P::begin(const void* config)
{
  // Setup hardware features, channel, bitrate, retransmission, dynamic payload
  write(FEATURE, (_BV(EN_DPL) | _BV(EN_ACK_PAY) | _BV(EN_DYN_ACK)));
  write(RF_CH, m_channel);
  write(RF_SETUP, (RF_DR_2MBPS | RF_PWR_0DBM));
  write(SETUP_RETR, ((2 << ARD) | (15 << ARC)));
  write(DYNPD, DPL_PA);
  write(SETUP_AW, AW_3BYTES);

  // Setup hardware receive pipes (0:ack, 1:device, 2:broadcast)
  addr_t rx_addr = m_addr;
  write(RX_ADDR_P1, &rx_addr, sizeof(rx_addr));
  write(RX_ADDR_P2, 0);
  write(EN_RXADDR, (_BV(ERX_P2) | _BV(ERX_P1) | _BV(ERX_P0)));
  
  // Auto-acknowledgement on device pipe
  write(EN_AA, (_BV(ENAA_P2) | _BV(ENAA_P1) | _BV(ENAA_P0)));

  // Ready to go
  powerup();
  m_irq.enable();
  return (true);
}

int
NRF24L01P::send(uint8_t dest, const iovec_t* vec)
{
  // Sanity check the payload size
  if (vec == NULL) return (-1);
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++)
    len += vp->size;
  if (len > PAYLOAD_MAX) return (-1);

  // Setting transmit destination
  set_transmit_mode(dest);

  // Write source address and payload to the transmit fifo
  // Fix: Allow larger payload(30*3) with fragmentation
  spi.begin(this);
  m_status = spi.transfer(dest ? W_TX_PAYLOAD : W_TX_PAYLOAD_NO_ACK);
  spi.transfer(m_addr.device);
  for (const iovec_t* vp = vec; vp->buf != 0; vp++)
    spi.write(vp->buf, vp->size);
  spi.end();

  // Wait for transmission
  do {
    Power::sleep(m_mode);
    read_status();
  } while (!m_status.tx_ds && !m_status.max_rt);
  write(STATUS, _BV(MAX_RT) | _BV(TX_DS));

  // Check that the message was delivered
  if (m_status.tx_ds) return (len);

  // Failed to delivery
  write(FLUSH_TX);
  return (-2);
}

int 
NRF24L01P::send(uint8_t dest, const void* buf, size_t len)
{
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(dest, vec));
}

bool
NRF24L01P::available()
{
  // Check the receiver fifo
  if (read_fifo_status().rx_empty) return (false);

  // Sanity check the size of the payload. Might require a flush
  if (read(R_RX_PL_WID) > 32) {
    write(FLUSH_RX);
    return (false);
  }
  return (true);
}

int
NRF24L01P::recv(uint8_t& src, void* buf, size_t size, uint32_t ms)
{
  // Run in receiver mode
  set_receiver_mode();

  // Check if there is data available on any pipe
  uint32_t start = RTC::millis();
  while (!available()) {
    if ((ms != 0) && (RTC::since(start) > ms)) return (-2);
    Power::sleep(m_mode);
  } 
  m_dest = (m_status.rx_p_no == 1 ? m_addr.device : 0);
  write(STATUS, _BV(RX_DR));
  
  // Check for payload error from device (Tab. 20, pp. 51, R_RX_PL_WID)
  uint8_t count = read(R_RX_PL_WID) - 1;
  if ((count > PAYLOAD_MAX) || (count > size)) {
    write(FLUSH_RX);
    return (-1);
  }

  // Read the source address and payload
  spi.begin(this);
  m_status = spi.transfer(R_RX_PAYLOAD);
  src = spi.transfer(0);
  spi.read(buf, count);
  spi.end();
  return (count);
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
