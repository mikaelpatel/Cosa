/**
 * @file CosaButton.ino
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
 * Demonstration of Cosa debouncing of switch/buttons connected to an
 * input pin. The input pin object will receive falling/rising/changed
 * events from the button which will filter changes on the pin using
 * low frequency sampling (64 ms).
 *
 * @section Circuit
 * A button/switch should be connected to Arduino pin 2 and ground. No
 * additional components are needed as the input pin is configured
 * with input pullup resistor.
 * @code
 *
 * (D2)-----------------+
 *                      |
 *                     (\)
 *                      |
 * (GND)----------------+

 * @endcode
 *
 * The Arduino builtin LED Pin 13/ATtiny Pin 4, is used for on-off state.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Button.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// Use the watchdog job scheduler
Watchdog::Scheduler scheduler;

// Use the built-in led
OutputPin ledPin(Board::LED);

// On-off button
class OnOffButton : public Button {
public:
  OnOffButton(Board::DigitalPin pin, Button::Mode mode) :
    Button(&scheduler, pin, mode),
    m_count(0)
  {}

  virtual void on_change(uint8_t type)
  {
    UNUSED(type);
    ledPin.toggle();
    m_count += 1;
    if (ledPin.is_set()) {
      INFO("%d: on", m_count);
    }
    else {
      INFO("%d: off", m_count);
    }
  }

private:
  uint8_t m_count;
};

OnOffButton onOff(Board::D2, Button::ON_FALLING_MODE);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaButton: started"));

  // Check amount of free memory and size of instances
  TRACE(free_memory());
  TRACE(sizeof(Event::Handler));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(Link));
  TRACE(sizeof(Job));
  TRACE(sizeof(Button));
  TRACE(sizeof(OnOffButton));

  // Start the watchdog
  Watchdog::begin();

  // Start the button handler
  onOff.start();
}

void loop()
{
  Event::service();
}
