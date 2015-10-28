/**
 * @file CosaDHT.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * @section Description
 * Cosa demonstration of the DHT11/DHT22 device driver.
 *
 * @section Circuit
 * @code
 *                       DHT11/outdoors
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D2/EXT0)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 *
 *                       DHT22/indoors
 *                       +------------+
 * (VCC)---------------1-|VCC  ====== |
 * (D3/EXT1)-----------2-|DATA ====== |
 *                     3-|     ====== |
 * (GND)---------------4-|GND  ====== |
 *                       +------------+
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <DHT.h>

#include "Cosa/Memory.h"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Time.hh"
#include "Cosa/Periodic.hh"

// Configuration
#define NO_INDOORS

// Use DHT11 for outdoors measurement and DHT22 for indoors
DHT11 outdoors(Board::EXT0);

// ATtiny has only one external interrupt pin
#if defined(BOARD_ATTINY)
#define NO_INDOORS
Soft::UAT uart(Board::D1);
#elif !defined(NO_INDOORS)
DHT22 indoors(Board::EXT1);
#endif

// Start time in milli-seconds
const uint32_t START = 8 * 3600 * 1000UL;

// Use the Real-Time Clock
RTT::Clock clock;

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDHT: started"));

  // Check amount of free memory and size of instance
  TRACE(free_memory());
  TRACE(sizeof(DHT));
  TRACE(sizeof(DHT11));
  TRACE(sizeof(DHT22));

  // Start the watchdog for low power sleep
  Watchdog::begin();
  RTT::begin();
  Watchdog::millis(START);
  RTT::millis(START);
  clock.time(START / 1000);
}

void loop()
{
  // Periodically read and print humidity and temperature
  periodic(timer, 2000) {
#if !defined(NO_INDOORS)
    trace << time_t(clock.time()) << ':';
    trace << hex << RTT::micros() << PSTR(":indoors: ");
    if (indoors.sample())
      trace << indoors;
    else
      trace << PSTR("failed");
    trace << endl;
#endif
    trace << time_t(clock.time()) << ':';
    trace << hex << RTT::micros() << PSTR(":outdoors: ");
    if (outdoors.sample())
      trace << outdoors;
    else
      trace << PSTR("failed");
    trace << endl;
  }
  yield();
}
