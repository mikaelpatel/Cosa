/**
 * @file CosaNRFreceiver.ino
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
 * Cosa demonstration of NRF24L01+ driver; receiver
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/NRF.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Memory.h"

// Configuration
// #define USE_TIMEOUT_EVENTS
#define USE_RECEIVE_INTERRUPTS

// NRF24L01+ Wireless communication using SPI and default pins(9, 10, 2)
NRF nrf;

void setup()
{
  // Start serial interface for trace output
  Serial.begin(9600);

  // Check amount of free memory
  Serial_trace(free_memory());
  Serial_trace(sizeof(NRF));

  // Start the watchdog ticks counter and push timeout events
#ifdef USE_TIMEOUT_EVENTS
  Watchdog::begin(1024, Watchdog::push_event);
#endif
#ifdef USE_RECEIVE_INTERRUPTS
  Watchdog::begin(1024);
#endif

  // Powerup the transceiver and select receiver mode.
  nrf.set_powerup_mode();
  nrf.set_receiver_mode("cosa1");
  Serial_trace(nrf.read(NRF::FEATURE));
  Serial_trace(nrf.read(NRF::RF_CH));
  Serial_trace(nrf.read(NRF::RF_SETUP));
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

#ifdef USE_RECEIVE_INTERRUPTS
  // Setup interrupt handler to push events on message received
  nrf.set_interrupt(NRF::push_event);
#endif

  // Turn off Arduino timer0
  TIMSK0 = 0;
}

typedef struct msg_t msg_t;
struct msg_t {
  uint16_t id;
  uint8_t observe;
  uint8_t status;
};

void loop()
{
  // Wait for the next event. Go to low power during the wait.
  Event event;
  Event::queue.await(&event);
  Serial_trace(event.get_type());

  // Print transceiver and fifo status
  uint16_t ticks = Watchdog::get_ticks();
  uint8_t observe = nrf.read(NRF::OBSERVE_TX);
  uint8_t status = nrf.get_status();
  uint8_t fifo = nrf.read(NRF::FIFO_STATUS);
  Serial.print(ticks);
  Serial_print(":RECV(observe = 0b");
  Serial.print(observe, BIN);
  Serial_print(", status = 0b");
  Serial.print(status, BIN);
  Serial_print(", fifo = 0b");
  Serial.print(fifo, BIN);
  Serial_print(")\n");

  // Attempt to receive messages.
  msg_t msg;
  uint8_t pipe; 
  if (nrf.recv(&msg, &pipe)) {
    Serial_print("  PIPE(");
    Serial.print(pipe);
    Serial_print("):msg(id = ");
    Serial.print(msg.id);
    Serial_print(", observe = 0b");
    Serial.print(msg.observe, BIN);
    Serial_print(", status = 0b");
    Serial.print(msg.status, BIN);
    Serial_print(")\n");
  }
}
