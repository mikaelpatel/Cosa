/**
 * @file CosaWirelessDS18B20.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * @section Description
 * Demonstration sending temperature readings from two 1-Wire DS18B20
 * devices over the Wireless Interface and devices.
 *
 * @section Circuit
 * @code
 *                         RF433/TX
 *                       +------------+
 * (D8/D0)-------------1-|DATA        |
 * (VCC)---------------2-|VCC         |                    V
 * (GND)---------------3-|GND         |                    |
 *                       |ANT       0-|--------------------+
 *                       +------------+       17.3 cm
 *
 *                         DS18B20/2
 *                       +------------++
 * (GND)---------------1-|GND         |\\
 * (OWI/D3)--+---------2-|DQ          | ||
 *           |       +-3-|VDD         |//
 *          4K7      |   +------------++
 *           |       |
 * (PW/D4)---+       +---(VCC/GND)
 *
 * @endcode
 *
 * Connect RF433/315 Transmitter Data to ATtiny85 D0, connect VCC
 * GND. Connect 1-Wire digital thermometer to D3 with pullup resistor.
 * The pullup resistor (4K7) may be connected to D4 to allow active power
 * control. This sketch supports parasite powered DS18B20 devices.
 * Connect the DS18B20 VCC to GND.
 *
 * @section Measurements
 * Arduino Mini Pro 16 Mhz (Power LED removed).
 *  Power	Idle	Sampling	Transmitting
 *  LiPo 3.9 V	40 uA	1.3 mA 		6 mA (RF433).
 *  FTDI 5,1 V	190 uA	1.5 mA		10 mA (RF433).
 * ATtiny85V
 *  LiPo 3.9 V	5 uA	1.3 mA 		5 mA (RF433).
 *
 * @section Note
 * This sketch is designed to also run on an ATtiny85 running on the
 * internal 8 MHz clock.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/RTT.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/AnalogPin.hh"

#include <OWI.h>
#include <DS18B20.h>

// Configuration; network and device addresses
#define NETWORK 0xC05A
#if defined(BOARD_ATTINY)
#define DEVICE 0x30
#else
#define DEVICE 0x31
#endif


// Select Wireless device driver
// #include <CC1101.h>
// CC1101 rf(NETWORK, DEVICE);

// #include <NRF24L01P.h>
// NRF24L01P rf(NETWORK, DEVICE);

// #include <RFM69.h>
// RFM69 rf(NETWORK, DEVICE);

#include <VWI.h>
// #include <BitstuffingCodec.h>
// BitstuffingCodec codec;
// #include <Block4B5BCodec.h>
// Block4B5BCodec codec;
// #include <HammingCodec_7_4.h>
// HammingCodec_7_4 codec;
// #include <HammingCodec_8_4.h>
// HammingCodec_8_4 codec;
// #include <ManchesterCodec.h>
// ManchesterCodec codec;
#include <VirtualWireCodec.h>
VirtualWireCodec codec;
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI::Transmitter tx(Board::D0, &codec);
#else
VWI::Transmitter tx(Board::D6, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &tx);

// Connect to one-wire device; Assuming there are two sensors
OWI owi(Board::D3);
DS18B20 indoors(&owi);
DS18B20 outdoors(&owi);

// Active pullup (pullup resistor 4K7 connected between this pin
// and OWI pin)
OutputPin pw(Board::D4);

// Power-down sleep
#define DEEP_SLEEP(s)					\
  do {							\
    uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);	\
    Watchdog::delay(s * 1000);				\
    Power::set(mode);					\
  } while (0)

void setup()
{
  // Start watchdog and real-time timer
  Watchdog::begin(1024);
  RTT::begin();

  // Start the wireless device
  rf.begin();
  rf.powerdown();

  // Connect to the temperature sensors; use active pullup
  asserted(pw) {
    indoors.connect(0);
    outdoors.connect(1);
  }
}

// Message from the device; temperatures and voltage reading
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

  // Turn on necessary hardware modules
  Power::all_enable();

  // Make a conversion request and read the temperature (scratchpad)
  asserted(pw) {
    DS18B20::convert_request(&owi, 12, true);
    indoors.read_scratchpad();
    outdoors.read_scratchpad();
  }

  // Initiate the message with measurements
  dt_msg_t msg;
  msg.nr = nr++;
  msg.indoors = indoors.temperature();
  msg.outdoors = outdoors.temperature();
  msg.battery = AnalogPin::bandgap(1100);

  // Broadcast the message and power down after completion
  rf.powerup();
  rf.broadcast(DIGITAL_TEMP_TYPE, &msg, sizeof(msg));
  rf.powerdown();

  // Turn off hardware and deep sleep until next sample (period 5 s)
  Power::all_disable();
  DEEP_SLEEP(5);
}

