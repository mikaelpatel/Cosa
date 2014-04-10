/**
 * @file CosaBlinkInterrupt.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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
 * Cosa LED blink with watchdog timeout interrupt callback and sleep
 * mode for low power.   
 *
 * @section Circuit
 * This example requires no special circuit. The Arduino Pin 13
 * (builtin LED) is used.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/Watchdog.hh"

// Use the builtin led
OutputPin ledPin(Board::LED);

// Interrupt handler; toggle the led for each interrupt
void blink(void* env)
{
  ledPin.toggle();
}

void setup()
{
  // Start watchdog with approx. 0.5 s timeout and blink interrupt call
  Watchdog::begin(512, blink);
}

void loop()
{
  // Sleep during timeout wait
  sleep(10);
}
