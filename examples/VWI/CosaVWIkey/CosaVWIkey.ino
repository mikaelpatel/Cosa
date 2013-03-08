/**
 * @file CosaVWIkey.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver
 * and InterruptPin for wakeup after power down on ATtiny85.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to ATtiny85 D1, connect VCC 
 * GND. Connect button with pullup resistor to D2.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/VWI/Codec/VirtualWireCodec.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Power.hh"

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Simple interrupt handler just for wakeup call and some bookkeeping just 
// to see how many times it is called; many as it is called while low.
class WakeupButton : public InterruptPin {
private:
  uint16_t m_count;
public:
  WakeupButton(Board::InterruptPin pin, InterruptPin::Mode mode) :
    InterruptPin(pin, mode),
    m_count(0)
  {}
  virtual void on_interrupt() { m_count++; }
  uint16_t get_count() { return (m_count); }
};

// Connect RF433 transmitter to ATtiny/D1
VirtualWireCodec codec;
VWI::Transmitter tx(Board::D1, &codec);
const uint16_t SPEED = 4000;

// Connect button with pullup to EXT0/D2
WakeupButton wakeup(Board::EXT0, InterruptPin::ON_LOW_LEVEL_MODE);

void setup()
{
  // Start the Virtual Wire Interface/Transmitter
  VWI::begin(SPEED);
  tx.begin();
  
  // Disable hardware
  VWI::disable();
  Power::adc_disable();
  Power::usi_disable();

  // Enable the interrupt pin
  wakeup.enable();
}

// Message type to send
struct msg_t {
  uint32_t id;
  uint8_t nr;
  uint16_t data[2];
};

void loop()
{
  // Some dummy data to send for each wakeup
  static msg_t msg = { 
    0xC05A0003,
    0, 	
    { 0, 0xa5a5 }
  };

  // Go to sleep in power down mode
  Power::sleep(SLEEP_MODE_PWR_DOWN);
  msg.data[0] = wakeup.get_count();

  // Send the message and update number
  VWI::enable();
  tx.send(&msg, sizeof(msg));
  tx.await();
  VWI::disable();
  msg.nr += 1;
}
