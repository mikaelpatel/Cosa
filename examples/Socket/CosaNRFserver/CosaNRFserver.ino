/**
 * @file CosaNRFserver.ino
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
 * connect and send of messages. Run CosaNRFclient on another
 * Arduino.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Socket.hh"
#include "Cosa/Socket/Driver/NRF24L01P.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

// NRF24L01+ Wireless communication using default pins(SPI, D9, D10, D2)
NRF24L01P nrf(0xc05a0005);

class Monitor : public Server {
private:
  class Demo : public Service {
  public:
    Demo(Socket::Device* dev) : Service(dev) {}
    virtual void on_recv(const void* buf, size_t size);
  };
  Demo m_demo;
  
public:
  Monitor(Socket::Device* dev, uint16_t port) : 
    Server(dev, port),
    m_demo(dev) 
  {}
  virtual Service* on_connect_request(Socket::addr_t& src);
  virtual void on_disconnect_request(Service* service);
};


void
Monitor::Demo::on_recv(const void* buf, size_t size)
{
  trace << PSTR("Demo::on_recv(buf = ") << buf 
	<< PSTR(", size = ") << size << ')' << endl;
  trace.print(buf, size, IOStream::hex);
}

Server::Service* 
Monitor::on_connect_request(Socket::addr_t& src)
{
  trace << PSTR("Monitor::on_connect_request(Socket::addr_t& src = ");
  trace << src << ')' << endl;
  return (&m_demo);
}

void 
Monitor::on_disconnect_request(Service* service)
{
}

Monitor server(&nrf, 6000);

void setup()
{
  // Start UART and bind as trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNRFserver: started"));

  // Start Watchdog timer
  Watchdog::begin();

  // Start NRF24L01P socket device driver and listen for incoming requests
  nrf.begin(true);
  server.listen();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
