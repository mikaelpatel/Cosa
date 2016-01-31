/**
 * @file CosaWirelessDHT11.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * with pullup (approx. 5 Kohm) to D3/D2. The LED (ATtiny84/85
 * D7/D4) is on when transmitting.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DHT.h>

#include "Cosa/AnalogPin.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

// Configuration; network and device addresses, and sensor pin
#define NETWORK 0xC05A
#if defined(BOARD_ATTINY)
#define DEVICE 0x20
#define EXT Board::EXT0
#define RX Board::D1
#define TX Board::D0
#else
#define DEVICE 0x21
#define EXT Board::EXT1
#define RX Board::D7
#define TX Board::D8
#endif

// Select Wireless device driver
// #include <CC1101.h>
// CC1101 rf(NETWORK, DEVICE);

// #include <NRF24L01P.h>
// NRF24L01P rf(NETWORK, DEVICE);

// #include <RFM69.h>
// RFM69 rf(NETWORK, DEVICE);

#include <VWI.h>
// #include <BitstuffingCodec.h>
// BitstuffingCodec codec;
// #include <Block4B5BCodec.h>
// Block4B5BCodec codec;
// #include <HammingCodec_7_4.h>
// HammingCodec_7_4 codec;
// #include <HammingCodec_8_4.h>
// HammingCodec_8_4 codec;
// #include <ManchesterCodec.h>
// ManchesterCodec codec;
#include <VirtualWireCodec.h>
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI::Transmitter tx(Board::D0, &codec);
#else
VWI::Transmitter tx(Board::D6, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &tx);

struct dht_msg_t {
  uint8_t nr;
  int16_t humidity;
  int16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_HUMIDITY_TEMPERATURE_TYPE = 0x03;

static const uint32_t SLEEP_PERIOD = 5000L;

DHT11 sensor(EXT);

void setup()
{
  // Start timers and the transceiver
  Watchdog::begin();
  RTT::begin();
  rf.begin();

  // Power down transceiver until time to send message
  rf.powerdown();
}

void loop()
{
  static uint8_t nr = 0;
  dht_msg_t msg;

  // Construct message with humidity, temperature and battery reading
  msg.nr = nr++;
  sensor.sample(msg.humidity, msg.temperature);
  msg.battery = AnalogPin::bandgap(1100);

  // Broadcast message and powerdown
  rf.powerup();
  rf.broadcast(DIGITAL_HUMIDITY_TEMPERATURE_TYPE, &msg, sizeof(msg));
  rf.powerdown();

  // Deep sleep with only watchdog awake
  Power::all_disable();
  uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);
  Watchdog::delay(SLEEP_PERIOD);
  Power::set(mode);
  Power::all_enable();
}
