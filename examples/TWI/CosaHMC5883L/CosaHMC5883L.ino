/**
 * @file CosaHMC5883L.ino
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
 * Cosa HMC5883L 3-Axis Digital Compass IC driver example.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/TWI/Driver/HMC5883L.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

// The 3-Axis Digital Compass
HMC5883L compass;

// Use the built-in led as a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaHMC5883L: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(TWI));
  TRACE(sizeof(compass));

  // Check swap implementation
  // ASSERT(swap(0x1234) == 0x3412);
  // ASSERT(swap(0x12345678) == 0x78563412);

  // Start the watchdog ticks and push time events
  Watchdog::begin(1024, SLEEP_MODE_IDLE, Watchdog::push_watchdog_event);

  // Start the compass
  TRACE(compass.begin());

  // Check the device status; output data ready?
  while (!compass.available()) SLEEP(1);
  
  // Fetch the sample and print
  HMC5883L::data_t output;
  TRACE(compass.read_data(output));
  trace << output.x << '.' << output.y << '.' << output.z << endl;

  // Set continous measurement mode
  TRACE(compass.set_mode(HMC5883L::CONTINOUS_MEASUREMENT_MODE));
}

void loop()
{
  // Wait for the watchdog event
  Event event;
  Event::queue.await(&event);
  ledPin.toggle();

  // Check the device status; output data ready?
  bool res;
  do {
    TRACE(res = compass.available());
  } while (!res);

  // Fetch the sample and print
  HMC5883L::data_t output;
  TRACE(compass.read_data(output));
  trace << output.x << '.' << output.y << '.' << output.z << endl;
}
