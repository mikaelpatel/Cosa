/**
 * @file Cosa/TWI/Driver/HMC5883L.cpp
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

#include "Cosa/TWI/Driver/HMC5883L.hh"

const uint16_t HMC5883L::s_gain[] __PROGMEM = {
  1370,
  1090,
  820,
  660,
  440,
  390,
  330,
  230
};

bool
HMC5883L::begin()
{
  // Read the device identity register
  uint8_t id[3];
  twi.begin(this);
  twi.write((uint8_t) IDENTITY);
  twi.read(id, sizeof(id));
  twi.end();

  // Sanity check the identity
  static const uint8_t ID[3] __PROGMEM = { 'H', '4', '3' };
  if (memcmp_P(id, ID, sizeof(ID))) return (false);

  // Write configuration
  return (write_config());
}

bool
HMC5883L::write_config()
{
  twi.begin(this);
  int count = twi.write((uint8_t) CONFIG, &m_config, sizeof(m_config));
  twi.end();
  return (count == (sizeof(m_config) + 1));
}

bool
HMC5883L::set_mode(Mode mode)
{
  twi.begin(this);
  int count = twi.write((uint8_t) MODE, &mode, sizeof(mode));
  twi.end();
  return (count == (sizeof(mode) + 1));
}

bool
HMC5883L::read_status(status_t& status)
{
  twi.begin(this);
  twi.write((uint8_t) STATUS);
  int count = twi.read(&status, sizeof(status));
  twi.end();
  return (count == sizeof(status));
}

bool
HMC5883L::read_heading()
{
  // Read output data from the device
  twi.begin(this);
  twi.write((uint8_t) OUTPUT);
  int count = twi.read(&m_output, sizeof(m_output));
  twi.end();
  if (count != sizeof(m_output)) return (false);

  // Adjust to little endian
  swap<data_t>(&m_output);

  // Check if an overflow occured
  m_overflow =
    (m_output.x == -4096) ||
    (m_output.y == -4096) ||
    (m_output.z == -4096);
  return (true);
}

void
HMC5883L::to_milli_gauss()
{
  // Do not scale if overflow
  if (is_overflow()) return;

  // Scale with the current gain setting
  uint16_t gain = pgm_read_word(&s_gain[m_config.GN]);
  m_output.x = (1000L * m_output.x) / gain;
  m_output.y = (1000L * m_output.y) / gain;
  m_output.z = (1000L * m_output.z) / gain;
}

IOStream&
operator<<(IOStream& outs, HMC5883L& compass)
{
  if (compass.is_overflow()) {
    outs << PSTR("HMC5883L(overflow)");
  }
  else {
    HMC5883L::data_t value;
    compass.get_heading(value);
    outs << PSTR("HMC5883L(x = ") << value.x
	 << PSTR(", y = ") << value.y
	 << PSTR(", z = ") << value.z
	 << PSTR(")");
  }
  return (outs);
}
