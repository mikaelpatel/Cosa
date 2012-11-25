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
#include "Cosa/Pins.h"
#include "Cosa/TWI/DS1307.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"

// The real-time device, latest start and sample time in ram
DS1307 rtc;
DS1307::timekeeper_t now;
struct {
  DS1307::timekeeper_t start;
  DS1307::timekeeper_t sample;
} latest;
const uint8_t ram_pos = sizeof(DS1307::timekeeper_t);

// Use the buildin led as a heartbeat
OutputPin ledPin(13, 0);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaDS1703: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Start the watchdog ticks counter
  Watchdog::begin(16);
}

void loop()
{
  // Wait a second
  Watchdog::delay(1000);
  ledPin.toggle();

  // Read the time from the rtc device and print
  rtc.get_time(now);
  trace.print_P(PSTR("rtc (bcd):"));
  trace.print(&now, sizeof(now), 16);
  now.to_binary();
  trace.print_P(PSTR("rtc (bin):"));
  trace.print(&now, sizeof(now), 16);
  now.to_bcd();
  trace.print_P(PSTR("rtc (bcd):"));
  trace.print(&now, sizeof(now), 16);
  trace.println();

  // Read the latest start and sample time
  rtc.read_ram(&latest, sizeof(latest), ram_pos);
  trace.print_P(PSTR("ram (start):"));
  trace.print(&latest.start, sizeof(latest.start), 16);
  trace.print_P(PSTR("ram (sampl):"));
  trace.print(&latest.sample, sizeof(latest.sample), 16);
  latest.sample = now;
  rtc.write_ram(&latest, sizeof(latest), ram_pos);
  trace.println();

  // Heartbeat
  ledPin.toggle();
}
