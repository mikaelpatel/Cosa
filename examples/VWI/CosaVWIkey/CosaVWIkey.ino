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
 * and ExternalInterruptPin for wakeup after power down.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino/ATtiny D9/D1, 
 * connect VCC and GND. Connect button with pullup resistor to 
 * Arduino EXT0/D2, Mega EXT2/D19, Mighty/D10, TinyX4/D10, and 
 * TinyX5/D2.
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
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Event.hh"
#include "Cosa/Power.hh"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIreceiver.ino
VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
// BitstuffingCodec codec;

// Connect RF433 transmitter to Arduno/ATtiny D9/D1
#if defined(__ARDUINO_TINY__)
VWI::Transmitter tx(Board::D1, &codec);
#else 
VWI::Transmitter tx(Board::D9, &codec);
#endif
const uint16_t SPEED = 4000;

// Connect button with pullup to Arduino Mega EXT2/D19, others to 
// Arduino EXT0; Mighty/D10, TinyX4/D10, Standard/D2 and TinyX5/D2.
#if defined(__ARDUINO_MEGA__)
ExternalInterrupt wakeup(Board::EXT2, ExternalInterrupt::ON_LOW_LEVEL_MODE);
#else
ExternalInterrupt wakeup(Board::EXT0, ExternalInterrupt::ON_LOW_LEVEL_MODE);
#endif

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

void setup()
{
  // Startup Virtual Wire Interface/Transmitter
  VWI::begin(SPEED);
  tx.begin();

  // Put hardware to sleep and allow interrupts from the button
  VWI::disable();
  Power::adc_disable();
#if defined(__ARDUINO_TINY__)
  Power::usi_disable();
#endif
  wakeup.enable();
}

// Message to send
struct msg_t {
  uint16_t nr;
  uint16_t luminance;
  uint16_t temperature;
};

void loop()
{
  // Wait for events from the button
  static uint16_t nr = 0;
  Event event;
  Event::queue.await(&event, SLEEP_MODE_PWR_DOWN);

  // Tune off interrupts from button and wake up the hardware
  wakeup.disable();
  Power::adc_enable();
  VWI::enable();

  // Construct the message, sample the values, and send
  msg_t msg;
  msg.nr = nr++;
  msg.luminance = luminance.sample();
  msg.temperature = temperature.sample();
  tx.send(&msg, sizeof(msg));
  tx.await();

  // Put the hardware back to sleep and allow interrupts from the button
  Power::adc_disable();
  VWI::disable();
  wakeup.enable();
}
