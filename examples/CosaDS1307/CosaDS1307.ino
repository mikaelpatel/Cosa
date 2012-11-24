/**
 * @file CosaDS1307.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Cosa demonstration of the DS1307 I2C/Two-Wire Realtime clock device.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/TWI/DS1307.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"

// The real-time device, time and state kept in ram in device
DS1307 rtc;
DS1307::timekeeper_t now;
uint8_t state[16];

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaDS1703: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter
  Watchdog::begin(16);

  // Initate state vector (restart will force zero of the ram)
  rtc.write_ram(state, sizeof(state), 16);
}

void loop()
{
  // Wait a second
  Watchdog::delay(1000);

  // Read the time from the rtc device and print
  rtc.get_time(now);
  uint8_t* buffer = (uint8_t*) &now;
  for (uint8_t i = 0; i < sizeof(now); i++) {
    trace.print(buffer[i], 16);
    trace.print_P(PSTR(" "));
  }
  trace.println();

  // Read the state and update
  rtc.read_ram(state, sizeof(state), 16);
  for (uint8_t i = 0; i < sizeof(state); i++) {
    trace.print(state[i], 16);
    trace.print_P(PSTR(" "));
    state[i]++;
  }
  trace.println();
  rtc.write_ram(state, sizeof(state), 16);

  // Clear the local state copy to verify write and read back
  for (uint8_t i = 0; i < sizeof(state); i++) {
    state[i] = 0;
  }
}
