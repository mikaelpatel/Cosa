/**
 * @file CosaProbe.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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
 * Probe to sample pulse sequence for analysis.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Types.h"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Power.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/UART.hh"
#include "Cosa/Trace.hh"

/**
 * Probe to collect samples using an external interrupt pin.
 * Interrupt handler captures and records the pulse with until
 * max number of samples or illegal pulse. Sample request/await
 * starts interrupt handler and stops on max time.
 */
class Probe : public ExternalInterrupt {
  friend IOStream& operator<<(IOStream& outs, Probe& probe);
private:
  static const uint8_t SAMPLE_MAX = 128;

  volatile uint16_t m_sample[SAMPLE_MAX];
  volatile uint8_t m_sampling;
  volatile uint16_t m_start;
  volatile uint8_t m_ix;

  const uint16_t LOW_THRESHOLD;
  const uint16_t HIGH_THRESHOLD;

  virtual void on_interrupt(uint16_t arg = 0);

public:
  Probe(Board::ExternalInterruptPin pin,
	ExternalInterrupt::InterruptMode mode,
	uint16_t low, uint16_t high) :
    ExternalInterrupt(pin, mode),
    m_sampling(false),
    m_start(0L),
    m_ix(0),
    LOW_THRESHOLD(low),
    HIGH_THRESHOLD(high)
  {
  }

  void sample_request();
  void sample_await(uint32_t ms);
};

IOStream& operator<<(IOStream& outs, Probe& probe)
{
  outs << probe.m_ix << ':';
  for (uint8_t i = 0; i < probe.m_ix; i++)
    outs << ' ' << probe.m_sample[i];
  return (outs);
}

void
Probe::on_interrupt(uint16_t arg)
{
  UNUSED(arg);
  if (m_start == 0) {
    m_start = RTT::micros();
    m_ix = 0;
    return;
  }
  uint16_t stop = RTT::micros();
  uint16_t us = (stop - m_start);
  m_start = stop;
  m_sample[m_ix++] = us;
  if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) goto exception;
  if (m_ix != SAMPLE_MAX) return;

 exception:
  m_sampling = false;
  disable();
}

void
Probe::sample_request()
{
  m_sampling = true;
  m_start = 0;
  enable();
}

void
Probe::sample_await(uint32_t ms)
{
  uint32_t start = RTT::millis();
  DELAY(HIGH_THRESHOLD);
  while (m_sampling && (RTT::since(start) < ms)) {
    Power::sleep(SLEEP_MODE_IDLE);
    uint16_t us = (RTT::micros() - m_start);
    if (us > 1000) break;
  }
  disable();
}

// Probe probe(Board::EXT0, ExternalInterrupt::ON_CHANGE_MODE, 20, 100);
Probe probe(Board::EXT0, ExternalInterrupt::ON_FALLING_MODE, 30, 300);
// Probe probe(Board::EXT0, ExternalInterrupt::ON_RISING_MODE, 30, 300);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaProbe: started"));
  Watchdog::begin();
  RTT::begin();
}

void loop()
{
  sleep(2);

  // Make a request (DHT)
  probe.mode(IOPin::OUTPUT_MODE);
  probe.clear();
  Watchdog::delay(32);
  probe.set();
  probe.mode(IOPin::INPUT_MODE);
  DELAY(40);

  // Wait for the response max 100 ms
  probe.sample_request();
  probe.sample_await(100);

  // Print samples (micro-second pulses)
  trace << probe << endl;
}

