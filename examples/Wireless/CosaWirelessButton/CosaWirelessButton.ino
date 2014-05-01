/**
 * @file CosaWirelessButton.ino
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
 * Demonstration of the Wireless interface and ExternalInterruptPin
 * for wakeup after power down. An ATtiny85-20PU running this example
 * sketch will require 5 uA in power down mode (VWI transmitter).
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Event.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

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

class Button : public ExternalInterrupt {
public:
  Button() : ExternalInterrupt(EXT, ON_LOW_LEVEL_MODE, true) {}

  virtual void on_interrupt(uint16_t arg) 
  {
    Event::push(Event::NULL_TYPE, NULL);
    disable();
  }
};

// Wakeup button
Button wakeup;

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

void setup()
{
  // Initiate Watchdog with 512 ms period. Start RTC and Wireless device
  Power::set(SLEEP_MODE_PWR_DOWN);
  Watchdog::begin(512);
  RTC::begin();
  rf.begin();
  
  // Put the hardware in power down
  rf.powerdown();
  Power::all_disable();
  
  // Allow wakeup on button
  wakeup.enable();  
}

struct dlt_msg_t {	       // Digital Luminance Temperature message
  uint32_t timestamp;	       // Seconds since start of sensor
  uint16_t luminance;	       // Light level (0..1023 raw value)
  uint16_t temperature;	       // Room temperature (0..1023 raw value)
  uint16_t battery;	       // Battery level (mV)
};
static const uint8_t DIGITAL_LUMINANCE_TEMPERATURE_TYPE = 0x04;

void loop()
{
  // Wait for events from the button
  Event event;
  Event::queue.await(&event);

  // Wake up the hardware
  Power::all_enable();

  // Construct the message with sample values and broadcast
  uint32_t now = Watchdog::millis();
  dlt_msg_t msg;
  msg.timestamp = (now + 500L) / 1000;
  msg.luminance = luminance.sample();
  msg.temperature = temperature.sample();
  msg.battery = AnalogPin::bandgap();
  rf.broadcast(DIGITAL_LUMINANCE_TEMPERATURE_TYPE, &msg, sizeof(msg));

  // Put the hardware back to sleep
  rf.powerdown();
  Power::all_disable();

  // Debounce the button before allowing further interrupts. This also
  // gives periodic (1 second) message send when the button is kept low.
  Watchdog::delay(1000 - (Watchdog::millis() - now));
  wakeup.enable();  
}
