/**
 * @file Cosa/TWI/Driver/BMP085.cpp
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

#include "Cosa/TWI/Driver/BMP085.hh"

const uint16_t BMP085::PRESSURE_CONV_US[] __PROGMEM = {
  4500,
  7500,
  13500,
  25500
};

bool 
BMP085::begin()
{
  if (!twi.begin(this)) return (false);
  twi.write(COEFF_REG);
  int res = twi.read(&m_param, sizeof(m_param));
  twi.end();

  if (res != sizeof(m_param)) return (false);
  swap(&m_param.ac1, &m_param.ac1, sizeof(m_param) / 2);

  return (true);
}

bool 
BMP085::sample(int32_t& UT)
{
  uint8_t cmd = TEMP_CONV_CMD;
  if (!twi.begin(this)) return (false);
  twi.write(CMD_REG, &cmd, sizeof(cmd));
  twi.end();
  DELAY(TEMP_CONV_US);

  int16_t res;
  if (!twi.begin(this)) return (false);
  twi.write(RES_REG);
  twi.read(&res, sizeof(res));
  twi.end();
  res = swap(res);
  UT = (int32_t) res;
  return (true);
}
  
bool 
BMP085::sample(uint32_t& UP)
{
  uint8_t cmd = PRESSURE_CONV_CMD + (m_mode << 6);
  if (!twi.begin(this)) return (false);
  twi.write(CMD_REG, &cmd, sizeof(cmd));
  twi.end();
  DELAY(PRESSURE_CONV_US[m_mode]);

  univ32_t res;
  res.as_uint8[0] = 0;
  if (!twi.begin(this)) return (false);
  twi.write(RES_REG);
  twi.read(&res.as_uint8[1], 3);
  twi.end();
  UP = swap(res.as_int32);
  UP = UP >> (8 - m_mode);
  return (true);
}

int16_t 
BMP085::calculate(int32_t UT)
{
  int32_t X1, X2, B5;
  X1 = ((UT - ((int32_t) m_param.ac6)) * ((int32_t) m_param.ac5)) >> 15;
  X2 = (((int32_t) m_param.mc) << 11) / (X1 + ((int32_t) m_param.md));
  B5 = X1 + X2;
  return ((B5 + 8) >> 4);
}

uint32_t
BMP085::calculate(uint32_t UP, int32_t UT)
{
  int32_t B3, B5, B6, X1, X2, X3, pressure;
  uint32_t B4, B7;

  // Temperature calculation
  X1 = (((int32_t) UT - m_param.ac6) * ((int32_t) m_param.ac5)) >> 15;
  X2 = (((int32_t) m_param.mc) << 11) / (X1 + ((int32_t) m_param.md));
  B5 = X1 + X2;

  // Pressure calculation
  B6 = B5 - 4000;
  X1 = ((int32_t) m_param.b2 * ((B6 * B6) >> 12)) >> 11;
  X2 = ((int32_t) m_param.ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t) m_param.ac1*4 + X3) << m_mode) + 2) >> 2;

  X1 = ((int32_t) m_param.ac3 * B6) >> 13;
  X2 = ((int32_t) m_param.b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t) m_param.ac4 * (uint32_t) (X3 + 32768)) >> 15;
  B7 = ((uint32_t) UP - B3) * (uint32_t) (50000UL >> m_mode);

  if (B7 < 0x80000000UL) {
    pressure = (B7 * 2) / B4;
  } else {
    pressure = (B7 / B4) * 2;
  }
  X1 = (pressure >> 8) * (pressure >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * pressure) >> 16;
  
  pressure += ((X1 + X2 + 3791) >> 4);
  return (pressure);
}
