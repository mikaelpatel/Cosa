/**
 * @file CosaOWImaster.ino
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
 * Cosa demonstration of the OneWire Master-Slave.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Trace.hh"
#include "Cosa/Memory.h"

// Slave device driver
class Driver : public OWI::Driver {
private:
  static const uint8_t STATUS_MAX = 8;
  uint8_t m_status[STATUS_MAX];
public:
  Driver(OWI* pin) : OWI::Driver(pin) {}
  bool read_status();
  void print_status(IOStream& stream = trace);
};

bool
Driver::read_status()
{
  if (!match_rom()) return (0);
  m_pin->write(OWI::Device::STATUS);
  m_pin->begin();
  for (uint8_t i = 0; i < membersof(m_status) - 1; i++)
    m_status[i] = m_pin->read(8);
  m_status[7] = m_pin->end();
  return (m_pin->end() == 0);
}

void 
Driver::print_status(IOStream& stream)
{
  stream.print(m_status, sizeof(m_status));
}

// The OneWire bus on pin 7 and led heartbeat
OWI owi(Board::D7);
Driver driver(&owi);
OutputPin ledPin(Board::LED);

void setup()
{
  trace.begin(9600, PSTR("CosaOWImaster: started"));
  TRACE(free_memory());
  Watchdog::begin();
  driver.connect(0xC0, 0);
  driver.print_rom();
}

#define __DEBUG__

void loop()
{
  static uint16_t ok = 0;
  static uint16_t err = 0;

  // Read status
  ledPin.toggle();
  if (driver.read_status()) ok++; else err++;
  ledPin.toggle();

#ifdef __DEBUG__
  trace.print(ok);
  trace.print_P(PSTR(":"));
  trace.print(err);
  trace.print_P(PSTR(" "));
  driver.print_status();
  Watchdog::delay(512);
#else
  if (ok % 100 == 0) {
    trace.print(ok);
    trace.print_P(PSTR(":"));
    trace.print(err);
    trace.println();
  }
  Watchdog::delay(128);
#endif
}
