/**
 * @file Cosa/OWI/DS18B20.cpp
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

#include "Cosa/OWI/DS18B20.hh"
#include "Cosa/FixedPoint.hh"

bool
DS18B20::convert_request()
{
  if (!match_rom()) return (0);
  m_pin->write(CONVERT_T);
  return (1);
}

bool
DS18B20::read_scratchpad()
{
  if (!match_rom()) return (0);
  m_pin->write(READ_SCRATCHPAD);
  m_pin->begin();
  uint8_t* ptr = (uint8_t*) &m_scratchpad;
  for (uint8_t i = 0; i < sizeof(m_scratchpad); i++) {
    *ptr++ = m_pin->read();
  }
#ifdef __DEBUG__
  print_scratchpad();
#endif
  return (m_pin->end() == 0);
}

void 
DS18B20::print_scratchpad(IOStream& stream)
{
  uint8_t* ptr = (uint8_t*) &m_scratchpad;
  for (uint8_t i = 0; i < sizeof(m_scratchpad); i++) {
    stream.printf_P(PSTR("scratchpad[%d] = %hd\n"), i, *ptr++);
  }
}

int16_t 
DS18B20::get_temperature()
{
  return (m_scratchpad.temperature);
}

void 
DS18B20::print_temperature_P(const char* prefix, IOStream& stream)
{
  FixedPoint temp(m_scratchpad.temperature, 4);
  int16_t integer = temp.get_integer();
  uint16_t fraction = temp.get_fraction(4);
  stream.printf_P(PSTR("%S%d.%s%d C"), prefix,
		  integer,
		  ((fraction != 0) && (fraction < 1000) ? "0" : ""),
		  fraction);
}
