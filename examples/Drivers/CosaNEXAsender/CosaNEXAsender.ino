/**
 * @file CosaNEXAsender.ino
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
 * Simple sketch to demonstrate sending Nexa Home Wireless Switch
 * Remote codes. First command sent will be used as the device 
 * identity. Will blink the receiver and transmitter leds.
 * See CosaNEXAreceiver.ino.
 *
 * @section Circuit
 * Connect Arduino(ATtinyX5) D9(D2) to RF433 Transmitter data pin.
 * 
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Driver/NEXA.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

// Enable sending of dimmer level (-1..-15)
// #define DIMMER

OutputPin led(Board::LED);
#if defined(__ARDUINO_TINY__)
NEXA::Transmitter transmitter(Board::D2, 0xc05a01L);
#else
NEXA::Transmitter transmitter(Board::D9, 0xc05a01L);
#endif

void setup()
{
  // Initiate Real-time clock and Watchdog
  RTC::begin();
  Watchdog::begin();

  // First code will be used by receiver as address (learning mode)
  transmitter.send(0, 1);
}

void loop()
{
  // Blink the transmitter and receiver leds
  led.on();
#if defined(DIMMER)
  for (int8_t level = -1; level > -16; level--) {
    transmitter.send(0, level);
    SLEEP(3);
  }
#else
  transmitter.send(0, 1);
  SLEEP(1);
#endif
  transmitter.send(0, 0);
  led.off();
  SLEEP(5);
}
