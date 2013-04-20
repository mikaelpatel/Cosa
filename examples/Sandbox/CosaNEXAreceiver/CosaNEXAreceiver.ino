/**
 * @file CosaNexaAnalyser.ino
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
 * Simple sketch to capture Nexa Home Wireless Switch Remote codes.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Pins.hh"
#include "Cosa/ExternalInterruptPin.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Memory.h"

class NEXA {
public:
  union code_t {
    uint32_t raw;
    struct {
      uint8_t unit:4;
      uint8_t onoff:1;
      uint8_t group:1;
      uint32_t house:26;
    };

    code_t(uint32_t value) { raw = value; }

    void println(IOStream& outs) 
    { 
      outs << PSTR("house = ") << house 
	   << PSTR(", group = ") << group
	   << PSTR(", unit = ") << unit
	   << PSTR(", on/off = ") << onoff 
	   << endl;
    }
  };
  
  class Receiver : public ExternalInterruptPin {
  private:
    static const uint16_t SAMPLE_MAX = 129;
    static const uint16_t LOW_THRESHOLD = 200;
    static const uint16_t BIT_THRESHOLD = 500;
    static const uint16_t HIGH_THRESHOLD = 1500;
    static const uint16_t TIMEOUT = 512;
    volatile uint16_t m_sample[SAMPLE_MAX];
    volatile uint32_t m_start;
    volatile uint32_t m_code;
    volatile uint8_t m_ix;

    virtual void on_interrupt(uint16_t arg = 0);

    uint32_t decode();

    void sample();

  public:
    Receiver(Board::ExternalInterruptPin pin) :
      ExternalInterruptPin(pin, ExternalInterruptPin::ON_CHANGE_MODE),
      m_start(0),
      m_code(0),
      m_ix(0)
    {}

    code_t get_code() { return (m_code); }

    code_t read_code();
  };
};

void 
NEXA::Receiver::on_interrupt(uint16_t arg) 
{ 
  if (m_start == 0L) {
    if (is_clear()) return;
    m_start = RTC::micros();
    m_ix = 0;
    return;
  }

  uint32_t stop = RTC::micros();
  uint32_t us = (stop - m_start);
  uint32_t bits = 0L;
  m_start = stop;
  if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) goto exception;
  m_sample[m_ix++] = us;
  if (m_ix != SAMPLE_MAX) return;

  m_start = 0L;
  bits = decode();
  if ((bits == 0) || (m_code == bits)) return;
  m_code = bits;
  Event::push(Event::READ_COMPLETED_TYPE, this);
  return;

 exception:
  m_start = 0L;
}

uint32_t
NEXA::Receiver::decode()
{
  uint32_t bits = 0L;
  uint8_t bit = 0;
  uint8_t ix = 0;
  for (uint8_t i = 0; i < CHARBITS * sizeof(bits); i++) {
    bit = ((m_sample[ix] < BIT_THRESHOLD) << 1) 
      |   (m_sample[ix+1] < BIT_THRESHOLD);
    if (bit < 2) return (0);
    ix += 2;
    bit = ((m_sample[ix] < BIT_THRESHOLD) << 1) 
      |   (m_sample[ix+1] < BIT_THRESHOLD);
    if (bit < 2) return (0);
    ix += 2;
    bits = (bits << 1) | (bit > 2);
  }
  return (bits);
}

void
NEXA::Receiver::sample()
{
  uint32_t start, stop;
  uint16_t us;
  uint16_t ix;

  do {
    while (is_low());
    stop = RTC::micros();
    for (ix = 0; ix < SAMPLE_MAX; ix++) {
      start = stop;
      while (is_high());
      stop = RTC::micros();
      us = stop - start;
      if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) break;
      m_sample[ix++] = us;
      start = stop;
      while (is_low());
      stop = RTC::micros();
      us = stop - start;
      if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) break;
      m_sample[ix] = us;
    }
  } while (ix != SAMPLE_MAX);
}

NEXA::code_t
NEXA::Receiver::read_code()
{ 
  uint32_t code;
  do {
    sample(); 
    code = decode();
  } while (code == m_code);
  m_code = code;
  return (code);
}

NEXA::Receiver receiver(Board::EXT0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("CosaNEXAreceiver: started"));
  TRACE(free_memory());
  RTC::begin();
  Watchdog::begin(16, SLEEP_MODE_IDLE, Watchdog::push_timeout_events);
  receiver.read_code().println(trace);
  receiver.enable();
}

void loop()
{
  Event event;
  Event::queue.await(&event);
  uint8_t type = event.get_type();
  Event::Handler* handler = event.get_target();
  if ((type == Event::READ_COMPLETED_TYPE) && (handler == &receiver)) {
    receiver.get_code().println(trace);
  }
}
