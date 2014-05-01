/**
 * @file CosaWirelessDS18B20.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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
 * Demonstration sending temperature readings from two 1-Wire DS18B20
 * devices over the Wireless Interface and devices. 
 *
 * @section Circuit
 *                         RF433/TX
 *                       +------------+
 * (D0)----------------1-|DATA        |
 * (VCC)---------------2-|VCC         |                    V
 * (GND)---------------3-|GND         |                    |
 *                       |ANT       0-|--------------------+
 *                       +------------+       17.3 cm
 *
 *                           DS18B20
 *                       +------------+
 * (GND)---------------1-|GND         |\
 * (D3)------+---------2-|DQ          | |
 *           |       +-3-|VDD         |/
 *          4K7      |   +------------+
 *           |       | 
 * (D4)------+       +---(VCC/GND)
 *
 * Connect RF433/315 Transmitter Data to ATtiny85 D0, connect VCC 
 * GND. Connect 1-Wire digital thermometer to D3 with pullup resistor.
 * The pullup resistor (4K7) may be connected to D4 to allow active power
 * control. This sketch supports parasite powered DS18B20 devices.
 * Connect the DS18B20 VCC to GND. 
 * 
 * @section Measurements
 * Arduino Mini Pro 16 Mhz (Power LED removed).
 * Power	Idle	Sampling	Transmitting	
 * LiPo 3.9 V	40 uA	1.3 mA 		6 mA (RF433).
 * FTDI 5,1 V	190 uA	1.5 mA		10 mA (RF433).
 *
 * @section Note
 * This sketch is designed to also run on an ATtiny85 running on the
 * internal 8 MHz clock. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/OWI.hh"
#include "Cosa/OWI/Driver/DS18B20.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Configuration; network and device addresses
#define NETWORK 0xC05A
#if defined(BOARD_ATTINY)
#define DEVICE 0x30
#else
#define DEVICE 0x31
#endif

// Select Wireless device driver
#define USE_CC1101
// #define USE_NRF24L01P
// #define USE_VWI

#if defined(USE_CC1101)
#include "Cosa/Wireless/Driver/CC1101.hh"
CC1101 rf(NETWORK, DEVICE);

#elif defined(USE_NRF24L01P)
#include "Cosa/Wireless/Driver/NRF24L01P.hh"
NRF24L01P rf(NETWORK, DEVICE);

#elif defined(USE_VWI)
#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/Wireless/Driver/VWI/Codec/VirtualWireCodec.hh"
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI rf(NETWORK, DEVICE, SPEED, Board::D1, Board::D0, &codec);
#else
VWI rf(NETWORK, DEVICE, SPEED, Board::D7, Board::D8, &codec);
#endif
#endif

// Connect to one-wire device; Assuming there are two sensors
OWI owi(Board::D3);
DS18B20 indoors(&owi);
DS18B20 outdoors(&owi);

// Active pullup (pullup resistor connected to this pin)
OutputPin pw(Board::D4);

void setup()
{
  // Set up watchdog for power down sleep mode
  Power::set(SLEEP_MODE_PWR_DOWN);
  Watchdog::begin(128);
  RTC::begin();

  // Start the wireless device and powerdown
  rf.begin();
  rf.powerdown();

  // Connect to the temperature sensors
  pw.on();
  indoors.connect(0);
  outdoors.connect(1);
  pw.off();
  
  // Put the hardware in power down
  Power::all_disable();
}

// Message from the device; temperature and voltage reading
struct dt_msg_t {
  uint8_t nr;
  int16_t indoors;
  int16_t outdoors;
  uint16_t battery;
};
static const uint8_t DIGITAL_TEMP_TYPE = 0x02;

void loop()
{
  // Message sequence number initialization
  static uint8_t nr = 0;

  // Make a conversion request and read the temperature (scratchpad)
  pw.on();
  DS18B20::convert_request(&owi, 12, true);
  indoors.read_scratchpad();
  outdoors.read_scratchpad();
  pw.off();
  
  // Turn on necessary hardware modules
  Power::all_enable();

  // Initiate the message with measurements
  dt_msg_t msg;
  msg.nr = nr++;
  msg.indoors = indoors.get_temperature();
  msg.outdoors = outdoors.get_temperature();
  msg.battery = AnalogPin::bandgap(1100);

  // Broadcast the message and power down after completion
  rf.broadcast(DIGITAL_TEMP_TYPE, &msg, sizeof(msg));
  rf.powerdown();

  // Turn off hardware and deep sleep until next sample (period 5 s)
  Power::all_disable();
  SLEEP(5);
}

