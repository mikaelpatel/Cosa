/**
 * @file Cosa/OWI/Driver/DS18B20.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Driver for the DS18B20 Programmable Resolution 1-Write Digital
 * Thermometer.  
 * 
 * See Maxim Integrated product description (REV: 042208) 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI/Driver/DS18B20.hh"

uint16_t 
DS18B20::set_resolution(uint8_t bits)
{
  if (bits < 9) bits = 9; else if (bits > 12) bits = 12;
  m_scratchpad.configuration = (((bits - 9) << 5) | 0x1f);
  return (MAX_CONVERSION_TIME >> (12 - bits));
}

bool
DS18B20::convert_request()
{
  if (!match_rom()) return (false);
  m_pin->write(CONVERT_T);
  return (true);
}

bool
DS18B20::read_scratchpad()
{
  if (!match_rom()) return (false);
  m_pin->write(READ_SCRATCHPAD);
  m_pin->begin();
  uint8_t* ptr = (uint8_t*) &m_scratchpad;
  for (uint8_t i = 0; i < sizeof(m_scratchpad); i++) {
    *ptr++ = m_pin->read();
  }
  return (m_pin->end() == 0);
}

bool
DS18B20::write_scratchpad()
{
  if (!match_rom()) return (false);
  m_pin->write(WRITE_SCRATCHPAD);
  uint8_t* ptr = (uint8_t*) &m_scratchpad.high_trigger;
  const uint8_t CONFIG_MAX = 3;
  for (uint8_t i = 0; i < CONFIG_MAX; i++) {
    m_pin->write(*ptr++);
  }
  return (true);
}

bool
DS18B20::copy_scratchpad()
{
  if (!match_rom()) return (false);
  m_pin->write(COPY_SCRATCHPAD);
  return (true);
}

bool
DS18B20::recall()
{
  if (!match_rom()) return (false);
  m_pin->write(RECALL_E);
  return (true);
}
