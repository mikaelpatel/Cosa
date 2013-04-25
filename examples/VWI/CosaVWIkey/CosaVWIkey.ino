/**
 * @file CosaVWIkey.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver
 * and ExternalInterruptPin for wakeup after power down on ATtiny85.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to ATtiny85 D1, connect VCC 
 * GND. Connect button with pullup resistor to D2.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/ManchesterCodec.hh"
#include "Cosa/VWI/Codec/BitstuffingCodec.hh"
#include "Cosa/VWI/Codec/Block4B5BCodec.hh"
#include "Cosa/ExternalInterruptPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/Power.hh"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIreceiver.ino
VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
// BitstuffingCodec codec;

// Connect RF433 transmitter to ATtiny/D1 or Arduino/D9
#if defined(__ARDUINO_TINYX5__)
VWI::Transmitter tx(Board::D1, &codec);
#else 
VWI::Transmitter tx(Board::D9, &codec);
#endif
const uint16_t SPEED = 4000;

// Connect button with pullup to EXT0/D2
ExternalInterruptPin wakeup(Board::EXT0, ExternalInterruptPin::ON_LOW_LEVEL_MODE);

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

void setup()
{
  VWI::begin(SPEED);
  tx.begin();
  VWI::disable();
  Power::adc_disable();
#if defined(__ARDUINO_TINYX5__)
  Power::usi_disable();
#endif
  wakeup.enable();
}

struct msg_t {
  uint16_t nr;
  uint16_t luminance;
  uint16_t temperature;
};

void loop()
{
  static uint16_t nr = 0;
  Event event;
  Event::queue.await(&event, SLEEP_MODE_PWR_DOWN);
  wakeup.disable();
  Power::adc_enable();
  VWI::enable();
  msg_t msg;
  msg.nr = nr++;
  msg.luminance = luminance.sample();
  msg.temperature = temperature.sample();
  tx.send(&msg, sizeof(msg));
  tx.await();
  Power::adc_disable();
  VWI::disable();
  wakeup.enable();
}
