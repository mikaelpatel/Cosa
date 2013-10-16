/**
 * @file Cosa/Driver/DHT.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Driver/DHT.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Watchdog.hh"

// The interrupt handler, enabled after the request pulse, and
// on falling (low to high) transition. This allows lower interrupt
// frequency than on change mode (which would be required for
// pin change interrupts. First pulse is the device response and
// is a one(1) bit is encoded as a long pulse (54 + 80 = 134 us), 
// a zero(0) bit as a short pulse (54 + 24 = 78 us). Sequence ends
// with a low pulse (54 us) which allows falling/rising detection.
void 
DHT::on_interrupt(uint16_t arg) 
{ 
  // Check start condition
  if (m_start == 0) {
    m_start = RTC::micros();
    return;
  }

  // Calculate the pulse width and check against thresholds
  uint16_t stop = RTC::micros();
  uint16_t us = (stop - m_start);
  if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) goto exception;
  m_start = stop;

  // Check the initial response pulse
  if (m_state == RESPONSE) {
    if (us < BIT_THRESHOLD) goto exception;
    m_state = SAMPLING;
    m_bits = 0;
    m_ix = 0;
    return;
  }

  // Sample was valid, collect data
  m_value = (m_value << 1) + (us > BIT_THRESHOLD);
  m_bits += 1;
  if (m_bits != CHARBITS) return;

  // Next byte ready
  m_data.as_byte[m_ix++] = m_value;
  m_bits = 0;
  if (m_ix == DATA_MAX) goto completed;
  return;

  // Invalid sample reject sequence
 exception:
  m_start = 0;
  m_ix = 0;

  // Sequence completed
 completed:
  m_state = COMPLETED;
  disable();
  if (m_period == 0) return;
  Event::push(Event::SAMPLE_COMPLETED_TYPE, this);
}

void 
DHT::on_event(uint8_t type, uint16_t value)
{
  switch (m_state) {

  case IDLE: 
    // Issue a request; pull down for more than 18 ms
    m_state = REQUEST;
    set_mode(OUTPUT_MODE);
    clear();
    Watchdog::attach(this, 32);
    break;

  case REQUEST: 
    // Request pulse completed; pull up for 40 us and collect
    // data as a sequence of on rising mode interrupts
    m_state = RESPONSE;
    m_start = 0;
    detach();
    set();
    set_mode(INPUT_MODE);
    DELAY(40);
    enable();
    break;

  case COMPLETED:
    // Data reading was completed; validate data and check sum
    // Wait before issueing the next request
    uint8_t invalid = 1;
    if (m_ix == DATA_MAX) {
      uint8_t sum = 0;
      for (uint8_t i = 0; i < DATA_LAST; i++) 
	sum += m_data.as_byte[i];
      if (sum == m_data.chksum) {
	invalid = 0;
	adjust_data();
      }
      on_sample_completed();
    }
    m_errors += invalid;
    m_state = IDLE;
    Watchdog::attach(this, m_period);
    break;
  }
}

void
DHT::begin(uint16_t ms)
{
  if (m_state != INIT) return;
  if (ms < MIN_PERIOD) ms = MIN_PERIOD;
  m_state = IDLE;
  m_period = ms;
  Watchdog::attach(this, m_period);
}

void
DHT::end()
{
  disable();
  detach();
  m_state = INIT;
  m_period = 0;
}

bool
DHT::sample_request()
{
  // Issue a request; pull down for more than 18 ms
  if (m_period != 0) return (true);
  set_mode(OUTPUT_MODE);
  set();
  clear();
  Watchdog::delay(32);

  // Request pulse completed; pull up for 40 us and collect
  // data as a sequence of on rising mode interrupts
  set();
  set_mode(INPUT_MODE);
  DELAY(40);
  m_state = RESPONSE;
  m_start = 0;
  enable();
  return (true);
}

bool
DHT::sample_await(uint8_t mode)
{
  if (m_period != 0) return (true);
  uint32_t start = RTC::millis();
  while (m_state != COMPLETED && RTC::since(start) < MIN_PERIOD) 
    Power::sleep(mode);
  if (m_state != COMPLETED) return (false);

  // Data reading was completed; validate data and check sum
  m_state = INIT;
  if (m_ix != DATA_MAX) return (false);
  uint8_t sum = 0;
  for (uint8_t i = 0; i < DATA_LAST; i++) 
    sum += m_data.as_byte[i];
  if (sum != m_data.chksum) return (false);
  adjust_data();
  return (true);
}

void 
DHT11::adjust_data()
{
  m_humidity = m_data.humidity * 10;
  m_temperature = m_data.temperature * 10;
}

void
DHT22::adjust_data() 
{
  m_humidity = swap(m_data.humidity);
  m_temperature = swap(m_data.temperature);
  if (m_temperature >= 0) return;
  m_temperature = -(m_temperature & 0x7fff);
}

IOStream& 
operator<<(IOStream& outs, DHT& dht)
{
  outs << PSTR("RH = ") << dht.m_humidity / 10
       << '.' << dht.m_humidity % 10
       << PSTR("%, T = ") << dht.m_temperature / 10
       << '.' << dht.m_temperature % 10
       << PSTR(" C");
  return (outs);
}
