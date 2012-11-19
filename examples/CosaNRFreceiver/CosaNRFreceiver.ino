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
#include "Cosa/Trace.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Memory.h"

// Configuration
// #define USE_TIMEOUT_EVENTS
#define USE_RECEIVE_INTERRUPTS

// NRF24L01+ Wireless communication using SPI and default pins(9, 10, 2)
NRF nrf;

void setup()
{
  // Start trace output
  Trace::begin(9600);

  // Check amount of free memory
  TRACE(free_memory());
  TRACE(sizeof(NRF));

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

  // Print out the configuration
  TRACE(nrf.read(NRF::FEATURE));
  TRACE(nrf.read(NRF::RF_CH));
  TRACE(nrf.read(NRF::RF_SETUP));
  TRACE(nrf.read(NRF::RX_ADDR_P0));
  TRACE(nrf.read(NRF::RX_ADDR_P1));
  TRACE(nrf.read(NRF::RX_ADDR_P2));
  TRACE(nrf.read(NRF::RX_ADDR_P3));
  TRACE(nrf.read(NRF::RX_ADDR_P4));
  TRACE(nrf.read(NRF::RX_ADDR_P5));
  TRACE(nrf.read(NRF::SETUP_RETR));
  TRACE(nrf.read(NRF::SETUP_AW));
  TRACE(nrf.read(NRF::DYNPD));
  TRACE(nrf.read(NRF::CONFIG));

#ifdef USE_RECEIVE_INTERRUPTS
  // Setup interrupt handler to push events on message received
  nrf.set_interrupt(NRF::push_event);
#endif

  // Turn off Arduino timer0 until init is replaced
  TIMSK0 = 0;
}

// Message block
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

  // Print event type
  TRACE(event.get_type());

  // Print receiver and fifo status
  uint16_t ticks = Watchdog::get_ticks();
  uint8_t observe = nrf.read(NRF::OBSERVE_TX);
  uint8_t status = nrf.get_status();
  uint8_t fifo = nrf.read(NRF::FIFO_STATUS);
  Trace::print(ticks);
  Trace::print_P(PSTR(":RECV(observe = "));
  Trace::print(observe, 2);
  Trace::print_P(PSTR(", status = "));
  Trace::print(status, 2);
  Trace::print_P(PSTR(", fifo = "));
  Trace::print(fifo, 2);
  Trace::print_P(PSTR(")\n"));

  // Attempt to receive and print a message
  msg_t msg;
  uint8_t pipe; 
  if (nrf.recv(&msg, &pipe)) {
    Trace::print_P(PSTR("  PIPE("));
    Trace::print(pipe);
    Trace::print_P(PSTR("):msg(id = "));
    Trace::print(msg.id);
    Trace::print_P(PSTR(", observe = "));
    Trace::print(msg.observe, 2);
    Trace::print_P(", status = ");
    Trace::print(msg.status, 2);
    Trace::print_P(PSTR(")\n"));
  }
}
