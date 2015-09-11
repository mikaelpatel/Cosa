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
 * @section Note
 * Should be compiled for standard Arduino. VCC class may be used
 * for ATtinyX5 by reimplementing on_low_power().
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/AnalogPin.hh"
#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/Periodic.hh"

// Use the watchdog job scheduler
Watchdog::Scheduler scheduler;

/**
 * Monitor power supply for low voltage/battery.
 */
class VCC : public Periodic {
public:
  VCC(uint16_t mv, uint16_t sec = 2) :
    Periodic(&scheduler, sec * 1024),
    m_threshold(mv),
    m_vcc(0)
  {
  }
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
  uint16_t get_vcc() { return (m_vcc); }
private:
  uint16_t m_threshold;
  uint16_t m_vcc;
};

// Monitor low voltage at 4.4 V
// VCC lowPower(4400);
VCC lowPower(4800);

/**
 * Periodical sampling of analog pin.
 */
class Sampler : public AnalogPin, public Periodic {
public:
  Sampler(Board::AnalogPin pin, uint16_t ms) : AnalogPin(pin), Periodic(&scheduler, ms) {}
  virtual void run() { sample_request(); }
};

// Sample analog pin A4 (A1 on ATTINY) four times per second
#if !defined(BOARD_ATTINY)
Sampler sampler(Board::A4, 256);
#else
Sampler sampler(Board::A1, 256);
#endif

/**
 * Periodical display the values
 */
class Display : public Periodic {
public:
  Display(uint16_t ms) : Periodic(&scheduler, ms) {}
  virtual void run()
  {
    trace << Watchdog::millis() << PSTR(":A4  = ") << sampler.get_value()
	  << PSTR(":Vcc  = ") << lowPower.get_vcc()
	  << endl;
  }
};

// Print the latest sample value and voltage
Display display(1024);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaVCC: started"));
  Watchdog::begin();
  lowPower.start();
  sampler.start();
  display.start();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  event.dispatch();
  if (event.get_type() == Event::SAMPLE_COMPLETED_TYPE)
    AnalogPin::powerdown();
}

