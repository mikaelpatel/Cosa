/**
 * @file Touch.cpp
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

#include "Touch.hh"
#include "Cosa/RTT.hh"

Touch::Touch(Job::Scheduler* scheduler, Board::DigitalPin pin, uint16_t threshold) :
  IOPin(pin),
  Periodic(scheduler, SAMPLE_RATE),
  THRESHOLD(threshold),
  m_sampling(false),
  m_touched(false)
{
  mode(OUTPUT_MODE);
  clear();
}

void
Touch::run()
{
  // Check if sampling should be initiated
  if (!m_sampling) {
    mode(INPUT_MODE);
    m_sampling = true;
    return;
  }

  // Sample the pin and discharge
  uint8_t state = is_clear();
  mode(OUTPUT_MODE);
  clear();
  m_sampling = false;

  // Was the pin discharge during the sampling period
  if (state) {
    m_start = RTT::millis();
    if (!m_touched) {
      on_touch();
      m_touched = true;
    }
    return;
  }

  // The pin was discharge; low-pass filter pin change
  if (m_touched && (RTT::since(m_start) > THRESHOLD)) {
    m_touched = false;
  }
}

