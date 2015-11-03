/**
 * @file DS18B20.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "DS18B20.hh"
#include "Cosa/Watchdog.hh"

DS18B20*
DS18B20::Search::next()
{
  DS18B20* dev = (DS18B20*) OWI::Search::next();
  if (UNLIKELY(dev == NULL)) return (NULL);
  dev->read_scratchpad(false);
  return (dev);
}

bool
DS18B20::connect(uint8_t index)
{
  if (!OWI::Driver::connect(FAMILY_CODE, index)) return (false);
  if (!read_scratchpad(false)) return (false);
  read_power_supply();
  return (true);
}

void
DS18B20::resolution(uint8_t bits)
{
  if (bits < 9) bits = 9; else if (bits > 12) bits = 12;
  m_scratchpad.configuration = (((bits - 9) << 5) | 0x1f);
}

bool
DS18B20::convert_request()
{
  if (!match_rom()) return (false);
  m_pin->write(CONVERT_T, CHARBITS, m_parasite);
  m_start = Watchdog::millis();
  m_converting = true;
  return (true);
}

bool
DS18B20::convert_request(OWI* owi, uint8_t resolution, bool parasite)
{
  if (!owi->reset()) return (false);
  owi->write(OWI::SKIP_ROM);
  owi->write(CONVERT_T, CHARBITS, parasite);
  if (resolution == 0) return (true);
  if (resolution < 9) resolution = 9;
  else if (resolution > 12) resolution = 12;
  uint16_t ms = (MAX_CONVERSION_TIME >> (12 - resolution));
  delay(ms);
  if (parasite) owi->power_off();
  return (true);
}

bool
DS18B20::read_scratchpad(bool flag)
{
  if (m_converting) {
    int32_t ms = Watchdog::millis() - m_start;
    uint16_t conv_time = (MAX_CONVERSION_TIME >> (12 - resolution()));
    if (ms < conv_time) {
      ms = conv_time - ms;
      delay(ms);
    }
    m_converting = false;
    power_off();
  }
  if (flag && !match_rom()) return (false);
  m_pin->write(READ_SCRATCHPAD);
  return (m_pin->read(&m_scratchpad, sizeof(m_scratchpad)));
}

bool
DS18B20::write_scratchpad()
{
  if (!match_rom()) return (false);
  m_pin->write(WRITE_SCRATCHPAD, &m_scratchpad.high_trigger, CONFIG_MAX);
  return (true);
}

bool
DS18B20::copy_scratchpad()
{
  if (!match_rom()) return (false);
  m_pin->write(COPY_SCRATCHPAD, CHARBITS, m_parasite);
  delay(MIN_COPY_PULLUP);
  power_off();
  return (true);
}

bool
DS18B20::recall()
{
  if (!match_rom()) return (false);
  m_pin->write(RECALL_E);
  return (true);
}

bool
DS18B20::read_power_supply()
{
  if (!match_rom()) return (false);
  m_pin->write(READ_POWER_SUPPLY);
  m_parasite = (m_pin->read(1) == 0);
  return (m_parasite);
}

void
DS18B20::print(IOStream& outs, int16_t temp)
{
  if (temp < 0) {
    temp = -temp;
    outs << '-';
  }
  uint16_t fraction = (625 * (temp & 0xf)) / 100;
  int16_t integer = (temp >> 4);
  outs << integer << '.';
  if (fraction < 10) outs << '0';
  outs << fraction;
}

IOStream& operator<<(IOStream& outs, DS18B20& thermometer)
{
  if (thermometer.NAME != NULL) outs << thermometer.NAME << PSTR(" = ");
  DS18B20::print(outs, thermometer.temperature());
  return (outs);
}

