/**
 * @file NRF24L01P.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "NRF24L01P.hh"
#if !defined(BOARD_ATTINYX5)

#include "Cosa/Power.hh"
#include "Cosa/RTT.hh"
#include <util/delay.h>

NRF24L01P::NRF24L01P(uint16_t net, uint8_t dev,
		     Board::DigitalPin csn,
		     Board::DigitalPin ce,
		     Board::ExternalInterruptPin irq) :
  SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV4_CLOCK, 0, SPI::MSB_ORDER, &m_irq),
  Wireless::Driver(net, dev),
  m_ce(ce, 0),
  m_irq(irq, ExternalInterrupt::ON_FALLING_MODE, this),
  m_status(0),
  m_state(POWER_DOWN_STATE),
  m_trans(0),
  m_retrans(0),
  m_drops(0)
{
  channel(64);
}

uint8_t
NRF24L01P::read(Command cmd)
{
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(cmd);
      uint8_t res = spi.transfer(0);
    spi.end();
  spi.release();
  return (res);
}

void
NRF24L01P::read(Command cmd, void* buf, size_t size)
{
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(cmd);
      spi.read(buf, size);
    spi.end();
  spi.release();
}

void
NRF24L01P::write(Command cmd)
{
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(cmd);
    spi.end();
  spi.release();
}

void
NRF24L01P::write(Command cmd, uint8_t data)
{
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(cmd);
      spi.transfer(data);
    spi.end();
  spi.release();
}

void
NRF24L01P::write(Command cmd, const void* buf, size_t size)
{
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(cmd);
      spi.write(buf, size);
    spi.end();
  spi.release();
}

NRF24L01P::status_t
NRF24L01P::read_status()
{
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(NOP);
    spi.end();
  spi.release();
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
NRF24L01P::receiver_mode()
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
NRF24L01P::transmit_mode(uint8_t dest)
{
  // Setup primary transmit address
  addr_t tx_addr(m_addr.network, dest);
  write(TX_ADDR, &tx_addr, sizeof(tx_addr));

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
  delay(32);
  m_ce.clear();
  write(CONFIG, (_BV(EN_CRC) | _BV(CRCO)));
  m_state = POWER_DOWN_STATE;
}

bool
NRF24L01P::begin(const void* config)
{
  UNUSED(config);

  // Check that a device is available
  if (UNLIKELY(read_status().reserved)) return (false);

  // Setup hardware features, channel, bitrate, retransmission, dynamic payload
  write(FEATURE, (_BV(EN_DPL) | _BV(EN_ACK_PAY) | _BV(EN_DYN_ACK)));
  write(RF_CH, m_channel);
  write(RF_SETUP, (RF_DR_2MBPS | RF_PWR_0DBM));
  write(SETUP_RETR, ((DEFAULT_ARD << ARD) | (DEFAULT_ARC << ARC)));
  write(DYNPD, DPL_PA);

  // Setup hardware receive pipes address; network (16-bit), device (8-bit)
  // P0: auto-acknowledge (see transmit_mode)
  // P1: node address<network:device> with auto-acknowledge
  // P2: broadcast<network:0>
  addr_t rx_addr = m_addr;
  write(SETUP_AW, AW_3BYTES);
  write(RX_ADDR_P1, &rx_addr, sizeof(rx_addr));
  write(RX_ADDR_P2, BROADCAST);
  write(EN_RXADDR, (_BV(ERX_P2) | _BV(ERX_P1)));
  write(EN_AA, (_BV(ENAA_P1) | _BV(ENAA_P0)));

  // Ready to go
  powerup();
  spi.attach(this);
  m_irq.enable();
  return (true);
}

int
NRF24L01P::send(uint8_t dest, uint8_t port, const iovec_t* vec)
{
  // Sanity check the payload size
  if (UNLIKELY(vec == NULL)) return (EINVAL);
  size_t len = iovec_size(vec);
  if (UNLIKELY(len > PAYLOAD_MAX)) return (EMSGSIZE);

  // Setting transmit destination
  transmit_mode(dest);

  // Write source address and payload to the transmit fifo
  // Fix: Allow larger payload(30*3) with fragmentation
  spi.acquire(this);
    spi.begin();
      uint8_t command = ((dest != BROADCAST)
			 ? W_TX_PAYLOAD
			 : W_TX_PAYLOAD_NO_ACK);
      m_status = spi.transfer(command);
      spi.transfer(m_addr.device);
      spi.transfer(port);
      spi.write(vec);
    spi.end();
  spi.release();
  m_trans += 1;

  // Check for auto-acknowledge pipe(0), and address setup and enable
  if (dest != BROADCAST) {
    addr_t tx_addr(m_addr.network, dest);
    write(RX_ADDR_P0, &tx_addr, sizeof(tx_addr));
    write(EN_RXADDR, (_BV(ERX_P2) | _BV(ERX_P1) | _BV(ERX_P0)));
  }

  // Wait for transmission
  do {
    yield();
    read_status();
  } while (!m_status.tx_ds && !m_status.max_rt);
  bool data_sent = m_status.tx_ds;

  // Check for auto-acknowledge pipe(0) disable
  if (dest != BROADCAST) {
    write(EN_RXADDR, (_BV(ERX_P2) | _BV(ERX_P1)));
  }

  // Reset status bits and read retransmission counter and update
  write(STATUS, _BV(MAX_RT) | _BV(TX_DS));
  observe_tx_t observe = read_observe_tx();
  m_retrans += observe.arc_cnt;

  // Check that the message was delivered
  if (data_sent) return (len);

  // Failed to delivery
  write(FLUSH_TX);
  m_drops += 1;
  return (EIO);
}

int
NRF24L01P::send(uint8_t dest, uint8_t port, const void* buf, size_t len)
{
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(dest, port, vec));
}

bool
NRF24L01P::available()
{
  // Check the receiver fifo
  if (read_fifo_status().rx_empty) return (false);

  // Sanity check the size of the payload. Might require a flush
  if (read(R_RX_PL_WID) <= DEVICE_PAYLOAD_MAX) return (true);
  write(FLUSH_RX);
  return (false);
}

int
NRF24L01P::recv(uint8_t& src, uint8_t& port,
		void* buf, size_t size,
		uint32_t ms)
{
  // Run in receiver mode
  receiver_mode();

  // Check if there is data available on any pipe
  uint32_t start = RTT::millis();
  while (!available()) {
    if ((ms != 0) && (RTT::since(start) > ms)) return (ETIME);
    yield();
  }
  m_dest = (m_status.rx_p_no == 1 ? m_addr.device : BROADCAST);
  write(STATUS, _BV(RX_DR));

  // Check for payload error from device (Tab. 20, pp. 51, R_RX_PL_WID)
  uint8_t count = read(R_RX_PL_WID) - 2;
  if ((count > PAYLOAD_MAX) || (count > size)) {
    write(FLUSH_RX);
    return (EMSGSIZE);
  }

  // Read the source address, port and payload
  spi.acquire(this);
    spi.begin();
      m_status = spi.transfer(R_RX_PAYLOAD);
      src = spi.transfer(0);
      port = spi.transfer(0);
      spi.read(buf, count);
    spi.end();
  spi.release();
  return (count);
}

void
NRF24L01P::output_power_level(int8_t dBm)
{
  uint8_t pwr = RF_PWR_0DBM;
  if      (dBm < -12) pwr = RF_PWR_18DBM;
  else if (dBm < -6)  pwr = RF_PWR_12DBM;
  else if (dBm < 0)   pwr = RF_PWR_6DBM;
  write(RF_SETUP, (RF_DR_2MBPS | pwr));
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
