/**
 * @file MPU6050.cpp
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

#include "MPU6050.hh"

void
MPU6050::write(Register reg, uint8_t value)
{
  twi.acquire(this);
  twi.write((uint8_t) reg, &value, sizeof(value));
  twi.release();
}

void
MPU6050::write(Register reg, void* buffer, size_t count)
{
  twi.acquire(this);
  twi.write((uint8_t) reg, buffer, count);
  twi.release();
}

uint8_t
MPU6050::read(Register reg)
{
  uint8_t res;
  twi.acquire(this);
  twi.write((uint8_t) reg);
  twi.read(&res, sizeof(res));
  twi.release();
  return (res);
}

void
MPU6050::read(Register reg, void* buffer, size_t count)
{
  twi.acquire(this);
  twi.write((uint8_t) reg);
  twi.read(buffer, count);
  twi.release();
}

bool
MPU6050::begin(uint8_t clksel)
{
  // Sanity check the identity register value
  if (read(WHO_AM_I) != 0x68) return (false);

  // Set clock source (default PLL with X axis gyroscope reference)
  pwr_mgmt_1_t pwr;
  pwr.CLKSEL = clksel;
  write(PWR_MGMT_1, pwr);

  // Set accelerometer full range (default +/-2 g)
  accel_config_t accel;
  accel.AFS_SEL = AFS_RANGE_2G;
  write(ACCEL_CONFIG, accel);

  // Set gyroscope full range (default +/-250 dps)
  gyro_config_t gyro;
  gyro.FS_SEL = FS_RANGE_250;
  write(GYRO_CONFIG, gyro);

  return (true);
}

bool
MPU6050::end()
{
  return (true);
}

int16_t
MPU6050::read_temperature()
{
  int16_t temperature;
  read(TEMP_OUT, &temperature, sizeof(temperature));
  return (swap(temperature) + 12410) / 34;
}

void
MPU6050::read_motion(motion_t& m)
{
  read(ACCEL_OUT, &m, sizeof(m));
  swap<motion_t>(&m);
  m.temp = (m.temp + 12410) / 34;
}

void
MPU6050::read_accelerometer(sample_t& s)
{
  read(ACCEL_OUT, &s, sizeof(s));
  swap<sample_t>(&s);
}

void
MPU6050::read_gyroscope(sample_t& s)
{
  read(GYRO_OUT, &s, sizeof(s));
  swap<sample_t>(&s);
}

IOStream&
operator<<(IOStream& outs, MPU6050& mpu)
{
  MPU6050::motion_t value;
  mpu.read_motion(value);
  outs << PSTR("MPU6050::accelerometer(x = ") << value.accel.x
       << PSTR(", y = ") << value.accel.y
       << PSTR(", z = ") << value.accel.z
       << PSTR(")") << endl;
  outs << PSTR("MPU6050::temperature = ") << value.temp << endl;
  outs << PSTR("MPU6050::gyroscope(x = ") << value.gyro.x
       << PSTR(", y = ") << value.gyro.y
       << PSTR(", z = ") << value.gyro.z
       << PSTR(")") << endl;
  return (outs);
}
