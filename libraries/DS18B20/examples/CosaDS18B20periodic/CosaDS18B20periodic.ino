/**
 * @file CosaDS18B20periodic.ino
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
 * Cosa demonstrate event driven, periodic, handling of the DS18B20
 * 1-Wire device driver.
 *
 * @section Circuit
 * @code
 *                           DS18B20/3
 *                       +------------+
 * (GND)---------------1-|GND         |\
 * (D4)------+---------2-|DQ          | |
 *           |       +-3-|VDD         |/
 *          4K7      |   +------------+
 *           |       |
 * (VCC)-----+       +---(VCC/GND)
 *
 * @endcode

 * Connect Arduino to DS18B20 in D7 and GND. May use parasite
 * powering (connect DS18B20 VCC to GND) otherwise to VCC.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include <OWI.h>
#include <DS18B20.h>

#include "Cosa/Event.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"

// One-wire pin
OWI owi(Board::D4);

class Sensor : public DS18B20, public Periodic {
public:
  /**
   * Construct sensor on given 1-wire bus. Convert and read
   * temperature with given time period (in milli-seconds).
   * @param[in] pin 1-wire bus.
   * @param[in] scheduler for periodic job.
   * @param[in] ms time period for samples (default 2048 ms).
   */
  Sensor(OWI* pin, Job::Scheduler* scheduler, uint16_t ms = 2048) :
    DS18B20(pin),
    Periodic(scheduler, ms / 2),
    m_state(0)
  {}

  /**
   * Return current state.
   */
  uint8_t state()
  {
    return (m_state);
  }

  /**
   * @override Sensor
   * Called when a new sample is available.
   */
  virtual void on_sample();

protected:
  /**
   * @override Periodic
   * Request temperature measurement and read back sample as a simple
   * state machine. Two periods are used.
   */
  virtual void run();

  /** Current state (0=convert request, 1=read sample, 2=error). */
  uint8_t m_state;
};

void
Sensor::run()
{
  switch (m_state) {
  case 0:
    m_state = convert_request() ? 1 : 2;
    break;
  case 1:
    m_state = read_scratchpad() ? 0 : 2;
    on_sample();
    break;
  case 2:
    ASSERT(m_state != 2);
    break;
  }
}

void
Sensor::on_sample()
{
  trace << Watchdog::millis() << ':' << *this << PSTR(" C") << endl;
}

Watchdog::Scheduler scheduler;
Sensor sensor(&owi, &scheduler);

void setup()
{
  // Start trace output stream on the serial port
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaDS18B20periodic: started"));

  // Start the watchdog ticks counter
  Watchdog::begin();

  // Connect to the device and start periodic function
  ASSERT(sensor.connect(0));
  sensor.start();
}

void loop()
{
  // Service events
  Event::service();
}
