/**
 * @file CosaPins.ino
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
 * Cosa demonstration of pin abstractions.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pin.hh"
#include "Cosa/InputPin.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/PWMPin.hh"
#include "Cosa/AnalogPin.hh"
#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Memory.h"

// Counter Class
class Counter {
private:
  volatile uint16_t m_counter;
public:
  Counter(uint16_t init = 0) :
    m_counter(init)
  {}
  uint16_t get_counter()
  {
    return (m_counter);
  }
  void set_counter(uint16_t value)
  {
    m_counter = value;
  }
  void increment(uint16_t value)
  {
    m_counter += value;
  }
};

// External Interrupt Pin Handler; count interrupts
class ExtPin : public ExternalInterrupt, public Counter {
private:
  virtual void on_interrupt(uint16_t arg)
  {
    UNUSED(arg);
    increment(1);
  }
public:
  ExtPin(Board::ExternalInterruptPin pin) :
    ExternalInterrupt(pin, ExternalInterrupt::ON_RISING_MODE),
    Counter(0)
  {}
};

// Pin Change Interrupt Handler; count interrupts
class IntPin : public PinChangeInterrupt, public Counter {
private:
  virtual void on_interrupt(uint16_t arg)
  {
    UNUSED(arg);
    increment(1);
  }
public:
  IntPin(Board::InterruptPin pin) :
    PinChangeInterrupt(pin),
    Counter(0)
  {}
};

// Input and output pins
ExtPin extPin(Board::EXT0);
IntPin int0Pin(Board::PCI9);
IntPin int1Pin(Board::PCI3);
IntPin int2Pin(Board::PCI15);
PWMPin ledPin(Board::PWM2);
InputPin onoffPin(Board::D7);
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
  TRACE(sizeof(Event::queue));
  TRACE(sizeof(Pin));
  TRACE(sizeof(InputPin));
  TRACE(sizeof(ExternalInterrupt));
  TRACE(sizeof(ExtPin));
  TRACE(sizeof(PinChangeInterrupt));
  TRACE(sizeof(IntPin));
  TRACE(sizeof(AnalogPin));
  TRACE(sizeof(OutputPin));
  TRACE(sizeof(PWMPin));
  TRACE(sizeof(Watchdog));

  // Start the watchdog
  Watchdog::begin();

  // Check interrupt pin; enable and print interrupt counter
  PinChangeInterrupt::begin();
  TRACE(int0Pin.get_counter());
  int0Pin.enable();
  TRACE(int1Pin.get_counter());
  int1Pin.enable();
  TRACE(int2Pin.get_counter());
  int2Pin.enable();
  TRACE(extPin.get_counter());
  extPin.enable();

  // Power up ADC
  AnalogPin::powerup();
}

Watchdog::Clock clock;

void loop()
{
  // Wait for the next seconds tick
  clock.await();

  // Print the time index
  INFO("ms = %ul", Watchdog::millis());

  // Sample the level
  uint16_t value = levelPin.sample();
  INFO("levelPin = %d", value);

  // Check if the led should be on and the pwm level updated
  if (onoffPin.is_set()) {
    ledPin.set(value, 0, 1023);
    INFO("duty = %d", ledPin.duty());

    // Print the interrupt counters
    TRACE(extPin.get_counter());
    TRACE(int0Pin.get_counter());
    TRACE(int1Pin.get_counter());
    TRACE(int2Pin.get_counter());
  }
  else {
    ledPin.set(0);
  }
}
