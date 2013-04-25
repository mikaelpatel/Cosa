/**
 * @file CosaVWIclient.ino
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
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino D9, RF433/315
 * Receiver to Arduino D8. Connect VCC and GND. Connect Arduino
 * analog pins A2 and A3 to analog sensors.  
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/VWI/Codec/ManchesterCodec.hh"
#include "Cosa/VWI/Codec/BitstuffingCodec.hh"
#include "Cosa/VWI/Codec/Block4B5BCodec.hh"
#include "Cosa/Trace.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIserver.ino
// VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
BitstuffingCodec codec;

// Network configuration
const uint32_t ADDR = 0xC05A0001;
const uint16_t SPEED = 4000;

// Virtual Wire Interface Transmitter and Receiver
#if defined(__ARDUINO_TINYX5__)
VWI::Transceiver trx(Board::D0, Board::D1, &codec);
#else
VWI::Transceiver trx(Board::D8, Board::D9, &codec);
#endif

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Message type to send (should be in an include file for client and server)
const uint8_t SAMPLE_CMD = 1;
struct sample_t {
  uint16_t luminance;
  uint16_t temperature;
};

const uint8_t STAT_CMD = 2;
struct stat_t {
  uint16_t voltage;
  uint16_t sent;
  uint16_t resent;
  uint16_t received;
  uint16_t failed;
};

// Counters
uint16_t sent = 0;
uint16_t resent = 0;
uint16_t received = 0;
uint16_t failed = 0;
  
void update(int8_t nr)
{
  sent += 1;
  if (nr <= 0) 
    failed += 1; 
  else if (nr > 1)
    resent += (nr - 1);
}

void setup()
{
  // Start watchdog for delay
  Watchdog::begin();
  RTC::begin();

  // Start virtual wire interface in extended mode; transceiver
  VWI::begin(ADDR, SPEED);
  trx.begin();
}

void loop()
{
  // Send message with luminance and temperature
  sample_t sample;
  sample.luminance = luminance.sample();
  sample.temperature = temperature.sample();
  int8_t nr = trx.send(&sample, sizeof(sample), SAMPLE_CMD);
  update(nr);
  
  // Send message with battery voltage and statistics every 10 messages
  if (sent % 10 == 0) {
    stat_t stat;
    stat.voltage = AnalogPin::bandgap(1100);
    stat.sent = sent;
    stat.resent = resent;
    stat.received = received;
    stat.failed = failed;
    nr = trx.send(&stat, sizeof(stat), STAT_CMD);
    update(nr);
  }

  // Take a nap
  MSLEEP(200);
}
