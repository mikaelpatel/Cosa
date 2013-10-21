/**
 * @file CosaWirelessDHT11.ino
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
 * Demonstration of the DHT11 and Wireless Interface device drivers.
 * Broadcasts a simple message with humidity and temperature readings
 * from DHT11. The messages may be monitored with the
 * CosaWirelessReceiver or the CosaWirelessDebug sketch.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino/ATtiny84/85 D9/D3/D3,
 * and DHT11 data with pullup (approx. 5 Kohm) to D7/D10/D2. 
 * The LED (ATtiny84/85 D4/D5) is on when transmitting.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Driver/DHT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

#if !defined(__ARDUINO_TINY__)
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Trace.hh"
#endif

// Select Wireless device driver
// #include "Cosa/Wireless/Driver/CC1101.hh"
// CC1101 rf(0xC05A, 0x02);

// #include "Cosa/Wireless/Driver/NRF24L01P.hh"
// NRF24L01P rf(0xC05A, 0x02);

#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
VirtualWireCodec codec;
#if defined(__ARDUINO_TINY__)
VWI rf(0xC05A, 0x04, 4000, Board::D1, Board::D0, &codec);
DHT11 sensor(Board::EXT0);
#else
VWI rf(0xC05A, 0x02, 4000, Board::D7, Board::D8, &codec);
DHT11 sensor(Board::EXT1);
#endif

OutputPin led(Board::LED);

struct dht_msg_t {
  uint8_t nr;
  int16_t humidity;
  int16_t temperature;
  uint16_t battery;
};
static const uint8_t DIGITAL_HUMIDITY_TEMPERATURE_TYPE = 0x02;

void setup()
{
#if !defined(__ARDUINO_TINY__)
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaWirelessDHT11: started"));
#endif
  Watchdog::begin();
  RTC::begin();
  rf.begin();
}

void loop()
{
  static dht_msg_t msg = { 0 };
  asserted(led) {
    sensor.sample(msg.humidity, msg.temperature);
    msg.battery = AnalogPin::bandgap(1100);
    rf.broadcast(DIGITAL_HUMIDITY_TEMPERATURE_TYPE, &msg, sizeof(msg));
    rf.powerdown();
    msg.nr += 1;
  }
  SLEEP(2);
}
