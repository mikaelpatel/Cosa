/**
 * @file CosaNRFsender.ino
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
 * Cosa demonstration of NRF24L01+ driver; sender
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/NRF.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Memory.h"

// NRF24L01+ Wireless communication using SPI and default pins(9, 10, 2)

NRF nrf;

// Configuration

#define USE_WATCHDOG_DELAY
// #define USE_EVENT_AWAIT

void setup()
{
  // Start serial interface for trace output
  Serial.begin(9600);

  // Check amount of free memory
  Serial_trace(free_memory());
  Serial_trace(sizeof(NRF));

  // Start the watchdog ticks counter and push timeout events
#ifdef USE_EVENT_AWAIT
  Watchdog::begin(1024, Watchdog::push_event);
#endif
#ifdef USE_WATCHDOG_DELAY
  Watchdog::begin(64);
#endif

  // Power up the transceiver
  nrf.set_powerup_mode();

  // Check configuration and setup transmitter mode
  nrf.set_transmitter_mode("cosa1");
  Serial_trace(nrf.read(NRF::FEATURE));
  Serial_trace(nrf.read(NRF::RF_CH));
  Serial_trace(nrf.read(NRF::RF_SETUP));
  Serial_trace(nrf.read(NRF::RX_PW_P0));
  Serial_trace(nrf.read(NRF::RX_PW_P1));
  Serial_trace(nrf.read(NRF::RX_ADDR_P0));
  Serial_trace(nrf.read(NRF::RX_ADDR_P1));
  Serial_trace(nrf.read(NRF::RX_ADDR_P2));
  Serial_trace(nrf.read(NRF::RX_ADDR_P3));
  Serial_trace(nrf.read(NRF::RX_ADDR_P4));
  Serial_trace(nrf.read(NRF::RX_ADDR_P5));
  Serial_trace(nrf.read(NRF::SETUP_RETR));
  Serial_trace(nrf.read(NRF::SETUP_AW));
  Serial_trace(nrf.read(NRF::DYNPD));
  Serial_trace(nrf.read(NRF::CONFIG));

  // Turn off Arduino timer0 to reduce wakeups
  TIMSK0 = 0;
}

typedef struct msg_t msg_t;
struct msg_t {
  uint16_t id; 
  uint8_t observe;
  uint8_t status;
};

msg_t msg = { 0 };
const uint8_t count = sizeof(msg);

void loop()
{
#ifdef USE_WATCHDOG_DELAY
  Watchdog::delay(1024);
#endif
#ifdef USE_EVENT_AWAIT
  Event event;
  Event::queue.await(&event);
  Serial_trace(event.get_type());
#endif

  // Attempt to send a message
  msg.id += 1;
  msg.status = nrf.get_status();
  msg.observe = nrf.read(NRF::OBSERVE_TX);
  if (nrf.send(&msg, count) == count) {
    Serial.print(Watchdog::get_ticks());
    Serial_print(":SEND(id = ");
    Serial.print(msg.id);
    Serial_print(", observe = 0b");
    Serial.print(msg.observe, BIN);
    Serial_print(", status = 0b");
    Serial.print(msg.status, BIN);
    Serial_print(")\n");
  }

  // Check if the transmission fifo needs flushing
  if (!nrf.is_ready() && nrf.is_max_retransmit()) {
    nrf.flush();
  }
}
