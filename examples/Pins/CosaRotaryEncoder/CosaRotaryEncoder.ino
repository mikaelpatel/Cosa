/**
 * @file CosaRotaryEncoder.ino
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
 * Cosa demonstration of Rotary Encoder and Dial.
 *
 * @section Circuit
 * KY-040 Rotary Encoder Module.
 *
 *                          Dial/dail
 *                       +------------+
 * (PCI6)--------------1-|CLK         |
 * (PCI7)--------------2-|DT          |
 *                     3-|SW   (/)    |
 * (VCC)---------------4-|VCC         |
 * (GND)---------------5-|GND         |
 *                       +------------+
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Rotary.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

void setup()
{
  // Use the UART as output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaRotaryEncoder: started"));

  // Start the interrupt pin handler
  PinChangeInterrupt::begin();
}

// Rotary Dial is connected to D6/D1 and D7/D2 (as interrupt pins)
// Mode: full cycle, Initial: -100, Min: -100, Max: 10, Step: 1
#if defined(__ARDUINO_TINY__)
Rotary::Dial<int> 
dial(Board::PCI1, Board::PCI2, Rotary::Encoder::FULL_CYCLE, -100, -100, 10, 1);
#else
// Rotary::AcceleratedDial<int, 100> 
// dial(Board::PCI6, Board::PCI7, Rotary::Encoder::FULL_CYCLE, -100, -100, 10, 1, 10);
Rotary::Dial<int> 
dial(Board::PCI6, Board::PCI7, Rotary::Encoder::FULL_CYCLE, -100, -100, 10, 1);
#endif

void loop()
{
  // Rotary Encoder/Dial will push event when a change occurs
  Event event;
  Event::queue.await(&event);

  // Dispatch the event so that the dial value is updated
  event.dispatch();

  // Change step mode at min and max value
  static int old_value = -100;
  int new_value = dial.get_value();
  if (old_value == -100 && new_value == -99)
    dial.set_mode(Rotary::Encoder::FULL_CYCLE);
  else if (old_value == 10 && new_value == 9)
    dial.set_mode(Rotary::Encoder::HALF_CYCLE);
  old_value = new_value;

  // Print the new value
  trace << new_value << endl;
}

