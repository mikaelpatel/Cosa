/**
 * @file CosaPins.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
  // Start the serial interface for trace output
  Serial.begin(9600);

  // Check amount of free memory
  Serial_trace(free_memory());

  // Check size of instances
  Serial_trace(sizeof(Event));
  Serial_trace(sizeof(Queue));
  Serial_trace(sizeof(Pin));
  Serial_trace(sizeof(InputPin));
  Serial_trace(sizeof(InterruptPin));
  Serial_trace(sizeof(AnalogPin));
  Serial_trace(sizeof(OutputPin));
  Serial_trace(sizeof(PWMPin));
  Serial_trace(sizeof(Watchdog));

  // Print debug information about the sketch pins
  onoffPin.println();
  intPin.println();
  ledPin.println();
  levelPin.println();
  tempVCC.println();

  // Check interrupt pin; enable and print interrupt count
  Serial_trace(state);
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

  // Sample the level and check for change
  static uint16_t old_value = 0;
  uint16_t new_value = levelPin.sample();
  int16_t diff = new_value - old_value;
  if (abs(diff) > 100) {
    // Print the time index
    Serial_print("ticks = ");
    Serial.println(Watchdog::get_ticks());
    // Asynchronous sample internal temperature level
    tempVCC.request_sample();
    Serial_print("levelPin = ");
    Serial.println(new_value);
    old_value = new_value;
    Serial_print("tempVCC = ");
    // Await the sample and print value
    Serial.println(tempVCC.await_sample());
    // Check if the led should be on and the pwm level updated
    if (onoffPin.is_set()) {
      ledPin.set(new_value, 0, 1023);
      Serial_print("duty = ");
      Serial.println(ledPin.get_duty());
    }
    else {
      ledPin.clear();
    }
    // Print the interrupt counter
    Serial_trace(state);
  }
}
