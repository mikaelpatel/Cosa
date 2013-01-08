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

#include "Cosa/TWI/DS1307.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

// The real-time device, latest start and sample time in ram
DS1307 rtc;

// Data structure stored in device ram; last set and run time
struct latest_t {
  DS1307::timekeeper_t set;
  DS1307::timekeeper_t run;
};
const uint8_t ram_pos = sizeof(DS1307::timekeeper_t);

// Use the builtin led for a heartbeat
OutputPin ledPin(13);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaDS1703: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Read the latest set and run time
  latest_t latest;
  rtc.read_ram(&latest, sizeof(latest), ram_pos);

  // Convert bcd to binary and print latest set time
  latest.set.to_binary();
  trace.print_P(PSTR("set on "));
  latest.set.print();
  trace.println();

  // And the latest run time
  latest.run.to_binary();
  trace.print_P(PSTR("run on "));
  latest.run.print();
  trace.println();

  // Update the run time with the current time and update ram
  DS1307::timekeeper_t now;
  rtc.get_time(now);
  latest.run = now;
  rtc.write_ram(&latest, sizeof(latest), ram_pos);
}

void loop()
{
  // Wait a second
  SLEEP(1);
  ledPin.toggle();

  // Read the time from the rtc device and print
  DS1307::timekeeper_t now;
  rtc.get_time(now);
  now.to_binary();
  now.print();
  trace.println();

  // Heartbeat
  ledPin.toggle();
}
