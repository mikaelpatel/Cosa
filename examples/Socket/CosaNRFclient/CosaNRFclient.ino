/**
 * @file CosaNRFclient.ino
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
 * Cosa demonstration of NRF24L01+ driver; client-server socket
 * connect and send of messages. Run CosaNRFserver on another
 * Arduino.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Socket.hh"
#include "Cosa/Socket/Driver/NRF24L01P.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Periodic.hh"

// NRF24L01+ Wireless communication using default pins(SPI, D9, D10, D2)
NRF24L01P nrf(0xc05a0004);

// Demonstrate client socket connection and periodic sending of message
class Demo : public Client, public Periodic {
public:
  Demo(Socket::Device* dev, uint16_t ms) : Client(dev), Periodic(ms) {}
  virtual void on_connected();
  virtual void on_recv(const void* buf, size_t size);
  virtual void run();
};

void 
Demo::on_connected()
{
  trace << PSTR("Demo::on_connected()") << endl;
}

void
Demo::on_recv(const void* buf, size_t size)
{
  trace << PSTR("Demo::on_recv(buf = ") << buf 
	<< PSTR(", size = ") << size << ')' << endl;
  trace.print(buf, size);
}

void
Demo::run()
{
  // Wait for a connection
  if (!is_connected()) return;

  // Send a message of max payload size to server
  uint8_t msg[NRF24L01P::PAYLOAD_MAX];
  static uint8_t nr = 0;
  for (uint8_t i = 0; i < sizeof(msg); i++)
    msg[i] = nr + i;
  trace << PSTR("Demo::run()::");
  TRACE(send(&msg, sizeof(msg)));
  nr += 1;
}

Demo client(&nrf, 1024);

void setup()
{
  // Start UART and bind as trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNRFclient: started"));

  // Start watchdog and push timeout events to periodic functions
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);

  // Start NRF24L01P device driver and connect client socket to server
  nrf.begin();
  const Socket::addr_t SERVER = { 0xc05a0005, 6000 };
  TRACE(client.connect(SERVER));
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
