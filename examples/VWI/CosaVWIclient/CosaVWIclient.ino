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
 * Multiple message types.
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
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

// Select the codec to use for the Virtual Wire Interface. Should be the
// same as in CosaVWIserver.ino
// VirtualWireCodec codec;
// ManchesterCodec codec;
// Block4B5BCodec codec;
BitstuffingCodec codec;

// Network configuration
const uint32_t ADDR = 0xc05a0001UL;
const uint16_t SPEED = 4000;

// Virtual Wire Interface Transceiver
#if defined(__ARDUINO_TINY__)
VWI::Transceiver trx(Board::D0, Board::D1, &codec);
#else
VWI::Transceiver trx(Board::D8, Board::D9, &codec);
#endif

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Measurement message
const uint8_t SAMPLE_CMD = 1;
struct sample_t {
  uint16_t luminance;
  uint16_t temperature;
  
  sample_t(uint16_t lum, uint16_t temp)
  {
    luminance = lum;
    temperature = temp;
  }
};

// Statistics message
const uint8_t STAT_CMD = 2;
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
  // Send message with luminance and temperature
  sample_t measurement(luminance.sample(), temperature.sample());
  int8_t nr = trx.send(&measurement, sizeof(measurement), SAMPLE_CMD);
  statistics.update(nr);
  
  // Send message with battery voltage and statistics every 15 messages
  if (statistics.sent % 15 == 0) {
    statistics.voltage = AnalogPin::bandgap(1100);
    nr = trx.send(&statistics, sizeof(statistics), STAT_CMD);
    statistics.update(nr);
  }

  // Take a nap
  SLEEP(1);
}
