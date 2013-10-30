/**
 * @file CosaWirelessIOStream.ino
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
 * Wireless IOStream demo.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/WIO.hh"

// Select Wireless device driver
// #include "Cosa/Wireless/Driver/CC1101.hh"
// CC1101 rf(0xC05A, 0x02);

#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(0xC05A, 0x02);

// #include "Cosa/Wireless/Driver/VWI.hh"
// #include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
// VirtualWireCodec codec;
// #if defined(__ARDUINO_TINYX5__)
// VWI rf(0xC05A, 0x03, 4000, Board::D1, Board::D0, &codec);
// #else
// VWI rf(0xC05A, 0x02, 4000, Board::D7, Board::D8, &codec);
// #endif

static const uint8_t IOSTREAM_TYPE = 0x00;

WIO wio(&rf, 0x01, IOSTREAM_TYPE);

void setup()
{
  trace.begin(&wio);
  Watchdog::begin();
  RTC::begin();
  rf.begin();
  trace << PSTR("\fWIO: connected") << flush;
  SLEEP(2);
}

void bar(uint16_t value, uint8_t pos, uint16_t max)
{
  uint32_t scale = ((uint32_t) value) * pos;
  uint16_t div = scale / max;
  uint16_t rem = ((scale % max) * 4) / max;
  for (uint8_t i = 0; i < div; i++) trace << (char) 4;
  if (rem != 0) trace << (char) (rem - 1);
}

void loop()
{
  // Print analog pins
  for (uint8_t pin = 0; pin < 8; pin++) {
    uint16_t sample = AnalogPin::sample(pin);
    trace << clear << 'A' << pin << PSTR(": ") << sample << endl;
    bar(sample, 16, 1023);
    trace << flush;
    SLEEP(1);
  }

  // Print bandgap voltage
  uint16_t vcc = AnalogPin::bandgap();
  trace << clear << PSTR("VCC: ") << vcc << PSTR(" mV") << endl;
  bar(vcc, 16, 5500);
  trace << flush;
  SLEEP(2);

  // Print digital pins
  trace << clear << PSTR("D0-7:  "); 
  for (uint8_t pin = 0; pin < 8; pin++) 
    trace << InputPin::read(pin);
  trace << endl;
  trace << PSTR("D8-16: "); 
  for (uint8_t pin = 8; pin < 16; pin++) 
    trace << InputPin::read(pin);
  trace << flush;
  SLEEP(2);

  // Print statistics
#if defined(__COSA_WIRELESS_DRIVER_NRF24L01P_HH__)
  trace << clear;
  trace << PSTR("TN: ") << rf.get_trans() << endl;
  trace << PSTR("ER: ") << rf.get_retrans();
  trace << PSTR(",") << rf.get_drops();
  trace << flush;
  SLEEP(2);
#endif
}
