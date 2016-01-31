/**
 * @file CosaWirelessButton.ino
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
 * Demonstration of the Wireless interface and ExternalInterruptPin
 * for wakeup after power down. An ATtiny85-20PU running this example
 * sketch will require 5 uA in power down mode (VWI transmitter),
 * Arduino Pro-Mini with LED removed will require 10 uA with
 * BOD disabled.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"

// Connect button between ground and pin TinyX4 EXT0/D10, TinyX5 EXT0/D2,
// Mega EXT2/D29 and others to Arduino EXT1 which is Standard/D3 and
// Mighty/D11.
#define NETWORK 0xC05A
#if defined(BOARD_ATMEGA2560)
#define DEVICE 0x52
#define EXT Board::EXT2
#elif defined(BOARD_ATTINY)
#define DEVICE 0x50
#define EXT Board::EXT0
#else
#define DEVICE 0x51
#define EXT Board::EXT1
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
#if defined(COSA_VWI_HH)
#define SPEED 4000
#if defined(BOARD_ATTINY)
VWI::Transmitter tx(Board::D0, &codec);
#else
VWI::Transmitter tx(Board::D6, &codec);
#endif
VWI rf(NETWORK, DEVICE, SPEED, &tx);
#endif

class Button : public ExternalInterrupt {
public:
  Button() : ExternalInterrupt(EXT, ON_LOW_LEVEL_MODE, true) {}

  virtual void on_interrupt(uint16_t arg)
  {
    UNUSED(arg);
    disable();
  }
};

// Wakeup button
Button wakeup;

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);
OutputPin led(Board::LED, 0);

// Sample and transmission min period
static const uint32_t MIN_PERIOD = 128;

void setup()
{
  // Start Watchdog, RTT and Wireless device
  Watchdog::begin();
  RTT::begin();
  rf.begin();
}

struct dlt_msg_t {	       // Digital Luminance Temperature message
  uint8_t nr;		       // Message sequence number
  uint16_t luminance;	       // Light level (0..1023 raw value)
  uint16_t temperature;	       // Room temperature (0..1023 raw value)
  uint16_t battery;	       // Battery level (mV)
};
static const uint8_t DIGITAL_LUMINANCE_TEMPERATURE_TYPE = 0x04;

void loop()
{
  // Construct the message with sample values and broadcast
  static uint8_t nr = 0;
  dlt_msg_t msg;
  msg.nr = nr++;
  msg.luminance = luminance.sample();
  msg.temperature = temperature.sample();
  msg.battery = AnalogPin::bandgap();
  led.on();
  rf.powerup();
  rf.broadcast(DIGITAL_LUMINANCE_TEMPERATURE_TYPE, &msg, sizeof(msg));
  rf.powerdown();
  led.off();

  // Put the hardware to sleep
  Power::all_disable();
  uint8_t mode = Power::set(SLEEP_MODE_PWR_DOWN);
  do Watchdog::delay(MIN_PERIOD); while (wakeup.is_clear());
  Watchdog::end();
  RTT::end();
  wakeup.enable();

  // And wait for the wakeup
  while (wakeup.is_set()) yield();

  // Wake up the hardware
  Power::set(mode);
  Watchdog::begin();
  RTT::begin();
  Power::all_enable();
}
