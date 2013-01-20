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

#include "Cosa/Pins.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/UART.hh"
#include "Cosa/Memory.h"

// Input interrupt pin with counter
class CounterInterruptPin : public InterruptPin {
private:
  volatile uint16_t m_counter;
  virtual void on_interrupt() { m_counter++; }
public:
  CounterInterruptPin(Board::InterruptPin pin) :
    InterruptPin(pin, InterruptPin::ON_RISING_MODE),
    m_counter(0)
  {}
  uint16_t get_counter() { return (m_counter); }
};

// Input and output pins
CounterInterruptPin intPin(Board::EXT0);
PWMPin ledPin(Board::PWM1);
InputPin onoffPin(Board::D7);
AnalogPin tempVCC(Board::A8, AnalogPin::A1V1_REFERENCE);
AnalogPin levelPin(Board::A0);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPins: started"));

  // Check amount of free memory
  TRACE(free_memory());

  // Check size of instances
  TRACE(sizeof(Event));
  TRACE(sizeof(Queue));
  TRACE(sizeof(Pin));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(InterruptPin));
  TRACE(sizeof(CounterInterruptPin));
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

  // Check interrupt pin; enable and print interrupt counter
  TRACE(intPin.get_counter());
  intPin.enable();

  // Start the watchdog ticks counter (1 second pulse)
  Watchdog::begin(1024, SLEEP_MODE_IDLE, Watchdog::push_watchdog_event);
}

void loop()
{
  // Wait for the next event. Allow a low power sleep
  Event event;
  Event::queue.await(&event);

  // Sample the level
  uint16_t value = levelPin.sample();

  // Print the time index
  INFO("ticks = %d", Watchdog::get_ticks());
  
  // Asynchronous sample internal temperature level
  tempVCC.sample_request();
  INFO("levelPin = %d", value);

  // Await the sample and print value
  INFO("tempVCC = %d", tempVCC.sample_await());

  // Check if the led should be on and the pwm level updated
  if (onoffPin.is_set()) {
    ledPin.set(value, 0, 1023);
    INFO("duty = %d", ledPin.get_duty());
  }
  else {
    ledPin.clear();
  }

  // Print the interrupt counter
  TRACE(intPin.get_counter());
}
