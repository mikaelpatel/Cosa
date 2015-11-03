/**
 * @file CosaWirelessIOStream.ino
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
 * Wireless IOStream demo.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/InputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/WIO.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#if defined(BOARD_ATTINY)
#define DEVICE 0x40
#else
#define DEVICE 0x41
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

static const uint8_t IOSTREAM_TYPE = 0x00;
#define DEST 0x01

WIO wio(&rf, DEST, IOSTREAM_TYPE);

void setup()
{
  trace.begin(&wio);
  Watchdog::begin();
  RTT::begin();
  rf.begin();
  rf.powerup();
  trace << PSTR("WIO: connected") << endl << flush;
  sleep(2);
}

void loop()
{
  // Print analog pins
  trace << PSTR("A0-") << membersof(analog_pin_map) - 1 << PSTR(":  ");
  for (uint8_t ix = 0; ix < membersof(analog_pin_map); ix++) {
    Board::AnalogPin pin;
    pin = (Board::AnalogPin) pgm_read_byte(analog_pin_map + ix);
    uint16_t sample = AnalogPin::sample(pin);
    if (ix > 0) trace << PSTR(", ");
    trace << sample;
  }
  trace << endl;
  trace << flush;
  sleep(2);

  // Print bandgap voltage
  uint16_t vcc = AnalogPin::bandgap();
  trace << PSTR("VCC: ") << vcc << PSTR(" mV");
  trace << endl;
  trace << flush;
  sleep(2);

  // Print digital pins
  trace << PSTR("D0-") << membersof(digital_pin_map) - 1 << PSTR(": ");
  for (uint8_t ix = 0; ix < membersof(digital_pin_map); ix++) {
    Board::DigitalPin pin;
    pin = (Board::DigitalPin) pgm_read_byte(digital_pin_map + ix);
    if (ix > 0) trace << PSTR(", ");
    trace << InputPin::read(pin);
  }
  trace << endl;
  trace << flush;
  sleep(2);

  // Print statistics
#if defined(COSA_WIRELESS_DRIVER_NRF24L01P_HH)
  trace << PSTR("TN: ") << rf.trans() << endl;
  trace << PSTR("ER: ") << rf.retrans();
  trace << PSTR(",") << rf.drops();
  trace << endl;
  trace << flush;
  sleep(2);
#endif
}
