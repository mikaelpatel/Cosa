/**
 * @file CosaWirelessDHT11.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * @section Description
 * Demonstration of the DHT11 and Wireless Interface device drivers.
 * Broadcasts a simple message with humidity and temperature readings
 * from DHT11. The messages may be monitored with the
 * CosaWirelessReceiver or the CosaWirelessDebug sketch.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections. Connect DHT11 data 
 * with pullup (approx. 5 Kohm) to D7/D10/D2. The LED (ATtiny84/85
 * D4/D5) is on when transmitting .
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Driver/DHT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Configuration; network and device addresses, and sensor pin
#define NETWORK 0xC05A
#if defined(BOARD_ATTINY)
#define DEVICE 0x20
#define EXT Board::EXT0
#else
#define DEVICE 0x21
#define EXT Board::EXT1
#endif

// Select Wireless device driver
#define USE_CC1101
// #define USE_NRF24L01P
// #define USE_VWI

#if defined(USE_CC1101)
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(NETWORK, DEVICE);

#elif defined(USE_NRF24L01P)
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(NETWORK, DEVICE);

#elif defined(USE_VWI)
#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI rf(NETWORK, DEVICE, SPEED, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, SPEED, Board::D7, Board::D8, &codec);
#endif
#endif

OutputPin led(Board::LED);
DHT11 sensor(EXT);

struct dht_msg_t {
  uint8_t nr;
  int16_t humidity;
  int16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_HUMIDITY_TEMPERATURE_TYPE = 0x03;

void setup()
{
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

void loop()
{
  static uint8_t nr = 0;
  asserted(led) {
    dht_msg_t msg;
    msg.nr = nr++;
    sensor.sample(msg.humidity, msg.temperature);
    msg.battery = AnalogPin::bandgap(1100);
    rf.broadcast(DIGITAL_HUMIDITY_TEMPERATURE_TYPE, &msg, sizeof(msg));
    rf.powerdown();
  }
  SLEEP(2);
}
