/**
 * @file CosaNRFmonitor.ino
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
 * Cosa demonstration of NRF24L01+ driver; monitor sensors.
 *
 * @section Circuit
 * NRF24L01+ Wireless communication using default pins(SPI, IRQ=D2,
 * CE=D9, CSN=D10). 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Socket/Driver/NRF24L01P.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/OWI.hh"
#include "Cosa/FixedPoint.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"

// NRF24L01+ Wireless communication using default pins(SPI, D9, D10, D2)
NRF24L01P nrf(0xc05a0002);

// Abstract sensor reading receiver class
class Sensor : public Socket {
private:
  virtual void on_recv(const void* buf, size_t size, const addr_t& src);
  const size_t SIZE;
public:
  Sensor(Device* dev, uint16_t port, size_t size) : 
    Socket(dev, port),
    SIZE(size)
  {}
  virtual void print(IOStream& outs, const void* buf);
};

void 
Sensor::on_recv(const void* buf, size_t size, const addr_t& src)
{
  if (size != SIZE) return;
  trace << src << ':' << get_address() << ':';
  print(trace, buf);
  trace << endl;
}

// Luminance and Temperature sensor reading receiver on port(7000)
class LTB : public Sensor {
public:
  struct msg_t {
    uint16_t nr;
    uint16_t luminance;
    uint16_t temperature;
    uint16_t voltage;
  };
  LTB(Device* dev, uint16_t port) : Sensor(dev, port, sizeof(msg_t)) {}
  virtual void print(IOStream& outs, const void* buf);
};
LTB ltb(&nrf, 7000);
  
void 
LTB::print(IOStream& outs, const void* buf)
{
  msg_t* msg = (msg_t*) buf;
  outs << PSTR("LTB:") << msg->nr << ':'
       << PSTR("luminance = ") << msg->luminance << ':'
       << PSTR("temperature = ") << msg->temperature << ':'
       << PSTR("voltage = ") << msg->voltage;
}

// Humidity and Temperature sensor reading receiver on port(7001)
class HTB : public Sensor {
public:
  struct msg_t {
    uint16_t nr;
    uint16_t humidity;
    uint16_t temperature;
    uint16_t voltage;
  };
  HTB(Device* dev, uint16_t port) : Sensor(dev, port, sizeof(msg_t)) {}
  virtual void print(IOStream& outs, const void* buf);
};
HTB dht(&nrf, 7001);

void 
HTB::print(IOStream& outs, const void* buf)
{
  msg_t* msg = (msg_t*) buf;
  outs << PSTR("HTB:") << msg->nr << ':'
       << PSTR("humidity = ") << msg->humidity << ':'
       << PSTR("temperature = ") << msg->temperature << ':'
       << PSTR("voltage = ") << msg->voltage;
}

// OneWire temperature reading receiver on port(7002)
class OWT : public Sensor {
public:
  struct msg_t {
    uint16_t nr;
    uint8_t id[OWI::ROM_MAX];
    uint16_t temperature;
    uint16_t voltage;
  };
  OWT(Device* dev, uint16_t port) : Sensor(dev, port, sizeof(msg_t)) {}
  virtual void print(IOStream& outs, const void* buf);
  void print(IOStream& outs, uint8_t x);
};
OWT owt(&nrf, 7002);

void 
OWT::print(IOStream& outs, uint8_t x)
{
  char c;
  c = (x >> 4);
  c += ((c > 9) ? 'a'- 9 : '0');
  outs << c;
  c = (x & 0x0f);
  c += ((c > 9) ? 'a' - 9 : '0');
  outs << c;
}

void 
OWT::print(IOStream& outs, const void* buf)
{
  msg_t* msg = (msg_t*) buf;
  uint8_t i;
  outs << PSTR("OWI:") << msg->nr << ':';
  for (i = 0; i < OWI::ROM_MAX; i++) print(outs, msg->id[i]);
  outs << PSTR(":temperature = ");
  FixedPoint temperature(msg->temperature, 4);
  uint16_t fraction = temperature.get_fraction(2);
  outs << temperature.get_integer() << '.';
  if (fraction < 10) outs << '0';
  outs << fraction;
  outs << PSTR(":voltage = ") << msg->voltage;
}

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNRFmonitor: started"));
  Watchdog::begin();
  nrf.begin(true);
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}
