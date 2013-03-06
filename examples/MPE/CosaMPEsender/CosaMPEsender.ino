/**
 * @file CosaMPEsender.ino
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
 * Demonstration of the Manchester Phase Encoder (MPE) driver.
 * Transmits a simple message with identity, message number,
 * and two data element; analog samples.
 *
 * @section Circuit
 * Connect RF433/315 Transmitter Data to Arduino(ATtiny85) D12(D2),
 * VCC to Arduino D10(D1) and connect GND. Connect Arduino analog pins
 * A2 and A3 to analog sensors. On ATtiny85 the pins are D2, D1. 
 * The power control pin D10(D1) can also be used for a LED.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/MPE.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"

// Analog pins to sample for values to send
AnalogPin luminance(Board::A2);
AnalogPin temperature(Board::A3);

// Virtual Wire Interface Transmitter and Power Control pins
#if defined(__AVR_ATtiny25__)		\
 || defined(__AVR_ATtiny45__)		\
 || defined(__AVR_ATtiny85__)
MPE::Transmitter tx(Board::D2);
OutputPin pw(Board::D1);
#define SPEED 2000
#else 
MPE::Transmitter tx(Board::D12);
OutputPin pw(Board::D10);
#define SPEED 4000
#endif

void setup()
{
  // Start watchdog for delay
  Watchdog::begin();

  // Start virtual wire interface and transmitter
  MPE::begin(SPEED);
  tx.begin();
}

// Message type to send
struct msg_t {
  uint32_t id;
  uint8_t nr;
  uint16_t data[2];
};

void loop()
{
  static msg_t msg = { 
    0xC05A0002,
    0, 	
    { 0, 0 }
  };

  // Turn power on. While stabilizing sample the analog values
  pw.on();
  msg.data[0] = luminance.sample();
  msg.data[1] = temperature.sample();

  // Send the message with the values and wait for completion
  tx.send(&msg, sizeof(msg));
  tx.await();

  // Turn power off.
  pw.off();
  
  // Update message number and data
  msg.nr += 1;

  // Delivery the next message after a 0.5 second delay
  Watchdog::delay(512);
}
