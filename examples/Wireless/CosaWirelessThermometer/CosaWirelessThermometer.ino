/**
 * @file CosaWirelessThermometer.ino
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
 * Demonstration sending temperature readings from 1-Wire DS18B20
 * devices over the Wireless Interface and devices. 
 *
 * @section Note
 * This sketch is designed to run on an ATtiny85 running on the
 * internal 8 MHz clock. 
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to ATtiny85 D0, connect VCC 
 * GND. Connect 1-Wire digital thermometer to D3 with pullup resistor.
 * The pullup resistor (4K7) may be connected to D4 to allow power
 * control. This sketch supports parasite powered DS18B20 devices.
 * Connect the DS18B20 VCC to GND. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/OWI.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Select Wireless device driver (network = 0xC05A, device = 0x02)
// #include "Cosa/Wireless/Driver/CC1101.hh"
// CC1101 rf(0xC05A, 0x02);

// #include "Cosa/Wireless/Driver/NRF24L01P.hh"
// NRF24L01P rf(0xC05A, 0x02);

#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
VirtualWireCodec codec;
#if defined(__ARDUINO_TINYX5__)
VWI rf(0xC05A, 0x03, 4000, Board::D1, Board::D0, &codec);
#else
VWI rf(0xC05A, 0x02, 4000, Board::D7, Board::D8, &codec);
#endif

// Connect to one-wire device; Assuming there are two sensors
OWI owi(Board::D3);
DS18B20 indoors(&owi);
DS18B20 outdoors(&owi);

// Active pullup
OutputPin pw(Board::D4);

void setup()
{
  // Set up watchdog for power down sleep mode
  Watchdog::begin(128, SLEEP_MODE_PWR_DOWN);
  RTC::begin();

  // Start the wireless device and powerdown
  rf.begin();
  rf.powerdown();

  // Connect to the temperature sensor
  pw.on();
  indoors.connect(0);
  outdoors.connect(1);
  pw.off();
  
  // Put the hardware in power down
  AnalogPin::powerdown();
  Power::all_disable();
}

// Message from the device; temperature and voltage reading
struct sample_t {
  uint16_t nr;
  int16_t indoors;
  int16_t outdoors;
  uint16_t voltage;
};

void loop()
{
  static sample_t msg = { 0xffff };
  
  // Make a conversion request and read the temperature (scratchpad)
  pw.on();
  DS18B20::convert_request(&owi, 12, true);
  indoors.read_scratchpad();
  outdoors.read_scratchpad();
  pw.off();
  
  // Turn on necessary hardware modules
  Power::timer0_enable();
  Power::timer1_enable();
  Power::adc_enable();
  AnalogPin::powerup();

  // Initiate the message with measurements
  msg.nr += 1;
  msg.indoors = indoors.get_temperature();
  msg.outdoors = outdoors.get_temperature();
  msg.voltage = AnalogPin::bandgap(1100);

  // Broadcast the message and power down after completion
  rf.broadcast(&msg, sizeof(msg));
  rf.powerdown();

  // Turn off hardware and deep sleep until next sample (period 2 s)
  AnalogPin::powerdown();
  Power::adc_disable();
  Power::timer1_disable();
  Power::timer0_disable();
  SLEEP(2);
}
