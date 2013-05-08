/**
 * @file CosaTinyDHT11.ino
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
 * Demonstration of the DHT11 and Virtual Wire Interface (VWI) driver
 * on ATtinyX5. Transmits a simple message with humidity and 
 * temperature readings from DHT11. The messages may be monitored 
 * with the CosaVWIreceiver or CosaTinyReceiver sketch.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino(ATtiny) D2,
 * and DHT11 data with pullup (approx. 5 Kohm) to D1. The LED
 * is on when transmitting.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Driver/DHT11.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Watchdog.hh"

OutputPin led(Board::LED);
DHT11 sensor(Board::D1);
VirtualWireCodec codec;
VWI::Transmitter tx(Board::D2, &codec);
const uint16_t SPEED = 4000;

// Message with DHT11 reading
struct msg_t {
  uint16_t nr;
  int16_t humidity;
  int16_t temperature;
};

void setup()
{
  // Start watchdog and virtual wire interface
  Watchdog::begin();
  VWI::begin(SPEED);
  tx.begin();
}

void loop()
{
  // Read sensor and send message every two seconds
  static uint16_t nr = 0;
  msg_t msg;
  if (!sensor.read(msg.humidity, msg.temperature)) return;
  msg.nr = nr++;
  led.toggle();
  tx.send(&msg, sizeof(msg));
  led.toggle();
  SLEEP(2);
}
