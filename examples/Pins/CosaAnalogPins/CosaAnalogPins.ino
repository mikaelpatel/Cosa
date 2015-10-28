/**
 * @file CosaAnalogPins.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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
 * Cosa demonstration of analog sample set.
 *
 * @section Circuit
 * @code
 *
 * (A0)-----------------<
 * (A1)-----------------<
 * (A2)-----------------<
 * (A3)-----------------<
 *
 * @endcode
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OutputPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/AnalogPins.hh"
#include "Cosa/Board.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// Analog pin vector for pin set. Note: use program memory
const Board::AnalogPin pins[] __PROGMEM = {
  Board::A0,
  Board::A1,
  Board::A2,
  Board::A3
};

// Buffer for sample values
uint16_t buffer[membersof(pins)];

// Declare the pin set with vector, buffer and number of members
// Use default reference voltage
AnalogPins analogPins(pins, buffer, membersof(pins));

// Use the builtin led for a heartbeat
OutputPin ledPin(Board::LED);

void setup()
{
  // Start the trace output stream
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaAnalogPins: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(Trace));
  TRACE(sizeof(Event));
  TRACE(sizeof(Event::queue));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(AnalogPins));
  TRACE(sizeof(pins));
  TRACE(sizeof(buffer));
  TRACE(membersof(pins));

  // Start the watchdog ticks counter (shortest possible tick, 16 ms)
  Watchdog::begin();
}

void loop()
{
  // Start sampling analog pins and wait for the next event
  ledPin.toggle();
  TRACE(analogPins.samples_request());
  Event event;
  Event::queue.await(&event);
  ledPin.toggle();

  // Print the values
  for (uint8_t i = 0; i < membersof(pins); i++)
    TRACE(buffer[i]);

  // Wait (approx. 0.5 s) before issuing the next sample
  Watchdog::delay(512);
}
