/**
 * @file CosaTinyMonitor.ino
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
 * Monitor soft uart output from ATtiny. Read and echo received 
 * characters.
 *
 * @section Circuit
 * Connect Arduino D7 to ATtiny D0 (default).
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/Trace.hh"
#include "Cosa/Board.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"

InputPin data(Board::D7);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaTinyMonitor: started"));
  Watchdog::begin();
}

static const uint16_t WAIT_US = 5;
static const uint16_t WAIT_MAX = 1000;
static const uint16_t BITS_PER_SECOND = 9600;
static const uint16_t US_PER_BIT = 1000000L / BITS_PER_SECOND;

int
read_tiny(InputPin* pin, uint16_t max)
{
  uint16_t wait = 0;
  max /= WAIT_US;
  while (pin->is_set() || wait < max) {
    DELAY(WAIT_US);
    wait += 1;
  }
  if (wait == max) return (-1);
  uint8_t data = 0;
  DELAY(US_PER_BIT/2);
  for (uint8_t i = 0; i < CHARBITS; i++) {
    DELAY(US_PER_BIT);
    data >>= 1;
    if (pin->is_set()) data |= 0x80;
  }
  return (data);
}

void loop()
{
  int c = read_tiny(&data, WAIT_MAX);
  if (c == -1) return;
  trace << (char) c;
}
