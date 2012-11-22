/**
 * @file CosaPins.ino
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
 * Cosa demonstration of pin abstractions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Memory.h"
#include "Cosa/Pins.h"
#include "Cosa/Watchdog.h"
#include "Cosa/Trace.h"

// Callback for interrupt pin(2) 

volatile uint16_t state = 0;

void do_interrupt(InterruptPin* pin, void* env)
{
  state++;
}

// Input and output pins

InterruptPin intPin(2, InterruptPin::ON_RISING_MODE, do_interrupt);
PWMPin ledPin(5);
InputPin onoffPin(7);
AnalogPin tempVCC(8, AnalogPin::A1V1_REFERENCE);
AnalogPin levelPin(14);

void setup()
{
  // Start trace output stream
  trace.begin(9600);

  // Info message using the trace log
  INFO("Initiated trace log", 0);

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Event));
  TRACE(sizeof(Queue));
  TRACE(sizeof(Pin));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(InterruptPin));
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(PWMPin));
  TRACE(sizeof(Watchdog));

  // Print debug information about the sketch pins
  intPin.println();
  ledPin.println();
  onoffPin.println();
  levelPin.println();
  tempVCC.println();

  // Check interrupt pin; enable and print interrupt count
  TRACE(state);
  intPin.enable();

  // Start the watchdog ticks counter (1 second pulse)
  Watchdog::begin(1024, Watchdog::push_event);

  // Give to serial interface some time
  delay(10);
}

void loop()
{
  // Wait for the next event. Allow a low power sleep
  Event event;
  Event::queue.await(&event);

  // Info message using the trace log
  INFO("Event received (type = %d)", event.get_type());

  // Sample the level and check for change
  static uint16_t old_value = 0;
  uint16_t new_value = levelPin.sample();
  int16_t diff = new_value - old_value;
  if (abs(diff) > 100) {

    // Print the time index
    trace.printf_P(PSTR("ticks = %d\n"), Watchdog::get_ticks());

    // Asynchronous sample internal temperature level
    tempVCC.request_sample();
    trace.printf_P(PSTR("levelPin = %d\n"), new_value);
    old_value = new_value;

    // Await the sample and print value
    trace.printf_P(PSTR("tempVCC = %d\n"), tempVCC.await_sample());

    // Check if the led should be on and the pwm level updated
    if (onoffPin.is_set()) {
      ledPin.set(new_value, 0, 1023);
      trace.printf_P(PSTR("duty = %d\n"), ledPin.get_duty());
    }
    else {
      ledPin.clear();
    }

    // Print the interrupt counter
    TRACE(state);
  }
}
