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
 * Demonstration of the Enhanced Virtual Wire Interface (VWI) driver;
 * Transceiver with acknowledgement and automatic retransmission.
 * Reads date and time from TWI/DS1307 and transmits to server.
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
const uint16_t ADDR = 0xCE51;
const uint16_t SPEED = 4000;

// Virtual Wire Interface Transceiver
VWI::Transceiver trx(Board::D8, Board::D9, &codec);

// Statistics message
const int8_t STAT_CMD = 2;
struct stat_t {
  uint16_t voltage;
  uint16_t sent;
  uint16_t resent;
  uint16_t received;
  uint16_t failed;

  void update(int8_t nr)
  {
    sent += 1;
    if (nr <= 0) 
      failed += 1; 
    else if (nr > 1)
      resent += (nr - 1);
  }
};

// Statistics state
stat_t statistics;
  
// The RTC clock and message type. Note: the timekeeper struct is send
DS1307 rtc;
const int8_t TIMEKEEPER_CMD = 3;
  
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
  // Read date and time and send message with timekeeper struct (BCD)
  DS1307::timekeeper_t now;
  rtc.get_time(now);
  int8_t nr = trx.send(&now, sizeof(now), TIMEKEEPER_CMD);
  statistics.update(nr);
  
  // Send non-acknowledged message with battery voltage and statistics 
  // every 12 messages 
  if (statistics.sent % 12 == 0) {
    statistics.voltage = AnalogPin::bandgap(1100);
    nr = trx.send(&statistics, sizeof(statistics), STAT_CMD, VWI::Transceiver::NACK);
    statistics.update(nr);
  }

  // Take a nap
  SLEEP(5);
}
