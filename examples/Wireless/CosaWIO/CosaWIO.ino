/**
 * @file CosaWIO.ino
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
  trace << PSTR("\fWIO: connected");
  wio.flush();
  SLEEP(2);
}

void loop()
{
  // Print analog pins and bandgap voltage
  trace << clear;
  trace << PSTR("A0-1: ") << AnalogPin::sample(0) 
	<< PSTR(", ") << AnalogPin::sample(1) 
	<< endl;
  trace << PSTR("VCC = ") << AnalogPin::bandgap() << PSTR(" mV");
  wio.flush();
  SLEEP(2);

  // Print digital pins
  trace << clear;
  trace << PSTR("D0-7:  "); 
  for (uint8_t pin = 0; pin < 8; pin++) 
    trace << InputPin::read(pin);
  trace << endl;
  trace << PSTR("D8-16: "); 
  for (uint8_t pin = 8; pin < 16; pin++) 
    trace << InputPin::read(pin);
  wio.flush();
  SLEEP(2);
}
