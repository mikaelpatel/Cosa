/**
 * @file CosaNRFsensors.ino
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
 * Cosa demonstration of NRF24L01+ driver; read and send sensor
 * data to monitor using sockets. Supports reading of analog pins,
 * DHT11 and 1-Wire DS18B20 sensors.
 *
 * @section Circuit
 * NRF24L01+ Wireless communication using default pins(SPI, IRQ=D2,
 * CE=D9, CSN=D10). Luminance and temperature sensor based on analog
 * pins(A2, A3). Digital humidity and temperture sensor on pin(D7) and
 * OneWire temperature sensor on pin(D8).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Socket/Driver/NRF24L01P.hh"
#include "Cosa/Watchdog.hh"

// NRF24L01+ Wireless communication using default pins(SPI, D9, D10, D2)
NRF24L01P nrf(0xc05a0001);

#ifndef __ARDUINO_TINY__
// Luminance and temperature sensor based on analog pins(A2, A3)
#include "Cosa/Pins.hh"
namespace LTB {
  const Socket::addr_t dest = { 0xc05a0002, 7000 };
  Socket socket(&nrf, 6000);
  AnalogPin luminance(Board::A2);
  AnalogPin temperature(Board::A3);
  uint16_t nr = 0;

  struct msg_t {
    uint16_t nr;
    uint16_t luminance;
    uint16_t temperature;
    uint16_t voltage;
  };

  void send_update()
  {
    msg_t msg;
    msg.nr = nr++;
    msg.luminance = luminance.sample();
    msg.temperature = temperature.sample();
    msg.voltage = AnalogPin::bandgap(1100);
    socket.send(&msg, sizeof(msg), dest);
  }
};
#endif

// Digital humidity and temperture sensor on pin(D3)
#include "Cosa/Driver/DHT.hh"
namespace HTB {
  const Socket::addr_t dest = { 0xc05a0002, 7001 };
  DHT11 indoors(Board::EXT1);
  Socket socket(&nrf, 6001);
  uint16_t nr = 0;

  struct msg_t {
    uint16_t nr;
    int16_t humidity;
    int16_t temperature;
    uint16_t voltage;
  };

  void send_update()
  {
    msg_t msg;
    if (!indoors.read(msg.humidity, msg.temperature)) return;
    msg.nr = nr++;
    msg.voltage = AnalogPin::bandgap(1100);
    socket.send(&msg, sizeof(msg), dest);
  }
};

#ifndef __ARDUINO_TINY__
// OneWire temperature sensor on pin(D8)
#include "Cosa/OWI/Driver/DS18B20.hh"
namespace OWT {
  const Socket::addr_t dest = { 0xc05a0002, 7002 };
  uint16_t nr = 0;
  OWI owi(Board::D8);
  DS18B20 outdoors(&owi);
  Socket socket(&nrf, 6002);

  struct msg_t {
    uint16_t nr;
    uint8_t id[OWI::ROM_MAX];
    int16_t temperature;
    uint16_t voltage;
  };

  void begin()
  {
    outdoors.connect(0);
  }

  void send_update()
  {
    msg_t msg;
    outdoors.convert_request();
    outdoors.read_scratchpad();
    msg.nr = nr++;
    memcpy(msg.id, outdoors.get_rom(), sizeof(msg.id));
    msg.temperature = outdoors.get_temperature();
    msg.voltage = AnalogPin::bandgap(1100);
    socket.send(&msg, sizeof(msg), dest);
  }
};
#endif

#ifndef __ARDUINO_TINY__
// The Arduino Standard version: All sensors (analog pin, DHT11 and DS18B20)

void setup()
{
  Watchdog::begin();
  nrf.begin();
  OWT::begin();
}

void loop()
{
  LTB::send_update();
  HTB::send_update();
  OWT::send_update();
  SLEEP(2);
}

#else
// The Arduino Tiny version: DHT11 sensor only

void setup()
{
  Watchdog::begin();
  nrf.begin();
}

void loop()
{
  HTB::send_update();
  SLEEP(2);
}
#endif
