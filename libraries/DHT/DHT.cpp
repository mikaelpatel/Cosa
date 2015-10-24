/**
 * @file DHT.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "DHT.hh"
#include "Cosa/RTT.hh"
#include "Cosa/Watchdog.hh"

/*
 * The interrupt handler, enabled after the request pulse, and on
 * falling (low to high) transition. This allows lower interrupt
 * frequency than on change mode (which would be required for pin
 * change interrupts. First pulse is the device response and a
 * one(1) bit is encoded as a long pulse (54 + 80 = 134 us), a
 * zero(0) bit as a short pulse (54 + 24 = 78 us). Sequence ends
 * with a low pulse (54 us) which allows falling/rising detection.
 */
void
DHT::on_interrupt(uint16_t arg)
{
  UNUSED(arg);

  // Calculate the pulse width and check against thresholds
  uint16_t stop = RTT::micros();
  uint16_t us = (stop - m_start);
  bool valid = false;

  // Check the initial response pulse
  if (m_state == RESPONSE) {
    if (us < BIT_THRESHOLD) goto exception;
    m_state = SAMPLING;
    m_start = stop;
    m_bits = 0;
    m_ix = 0;
    return;
  }

  // Sanity check the pulse length
  if (us < LOW_THRESHOLD || us > HIGH_THRESHOLD) goto exception;
  m_start = stop;

  // Sample was valid, collect bit and check for more
  m_value = (m_value << 1) + (us > BIT_THRESHOLD);
  m_bits += 1;
  if (m_bits != CHARBITS) return;

  // Next byte ready
  m_data.as_byte[m_ix++] = m_value;
  m_bits = 0;
  valid = (m_ix == DATA_MAX);
  if (valid) goto completed;
  return;

  // Invalid sample reject sequence
 exception:
  m_ix = 0;

  // Sequence completed
 completed:
  m_state = COMPLETED;
  disable();
  if (valid) adjust_data();
  on_sample_completed(valid);
}

bool
DHT::sample_request()
{
  // Issue a request; pull down for more than 18 ms
  mode(OUTPUT_MODE);
  IOPin::clear();
  Watchdog::delay(32);

  // Request pulse completed; pull up for 40 us and collect
  // data as a sequence of on rising mode interrupts
  m_state = RESPONSE;
  m_start = RTT::micros();
  IOPin::set();
  mode(INPUT_MODE);
  DELAY(40);
  enable();
  return (true);
}

bool
DHT::sample_await()
{
  // Wait for the sample request to complete
  uint32_t start = RTT::millis();
  while (m_state != COMPLETED && (RTT::since(start) < MIN_PERIOD))
    yield();
  if (m_state != COMPLETED) return (false);

  // Data reading was completed; validate data and check sum
  m_state = INIT;
  return (is_valid());
}

bool
DHT::is_valid()
{
  if (m_ix != DATA_MAX) return (false);
  uint8_t sum = 0;
  for (uint8_t i = 0; i < DATA_LAST; i++)
    sum += m_data.as_byte[i];
  return (sum == m_data.chksum);
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
