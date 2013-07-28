/**
 * @file Cosa/TWI/Driver/HMC5883L.cpp
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

#include "Cosa/TWI/Driver/HMC5883L.hh"

bool
HMC5883L::begin()
{
  if (!twi.begin()) return (false);
  twi.write(ADDR, (uint8_t) 0);
  int count = twi.read(ADDR, &m_reg, sizeof(m_reg));
  twi.end();
  return (count == sizeof(m_reg));
}

bool
HMC5883L::end()
{
  return (set_mode(IDLE_MEASUREMENT_MODE));
}

void
HMC5883L::set_bias(Bias bias)
{
  m_reg.config[0] = (m_reg.config[0] & ~BIAS_MASK) | bias;
}

void 
HMC5883L::set_output_rate(Rate rate)
{
  m_reg.config[0] = (m_reg.config[0] & ~OUTPUT_RATE_MASK) | rate;
}

void
HMC5883L::set_sample_avg(Avg avg)
{
  m_reg.config[0] = (m_reg.config[0] & ~SAMPLES_AVG_MASK) | avg;
}

void
HMC5883L::set_range(Range range)
{
  m_reg.config[1] = (m_reg.config[1] & ~RANGE_MASK) | range;
}

bool
HMC5883L::write_config()
{
  if (!twi.begin()) return (false);
  int count = twi.write(ADDR, (uint8_t) offsetof(reg_t, config), 
			&m_reg.config, sizeof(m_reg.config));
  twi.end();
  return (count == (sizeof(m_reg.config) + 1));
}

bool
HMC5883L::set_mode(Mode mode)
{
  m_reg.mode = (m_reg.mode & ~MEASUREMENT_MODE_MASK) | mode;
  if (!twi.begin()) return (false);
  int count = twi.write(ADDR, (uint8_t) offsetof(reg_t, mode), 
			&m_reg.mode, sizeof(m_reg.mode));
  twi.end();
  return (count == (sizeof(m_reg.mode) + 1));
}

bool 
HMC5883L::read_status(Status& status)
{
  if (!twi.begin()) return (false);
  twi.write(ADDR, (uint8_t) offsetof(reg_t, status));
  int count = twi.read(ADDR, &m_reg.status, sizeof(m_reg.status));
  twi.end();
  status = (Status) m_reg.status;
  return (count == sizeof(m_reg.status));
}

bool 
HMC5883L::read_data(data_t& data)
{
  if (!twi.begin()) return (false);
  twi.write(ADDR, (uint8_t) offsetof(reg_t, output));
  int count = twi.read(ADDR, &m_reg.output, sizeof(m_reg.output));
  twi.end();
  data.x = swap(m_reg.output.x);
  data.y = swap(m_reg.output.y);
  data.z = swap(m_reg.output.z);
  return (count == sizeof(m_reg.output));
}

