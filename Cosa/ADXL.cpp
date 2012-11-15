/**
 * @file Cosa/ADXL.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * Analog Devices ADXL345 Digital Accelerometer SPI driver.
 * http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "ADXL.h"

ADXL::ADXL(uint8_t ss) : 
  SPI(), 
  _ss(ss, 1) 
{
  begin(SPI::DIV4_CLOCK, 3, SPI::MSB_FIRST);
  write(DATA_FORMAT, _BV(FULL_RES) | RANGE_16G);
  write(POWER_CTL, _BV(MEASURE));
  calibrate(0, 0, 0);
}

void 
ADXL::write(Register reg, uint8_t value)
{
  SPI_transaction(_ss) {
    SPI::write(WRITE_CMD | (reg & REG_MASK), value);
  }
}

void 
ADXL::write(Register reg, void* buffer, uint8_t count)
{
  SPI_transaction(_ss) {
    SPI::write(WRITE_CMD | MULTIPLE_BYTE | (reg & REG_MASK), buffer, count);
  }
}

uint8_t 
ADXL::read(Register reg)
{
  uint8_t res;
  SPI_transaction(_ss) {
    res = SPI::read(READ_CMD | (reg & REG_MASK));
  }
  return (res);
}

void 
ADXL::read(Register reg, void* buffer, uint8_t count)
{
  SPI_transaction(_ss) {
    SPI::read(READ_CMD | MULTIPLE_BYTE | (reg & REG_MASK), buffer, count);
  }
}

void 
ADXL::calibrate(int8_t x, int8_t y, int8_t z)
{
  offset_t ofs; 
  ofs.x = x;
  ofs.y = y;
  ofs.z = z;
  write(OFS, &ofs, sizeof(ofs));
}

void 
ADXL::sample(sample_t& s)
{
  read(DATA, &s, sizeof(s));
}

void
ADXL::calibrate()
{
  calibrate(0, 0, 0);
  sample_t s;
  sample(s);
  calibrate(-s.x/4, -s.y/4, -s.z/4);
}

void 
ADXL::sample_t::print()
{
#ifndef NDEBUG
  Serial_print("ADXL::sample_t(");
  Serial.print(x);
  Serial_print(", ");
  Serial.print(y);
  Serial_print(", ");
  Serial.print(z);
  Serial_print(")");
#endif
}

void 
ADXL::sample_t::println()
{
#ifndef NDEBUG
  print();
  Serial_print("\n");
#endif
}


