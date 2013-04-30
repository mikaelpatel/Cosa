/**
 * @file CosaVWIsender.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver.
 * Transmits a simple message with two analog samples. 
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino(ATtiny85) D9(D2),
 * VCC to Arduino D10(D1) and connect GND. Connect Arduino analog pins
 * A2 and A3 to analog sensors. On ATtiny85 the pins are D2, D1. 
 * The power control pin D10(D1) can also be used for a LED.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/ManchesterCodec.hh"
#include "Cosa/VWI/Codec/BitstuffingCodec.hh"
#include "Cosa/VWI/Codec/Block4B5BCodec.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIreceiver.ino
VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
// BitstuffingCodec codec;

// Virtual Wire Interface Transmitter and Power Control pins
#if defined(__ARDUINO_TINYX5__)
VWI::Transmitter tx(Board::D2, &codec);
OutputPin pw(Board::D1);
#else 
VWI::Transmitter tx(Board::D9, &codec);
OutputPin pw(Board::D10);
#endif
const uint16_t SPEED = 4000;

void setup()
{
  // Start watchdog for delay
  Watchdog::begin();

  // Start virtual wire interface and transmitter
  VWI::begin(SPEED);
  tx.begin();
}

// Message type to send (header will be automatically be appended)
struct msg_t {
  uint16_t nr;
  uint16_t luminance;
  uint16_t temperature;
};

void loop()
{
  static uint16_t nr = 0;
  msg_t msg;

  // Turn power on. While stabilizing sample the analog values
  pw.on();
  msg.nr = nr++;
  msg.luminance = luminance.sample();
  msg.temperature = temperature.sample();

  // Send the message with the values and wait for completion
  tx.send(&msg, sizeof(msg));
  tx.await();

  // Turn power off.
  Watchdog::delay(128);
  pw.off();

  // Delivery the next message after a 0.5 second delay
  Watchdog::delay(512);
}
