/**
 * @file Cosa/DS18B20.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Driver for the DS18B20 Programmable Resolution 1-Write
 * Digital Thermometer.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/DS18B20.h"

bool
DS18B20::convert_request()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::SKIP_ROM);
  _pin->write(CONVERT_T);
  return (1);
}

bool
DS18B20::read_scratchpad()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::SKIP_ROM);
  _pin->write(READ_SCRATCHPAD);
  _pin->begin();
  for (uint8_t i = 0; i < SCRATCHPAD_MAX; i++) {
    _scratchpad[i] = _pin->read();
  }
  return (_pin->end() == 0);
}

void 
DS18B20::print_scratchpad(IOStream& stream)
{
  for (uint8_t i = 0; i < SCRATCHPAD_MAX; i++) {
    stream.printf_P(PSTR("scratchpad[%d] = %hd\n"), i, _scratchpad[i]);
  }
}

uint16_t 
DS18B20::get_temperature()
{
  uint16_t temp = (_scratchpad[1] << 8) | _scratchpad[0];
  return (temp);
}

