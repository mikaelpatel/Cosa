/**
 * @file CosaCapsules.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * A simple UML style modelling language with Capsules, Connectors and
 * Controller.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "UML.hh"

/**
 * A simple model with Clock, Button and LED. The connectors are
 * probed. The LED will blink when the Button is on otherwise the LED
 * is turned off. The probes will trace changes to the connector
 * value. The Clock and the Button are timed capsules. The clock has a
 * period of 1024 ms and the Button 64 ms. The Button will debound the
 * digital input pin. The LED has control dependency to the Clock tick
 * and a data dependency to the Button signal.
 *
 * @section Diagram
 * 
 *    Clock                  Probe
 *  +--------+             +-------+
 *  |   c1   |---[tick]--->|  p1   |
 *  +--------+     |       +-------+
 *   [1024 ms]     -          LED
 *                 |       +-------+
 *                 + - - ->|       |
 *                         |  l1   | 
 *                 +------>|       |
 *                 |       +-------+
 *    Button       |         Probe
 *  +--------+     |       +-------+
 *  |   b1   |---[onoff]-->|  p2   |
 *  +--------+             +-------+
 *     [64 ms]
 */

using namespace UML;

// Forward declaration of the connectors
extern Clock::Tick tick;
extern Button::Signal onoff;

// The capsules with data dependencies (connectors)
Clock c1(tick, 1024);
Button b1(Board::D2, onoff);
LED l1(onoff);

// Some probes to trace connector values
Probe<Clock::Tick> p1(tick);
Probe<Button::Signal> p2(onoff);

// The wiring; control dependencies
Capsule* const tick_listeners[] __PROGMEM = { &p1, &l1, NULL };
Clock::Tick tick(tick_listeners, 0);

Capsule* const onoff_listeners[] __PROGMEM = { &p2, &l1, NULL};
Button::Signal onoff(onoff_listeners, false);

void setup()
{
  // Start trace on serial output
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaCapsules: started"));

  // Use the watchdog for timeout events
  Watchdog::begin(16, Watchdog::push_timeout_events);

  // Start the Timed Capsules
  c1.begin();
  b1.begin();
}

void loop()
{
  Event::service();
  controller.run();
}
