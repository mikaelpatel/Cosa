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
#include "Cosa/Pins.hh"

// Simple null interrupt handler just for wakeup call
class WakeupButton : public InterruptPin {
public:
  WakeupButton(Board::InterruptPin pin, InterruptPin::Mode mode) :
    InterruptPin(pin, mode)
  {}
  virtual void on_interrupt() {}
};

// Connect RF433 transmitter to ATtiny/D1
VWI::Transmitter tx(Board::D1);
#define SPEED 4000

// Connect button with pullup to EXT0/D2
WakeupButton wakeup(Board::EXT0, InterruptPin::ON_LOW_LEVEL_MODE);

void setup()
{
  // Start the Virtual Wire Interface and transmitter at given speed
  VWI::begin(SPEED);
  tx.begin();

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
    { 0x5a5a, 0xa5a5 }
  };

  // Go to sleep in power mode node (approx. 
  cli();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();

  // Send the message and update number
  tx.send(&msg, sizeof(msg));
  tx.await();
  msg.nr += 1;
}
