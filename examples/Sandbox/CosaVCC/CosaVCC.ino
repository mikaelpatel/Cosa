/**
 * @file CosaVCC.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * Cosa demonstration of VCC monitoring for low power condition.
 * Class VCC may be reused. Override the default on_low_power() method.
 *
 * @section Note
 * Should be compiled for standard Arduino. VCC class may be used 
 * for ATtinyX5. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"

class VCC : public Periodic {
private:
  uint16_t m_threshold;
  uint16_t m_vcc;
  virtual void run()
  {
    m_vcc = AnalogPin::bandgap();
    on_low_voltage();
    if (m_vcc > m_threshold) return;
  }
public:
  VCC(uint16_t mv, uint16_t ms = 1024) : 
    Periodic(ms), 
    m_threshold(mv),
    m_vcc(0)
  {}
  virtual void on_low_voltage()
  {
    trace << Watchdog::get_millis() / 1000 << ':' << m_vcc << PSTR(" mV\n"); 
  }
};

VCC lowPower(4500);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVCC: started"));
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
}

