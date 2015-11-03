/**
 * @file BMP085.cpp
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

#include "BMP085.hh"
#include "Cosa/Watchdog.hh"

// Pressure convertion time depending on mode (pp. 10).
// Watchdog::delay() with a resolution of 16 ms is used.
const uint8_t BMP085::PRESSURE_CONV_MS[] __PROGMEM = {
  5,
  8,
  14,
  26
};

bool
BMP085::begin(Mode mode)
{
  // Set the operation mode
  m_mode = mode;

  // Read coefficients from the device
  twi.acquire(this);
  twi.write(COEFF_REG);
  twi.read(&m_param, sizeof(m_param));
  twi.release();

  // Adjust for little endien
  swap<param_t>(&m_param);
  return (true);
}

bool
BMP085::sample_temperature_request()
{
  // Check that a conversion request is not in process
  if (UNLIKELY(m_cmd != 0)) return (false);

  // Start a temperature measurement and wait
  m_cmd = TEMP_CONV_CMD;
  twi.acquire(this);
  twi.write(CMD_REG, &m_cmd, sizeof(m_cmd));
  twi.release();

  // Set start time for completion
  m_start = Watchdog::millis();
  return (true);
}

bool
BMP085::read_temperature()
{
  // Check that a temperature conversion request was issued
  if (UNLIKELY(m_cmd != TEMP_CONV_CMD)) return (false);
  m_cmd = 0;

  // Check if we need to wait for the conversion to complete
  int16_t ms = Watchdog::millis() - m_start + TEMP_CONV_MS;
  if (ms > 0) delay(ms);

  // Read the raw temperature sensor data
  int16_t UT;
  twi.acquire(this);
  twi.write(RES_REG);
  twi.read(&UT, sizeof(UT));
  twi.release();

  // Adjust for little endien
  UT = swap(UT);

  // Temperature calculation
  int32_t X1 = ((((int32_t) UT) - m_param.ac6) * m_param.ac5) >> 15;
  int32_t X2 = (((int32_t) m_param.mc) << 11) / (X1 + m_param.md);
  B5 = X1 + X2;
  return (true);
}

bool
BMP085::sample_pressure_request()
{
  // Check that a conversion request is not in process
  if (UNLIKELY(m_cmd != 0)) return (false);

  // Start a pressure measurement
  twi.acquire(this);
  m_cmd = PRESSURE_CONV_CMD + (m_mode << 6);
  twi.write(CMD_REG, &m_cmd, sizeof(m_cmd));
  twi.release();

  // Set start time for completion
  m_start = Watchdog::millis();
  return (true);
}

bool
BMP085::read_pressure()
{
  // Check that a conversion request was issued
  if (UNLIKELY(m_cmd != (PRESSURE_CONV_CMD + (m_mode << 6)))) return (false);
  m_cmd = 0;

  // Check if we need to wait for the conversion to complete
  int16_t ms = Watchdog::millis() - m_start;
  ms += pgm_read_byte(&PRESSURE_CONV_MS[m_mode]);
  if (ms > 0) delay(ms);

  // Read the raw pressure sensor data
  univ32_t res;
  res.as_uint8[0] = 0;
  twi.acquire(this);
  twi.write(RES_REG);
  twi.read(&res.as_uint8[1], 3);
  twi.release();

  // Adjust for little endian and resolution (oversampling mode)
  int32_t UP = swap(res.as_int32) >> (8 - m_mode);
  int32_t B3, B6, X1, X2, X3;
  uint32_t B4, B7;

  // Pressure calculation
  B6 = B5 - 4000;
  X1 = (m_param.b2 * ((B6 * B6) >> 12)) >> 11;
  X2 = (m_param.ac2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((((int32_t) m_param.ac1) << 2) + X3) << m_mode) + 2) >> 2;
  X1 = (m_param.ac3 * B6) >> 13;
  X2 = (m_param.b1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = (m_param.ac4 * (uint32_t) (X3 + 32768)) >> 15;
  B7 = ((uint32_t) UP - B3) * (50000 >> m_mode);
  m_pressure = (B7 < 0x80000000) ? (B7 << 1) / B4 : (B7 / B4) << 1;
  X1 = (m_pressure >> 8) * (m_pressure >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * m_pressure) >> 16;
  m_pressure += (X1 + X2 + 3791) >> 4;

  return (true);
}

IOStream&
operator<<(IOStream& outs, BMP085& bmp)
{
  outs << PSTR("BMP085(temperature = ") << bmp.temperature()
       << PSTR(", pressure = ") << bmp.pressure()
       << PSTR(")");
  return (outs);
}
