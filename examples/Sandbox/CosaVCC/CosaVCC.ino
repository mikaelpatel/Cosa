/**
 * @file CosaVCC.ino
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
 * Cosa demonstration of VCC monitoring for low power condition.
 * Class VCC may be reused. Override the default on_low_power() method.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"

// Use the Watchdog job scheduler
Watchdog::Scheduler scheduler;

/**
 * Monitor power supply for low voltage/battery.
 */
class VCC : public Periodic {
public:
  VCC(uint16_t mv, uint16_t sec = 1) :
    Periodic(&scheduler, sec * 1000UL),
    m_threshold(mv),
    m_vcc(0)
  {}
  virtual void run()
  {
    m_vcc = AnalogPin::bandgap();
    if (m_vcc > m_threshold) return;
    on_low_voltage();
  }
  virtual void on_low_voltage()
  {
    trace << Watchdog::millis()
	  << PSTR(":warning:low voltage(VCC = ")
	  << m_vcc << PSTR(" mV)\n");
  }
  uint16_t vcc()
  {
    return (m_vcc);
  }
private:
  uint16_t m_threshold;
  uint16_t m_vcc;
};

// Monitor low voltage at 4.4 V
VCC lowPower(4400);

/**
 * Periodical sampling of analog pin.
 */
class Sampler : public AnalogPin, public Periodic {
public:
  Sampler(Board::AnalogPin pin, uint16_t ms) :
    AnalogPin(pin),
    Periodic(&scheduler, ms),
    m_count(0)
  {}
  virtual void run()
  {
    m_count++;
    sample_request();
  }
  uint16_t count()
  {
    return (m_count);
  }
private:
  uint16_t m_count;
};

Sampler sampler(Board::A0, 250);

/**
 * Periodical display the values
 */
class Display : public Periodic {
public:
  Display(uint16_t sec = 1) : Periodic(&scheduler, sec * 1000UL)
  {
    expire_at(1000);
  }
  virtual void run()
  {
    trace << Watchdog::millis()
	  << PSTR(":A0:") << sampler.count()
	  << PSTR(" = ") << sampler.value()
	  << PSTR(":Vcc  = ") << lowPower.vcc()
	  << endl;
  }
};

// Print the latest sample value and voltage
Display display(1);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVCC: started"));
  AnalogPin::powerup();
  lowPower.start();
  sampler.start();
  display.start();
  Watchdog::begin();
}

void loop()
{
  Event::service();
}

