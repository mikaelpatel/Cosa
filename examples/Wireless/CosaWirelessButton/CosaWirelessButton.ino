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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Event.hh"
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

// Connect button between ground and pin TinyX4 EXT0/D10, TinyX5
// EXT0/D2, Mega EXT2/D29 and others to Arduino EXT1 which is
// Standard/D3 and Mighty/D11.
class Button : public ExternalInterrupt {
public:
  Button() : 
#if defined(__ARDUINO_MEGA__)
    ExternalInterrupt(Board::EXT2, ExternalInterrupt::ON_LOW_LEVEL_MODE, true)
#elif defined(__ARDUINO_TINY__)
    ExternalInterrupt(Board::EXT0, ExternalInterrupt::ON_LOW_LEVEL_MODE, true)
#else
    ExternalInterrupt(Board::EXT1, ExternalInterrupt::ON_LOW_LEVEL_MODE, true)
#endif
  {}

  virtual void on_interrupt(uint16_t arg = 0) 
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
  Watchdog::begin(512);
  RTC::begin();
  rf.begin();
  
  // Put the hardware in power down
  AnalogPin::powerdown();
  Power::all_disable();

  // Allow wakeup on button
  wakeup.enable();  
}

// Message to send
struct msg_t {
  uint32_t timestamp;
  uint16_t luminance;
  uint16_t temperature;
  uint16_t battery;
};

void loop()
{
  // Wait for events from the button
  Event event;
  Event::queue.await(&event, SLEEP_MODE_PWR_DOWN);

  // Wake up the hardware
  Power::all_enable();
  AnalogPin::powerup();

  // Construct the message with sample values
  msg_t msg;
  msg.timestamp = RTC::micros();
  msg.luminance = luminance.sample();
  msg.temperature = temperature.sample();
  msg.battery = AnalogPin::bandgap();

  // Broadcast the message and send addressed to node(1)
  rf.broadcast(&msg, sizeof(msg));
  msg.timestamp = RTC::micros();
  rf.send(0x01, &msg, sizeof(msg));

  // Put the hardware back to sleep
  rf.powerdown();
  AnalogPin::powerdown();
  Power::all_disable();

  // Debounce the button before allowing further interrupts. This also
  // gives periodic (1 second) message send when the button is kept low.
  Watchdog::delay(1024);
  wakeup.enable();  
}
