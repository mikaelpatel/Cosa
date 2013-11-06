/**
 * @file Cosa/TWI/Driver/MPU6050.cpp
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

#include "Cosa/TWI/Driver/MPU6050.hh"

void 
MPU6050::write(Register reg, uint8_t value)
{
  twi.begin(this);
  twi.write((uint8_t) reg, &value, sizeof(value));
  twi.end();
}

void 
MPU6050::write(Register reg, void* buffer, size_t count)
{
  twi.begin(this);
  twi.write((uint8_t) reg, buffer, count);
  twi.end();
}

uint8_t 
MPU6050::read(Register reg)
{
  uint8_t res;
  twi.begin(this);
  twi.write((uint8_t) reg); 
  twi.read(&res, sizeof(res));
  twi.end();
  return (res);
}

void 
MPU6050::read(Register reg, void* buffer, size_t count)
{
  twi.begin(this);
  twi.write((uint8_t) reg); 
  twi.read(buffer, count);
  twi.end();
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
  swap(&m.accel.x, &m.accel.x, sizeof(m) / sizeof(int16_t));
  m.temp = (m.temp + 12410) / 34;
}
  
void 
MPU6050::read_accelerometer(sample_t& s)
{
  read(ACCEL_OUT, &s, sizeof(s));
  swap(&s.x, &s.x, sizeof(s) / sizeof(int16_t));
}
  
void 
MPU6050::read_gyroscope(sample_t& s)
{
  read(GYRO_OUT, &s, sizeof(s));
  swap(&s.x, &s.x, sizeof(s) / sizeof(int16_t));
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
