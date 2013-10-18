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
 * for wakeup after power down. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Event.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Select Wireless device driver
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
VWI rf(0xC05A, 0x03, 4000, Board::D7, Board::D8, &codec);
#endif

// Connect button with pullup to Arduino Mega EXT2/D19, others to 
// Arduino EXT0; Mighty/D10, TinyX4/D10, Standard/D2 and TinyX5/D2.
class Button : public ExternalInterrupt {
public:
  Button() : 
    ExternalInterrupt(
#if defined(__ARDUINO_MEGA__)
		      Board::EXT2, 
#elif defined(__ARDUINO_TINY__)
		      Board::EXT0, 
#else
		      Board::EXT1, 
#endif
		      ExternalInterrupt::ON_LOW_LEVEL_MODE,
		      true)
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
  Watchdog::begin(512);
  RTC::begin();
  rf.begin();
  AnalogPin::powerdown();
  Power::all_disable();
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

  // Broadcast but also send addressed to node(1)
  rf.broadcast(&msg, sizeof(msg));
  rf.send(0x01, &msg, sizeof(msg));

  // Put the hardware back to sleep
  rf.powerdown();
  AnalogPin::powerdown();
  Power::all_disable();

  // Debounce the button before allowing further interrupts. This also
  // give a periodic message send when the button is kept low 
  Watchdog::delay(1024);
  wakeup.enable();  
}
