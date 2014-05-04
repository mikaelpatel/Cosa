/**
 * @file CosaPinChangeInterrupt.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"

class Counter : public PinChangeInterrupt {
public:
  Counter(Board::InterruptPin pin) : 
    PinChangeInterrupt(pin), 
    m_count(0) 
  {}
  uint16_t get_count() const
  {
    return (m_count);
  }
private:
  virtual void on_interrupt(uint16_t arg) 
  { 
    UNUSED(arg);
    m_count++;
  }
  uint16_t m_count;
};

Counter pin(Board::PCI0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaPinChangeInterrupt: started"));
  Watchdog::begin();
  PinChangeInterrupt::begin();
  pin.enable();
}

void loop()
{
  delay(1000);
  trace << Watchdog::millis() << ':' << pin.get_count() << endl;
}
