/**
 * @file CosaNRFsender.ino
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
 * Cosa demonstration of NRF24L01+ driver; sender
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/SPI/Driver/NRF24L01P.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Memory.h"

// NRF24L01+ Wireless communication using SPI and default pins(9, 10, 2)
NRF24L01P nrf;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNRFsender: started"));

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(nrf));

  // Start the watchdog ticks counter and push timeout events
  Watchdog::begin(1024, SLEEP_MODE_IDLE, Watchdog::push_watchdog_event);
  RTC::begin();

  // Power up the transceiver
  nrf.set_powerup_mode();

  // Check configuration and setup transmitter mode
  nrf.set_transmitter_mode("cosa1");

  // Print configuration
  TRACE(nrf.read(NRF24L01P::FEATURE));
  TRACE(nrf.read(NRF24L01P::RF_CH));
  TRACE(nrf.read(NRF24L01P::RF_SETUP));
  TRACE(nrf.read(NRF24L01P::RX_PW_P0));
  TRACE(nrf.read(NRF24L01P::RX_PW_P1));
  TRACE(nrf.read(NRF24L01P::RX_ADDR_P0));
  TRACE(nrf.read(NRF24L01P::RX_ADDR_P1));
  TRACE(nrf.read(NRF24L01P::RX_ADDR_P2));
  TRACE(nrf.read(NRF24L01P::RX_ADDR_P3));
  TRACE(nrf.read(NRF24L01P::RX_ADDR_P4));
  TRACE(nrf.read(NRF24L01P::RX_ADDR_P5));
  TRACE(nrf.read(NRF24L01P::SETUP_RETR));
  TRACE(nrf.read(NRF24L01P::SETUP_AW));
  TRACE(nrf.read(NRF24L01P::DYNPD));
  TRACE(nrf.read(NRF24L01P::CONFIG));
}

// Message block
struct msg_t {
  uint16_t id;
};
msg_t msg = { 0 };

void loop()
{
  Event event;
  Event::queue.await(&event);

  // Print event type (should be Event::WATCHDOG_TYPE)
  TRACE(event.get_type());

  // Print transceiver status
  NRF24L01P::observe_tx_t observe = nrf.read(NRF24L01P::OBSERVE_TX);
  NRF24L01P::status_t status = nrf.get_status();
  NRF24L01P::fifo_status_t fifo = nrf.read(NRF24L01P::FIFO_STATUS);

  TRACE(RTC::seconds());
  trace.printf_P(PSTR("STATUS(RX_DR = %d, TX_DS = %d, MAX_RT = %d, RX_P_NO = %d, TX_FULL = %d)\n"), status.rx_dr, status.tx_ds, status.max_rt, status.rx_p_no, status.tx_full);
  trace.printf_P(PSTR("OBSERVE_TX(PLOS_CNT = %d, ARC_CNT = %d)\n"), observe.plos_cnt, observe.arc_cnt);
  trace.printf_P(PSTR("FIFO_STATUS(RX_EMPTY = %d, RX_FULL = %d, TX_EMPTY = %d, TX_FULL = %d, TX_REUSE = %d)\n"), fifo.rx_empty, fifo.rx_full, fifo.tx_empty, fifo.tx_full, fifo.tx_reuse);

  // Attempt to send a message
  uint8_t res = nrf.send(&msg, sizeof(msg));
  trace.printf_P(PSTR("SEND(id = %d, res = %d)\n\n"), msg.id, res);
  if (res == sizeof(msg)) msg.id += 1;
  
  // Check if the transmission fifo needs flushing
  if (!nrf.is_ready() && nrf.is_max_retransmit()) nrf.flush();
}
