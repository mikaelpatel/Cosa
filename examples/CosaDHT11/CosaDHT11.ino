/**
 * @file CosaDHT11.ino
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
 * Cosa demonstration of the DHT11 device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/DHT11.h"
#include "Cosa/Pins.h"
#include "Cosa/Trace.h"
#include "Cosa/Watchdog.h"

// Connect devices to pins
DHT11 dht11(7);
OutputPin ledPin(13);

void setup()
{
  // Start trace output stream
  trace.begin(9600, PSTR("CosaDHT11: started"));

  // Start the watchdog ticks and push time events
  Watchdog::begin(16);
}

void loop()
{
  // Wait for the watchdog
  Watchdog::delay(2048);

  // Sample the device for humidity and temperature
  ledPin.toggle();
  dht11.sample();
  ledPin.toggle();

  // Print the sampled values
  dht11.print();
}
