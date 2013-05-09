/**
 * @file CosaVWIclock.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver;
 * Transceiver with acknowledgement and automatic retransmission.
 * Reads date and time from DS1307 and transmits to server.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino D9, RF433/315
 * Receiver to Arduino D8. Connect VCC and GND. Connect DS1307
 * to TWI bus.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/DS1307.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/ManchesterCodec.hh"
#include "Cosa/VWI/Codec/BitstuffingCodec.hh"
#include "Cosa/VWI/Codec/Block4B5BCodec.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIserver.ino
VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
// BitstuffingCodec codec;

// Network configuration
const uint32_t ADDR = 0xc05a0003UL;
const uint16_t SPEED = 4000;

// Virtual Wire Interface Transceiver
VWI::Transceiver trx(Board::D8, Board::D9, &codec);

DS1307 rtc;
const uint8_t TIMEKEEPER_CMD = 3;
  
void setup()
{
  // Start watchdog for delay and RTC for time measurement
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire interface in enhanced mode and use the transceiver
  VWI::begin(ADDR, SPEED);
  trx.begin();
}

void loop()
{
  // Read date and time and send message
  DS1307::timekeeper_t now;
  rtc.get_time(now);
  trx.send(&now, sizeof(now), TIMEKEEPER_CMD);

  // Take a nap
  SLEEP(5);
}
