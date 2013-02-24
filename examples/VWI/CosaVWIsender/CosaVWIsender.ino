/**
 * @file CosaVWIsender.ino
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
 * Demonstration of the Virtual Wire Interface (VWI) driver.
 * Transmits a simple message with identity, message number,
 * and 32-bit data element.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter to Arduino D12.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/VWI.hh"
#include "Cosa/Watchdog.hh"

// Virtual Wire Interface Transmitter connected to pin D12
VWI::Transmitter tx(Board::D12);

void setup()
{
  // Start watchdog for delay
  Watchdog::begin();

  // Start virtual wire interface and transmitter
  VWI::begin(2000);
  tx.begin();
}

// Message type to send
struct msg_t {
  uint32_t id;
  uint8_t nr;
  uint32_t data;
};

void loop()
{
  static msg_t msg = { 
    0xdeadbeef,
    0, 	
    0x12345678
  };

  // Send message and await completion
  tx.send(&msg, sizeof(msg));
  tx.await();

  // Update message number and data
  msg.nr += 1;
  msg.data += 0x10001;

  // Delivery the next message after a 0.5 second delay
  Watchdog::delay(512);
}
