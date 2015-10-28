/**
 * @file CosaPinChangeInterrupt.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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
 * Demonstration of Pin Change Interrupt handler.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/PinChangeInterrupt.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

class Counter : public PinChangeInterrupt {
public:
  Counter(Board::InterruptPin pin, InterruptMode mode) :
    PinChangeInterrupt(pin, mode),
    m_count(0)
  {}
  void reset()
  {
    synchronized m_count = 0;
  }
  uint16_t count() const
  {
    uint16_t res;
    synchronized res = m_count;
    return (res);
  }
private:
  virtual void on_interrupt(uint16_t arg)
  {
    UNUSED(arg);
    m_count++;
  }
  uint16_t m_count;
};

Counter pin(Board::PCI7, PinChangeInterrupt::ON_RISING_MODE);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinChangeInterrupt: started"));
  Watchdog::begin();
  RTT::begin();
  PinChangeInterrupt::begin();
  pin.enable();
}

void loop()
{
  periodic(timer, 1000) {
    uint16_t count = pin.count();
    trace << RTT::millis() << ':' << count << endl;
    trace.flush();
    pin.reset();
  }
  yield();
}
