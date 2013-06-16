/**
 * @file CosaNRFpong.ino
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
 * @section Description
 * Cosa demonstration of NRF24L01+ driver; ping-ping. Should be
 * started after CosaNRFping. Initiates the echoing of messages.
 *
 * @section Circuit
 * NRF24L01+ Wireless communication using default pins(SPI, IRQ=D2,
 * CE=D9, CSN=D10). 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Socket/Driver/NRF24L01P.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

// NRF24L01+ Wireless communication using default pins(SPI, D9, D10, D2)
NRF24L01P nrf(0xc05a0001);

// Echo received message back to sender. Allow startup message and
// update of the message sequence counter.
class Echo : public Socket {
public:
  Echo(Device* dev, uint16_t port) : Socket(dev, port) {}
  virtual void on_recv(const void* buf, size_t size, const addr_t& src);
  void begin();
};

void 
Echo::on_recv(const void* buf, size_t size, const addr_t& src)
{
  uint32_t* nr = (uint32_t*) buf;
  trace << get_address() << ':' << src << ':' << *nr << endl;
  *nr += 1;
  Watchdog::delay(200);
  send(buf, size, src);
}

void
Echo::begin()
{
  NRF24L01P* nrf = (NRF24L01P*) m_dev;
  nrf->begin();
  Socket::addr_t dest = { 0xc05a0002, 8000 };
  uint32_t msg = 0L;
  send(&msg, sizeof(msg), dest);
  Watchdog::delay(20);
  nrf->set_receiver_mode();
}

// Echo pong services datagram port(8000)
Echo pong(&nrf, 8000);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNRFpong: started"));
  Watchdog::begin();
  pong.begin();
}

void loop()
{
  // Wait for the next event. Go to low power during the wait.
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
