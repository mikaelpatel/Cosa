/**
 * @file CosaVWItempsensor.ino
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
 * Demonstration sending temperature readings from a OneWire DS18B20
 * device over the Virtual Wire Interface (VWI). 
 *
 * @section Note
 * This sketch is designed for an ATtiny85 running on the internal 
 * 8 MHz clock. Receive measurements with the CosaVWItempmonitor sketch.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to ATtiny85 D1, connect VCC 
 * GND. Connect 1-Wire digital thermometer to D2 with pullup resistor.
 * For low voltage (3.3 V) the pullup resistor must be approx. 1 Kohm.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/FixedPoint.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Power.hh"

// Connect to one-wire device. Assuming only one device even if connected
OWI owi(Board::D2);
DS18B20 sensor(&owi);

// Connect RF433 transmitter to ATtiny/D1
VirtualWireCodec codec;
VWI::Transmitter tx(Board::D1, &codec);
const uint16_t SPEED = 4000;

void setup()
{
  // Set up watchdog for power down sleep
  Watchdog::begin(1024, SLEEP_MODE_PWR_DOWN);

  // Start the Virtual Wire Interface/Transmitter
  VWI::begin(SPEED);
  tx.begin();

  // Connect to the temperature sensor and give some time for startup
  sensor.connect(0);
  SLEEP(1);

  // Disable hardware
  VWI::disable();
  Power::adc_disable();
  Power::usi_disable();
  Power::timer0_disable();
  Power::timer1_disable();
}

// Message from the device. Use one-wire identity as virtual wire identity
struct msg_t {
  uint8_t id[OWI::ROM_MAX];
  uint16_t nr;
  int16_t temperature;
  uint16_t voltage;
};

void loop()
{
  static uint16_t nr = 0;
  msg_t msg;

  // Make a conversion request
  sensor.convert_request();
  SLEEP(1);

  // Turn on necessary hardware modules
  Power::timer1_enable();
  Power::adc_enable();

  // Read the temperature and initiate the message
  sensor.read_scratchpad();
  memcpy(&msg.id, sensor.get_rom(), sizeof(msg.id));
  msg.nr = nr++;
  msg.temperature = sensor.get_temperature();
  msg.voltage = AnalogPin::bandgap(1100);

  // Enable wireless transmitter and send. Wait completion and disable
  VWI::enable();
  tx.send(&msg, sizeof(msg));
  tx.await();
  VWI::disable();

  // Turn off hardware and sleep until next sample (period 5 s)
  Power::timer1_disable();
  Power::adc_disable();
  SLEEP(4);
}
