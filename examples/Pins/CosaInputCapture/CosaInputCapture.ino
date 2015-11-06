/**
 * @file CosaInputCapture.ino
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
 * Demonstration of Input Capture handler. Capture period between
 * falling signal on input capture pin (ICP1/D8). Calculate latency
 * from capture to interrupt handler.
 *
 * @section Measurement
 * Typical latency from event detect to object-oriented interrupt
 * handler is 4 us (48 cycles). This includes state save, read of input
 * capture register and dispatch in ISR.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/InputCapture.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/CPU.hh"

class Probe : public InputCapture {
public:
  Probe() : InputCapture(), m_triggers(0) {}
  uint16_t get_latency() const { return (m_latency); }
  uint16_t get_triggers() const { return (m_triggers); }
  void reset() { m_triggers = 0; }
protected:
  virtual void on_interrupt(uint16_t arg);
  uint16_t m_triggers;
  uint16_t m_latency;
};

void
Probe::on_interrupt(uint16_t arg)
{
  // Calculate latency; arg is the timer capture
  m_latency = TCNT1 - arg;
  // Count number of calls
  m_triggers++;
}

// The input capture probe on pin D8 (implicit)
Probe probe;

void setup()
{
  uart.begin(57600);
  trace.begin(&uart, PSTR("CosaInputCapture: started"));
  trace << PSTR("ICP1 - D8") << endl;
  Watchdog::begin();
  RTT::begin();
  InputCapture::begin();
}

void loop()
{
  // Periodically print capture period in hz and latency in us
  probe.reset();
  RTT::await();
  probe.enable();
  delay(1000);
  probe.disable();
  trace << probe.get_triggers() << PSTR(" hz,")
	<< probe.get_latency() / I_CPU << PSTR(" us")
	<< endl;
  trace.flush();
}
