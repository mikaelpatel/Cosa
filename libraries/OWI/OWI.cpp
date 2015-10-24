/**
 * @file OWI.cpp
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
 * @section References
 * The timing of 1-Wire read/write operations are according to
 * AVR318: Dallas 1-Wire(R) master, Rev. 2579A-AVR-09/04,
 * Table 3. Bit transfer layer delays.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "OWI.hh"

bool
OWI::reset()
{
  uint8_t retry = 4;
  uint8_t res = 0;
  do {
    mode(OUTPUT_MODE);
    set();
    clear();
    DELAY(480);
    set();
    synchronized {
      mode(INPUT_MODE);
      DELAY(70);
      res = is_clear();
    }
    DELAY(410);
  } while (retry-- && !res);
  return (res != 0);
}

uint8_t
OWI::read(uint8_t bits)
{
  uint8_t res = 0;
  uint8_t mix = 0;
  uint8_t adjust = CHARBITS - bits;
  while (bits--) {
    synchronized {
      mode(OUTPUT_MODE);
      set();
      clear();
      DELAY(6);
      mode(INPUT_MODE);
      DELAY(9);
      res >>= 1;
      if (is_set()) {
	res |= 0x80;
	mix = (m_crc ^ 1);
      }
      else {
	mix = (m_crc ^ 0);
      }
    }
    m_crc >>= 1;
    if (mix & 1) m_crc ^= 0x8C;
    DELAY(55);
  }
  res >>= adjust;
  return (res);
}

bool
OWI::read(void* buf, uint8_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  m_crc = 0;
  while (size--) *bp++ = read();
  return (m_crc == 0);
}

void
OWI::write(uint8_t value, uint8_t bits, bool power)
{
  uint8_t mix = 0;
  mode(OUTPUT_MODE);
  set();
  while (bits--) {
    synchronized {
      clear();
      if (value & 1) {
	DELAY(6);
	set();
	DELAY(64);
	mix = (m_crc ^ 1);
      }
      else {
	DELAY(60);
	set();
	DELAY(10);
	mix = (m_crc ^ 0);
      }
    }
    value >>= 1;
    m_crc >>= 1;
    if (mix & 1) m_crc ^= 0x8C;
  }
  if (!power) power_off();
}

void
OWI::write(uint8_t value, void* buf, uint8_t size)
{
  write(value);
  uint8_t* bp = (uint8_t*) buf;
  while (size--) write(*bp++);
}

OWI::Driver*
OWI::lookup(uint8_t* rom)
{
  for (Driver* dev = m_device; dev != NULL; dev = dev->m_next) {
    if (!memcmp(dev->m_rom, rom, ROM_MAX))
      return (dev);
  }
  return (NULL);
}

bool
OWI::alarm_dispatch()
{
  OWI::Search iter(this);
  OWI::Driver* dev = iter.next();
  if (UNLIKELY(dev == NULL)) return (false);
  do {
    dev->on_alarm();
    dev = iter.next();
  } while (dev != NULL);
  return (true);
}

IOStream& operator<<(IOStream& outs, OWI& owi)
{
  OWI::Driver dev(&owi);
  int8_t last = OWI::Driver::FIRST;
  do {
    last = dev.search_rom(last);
    if (UNLIKELY(last == OWI::Driver::ERROR)) return (outs);
    outs << dev << endl;
  } while (last != OWI::Driver::LAST);
  return (outs);
}

OWI::Driver::Driver(OWI* pin, const uint8_t* rom, const char* name) :
  NAME((str_P) name),
  ROM(rom),
  m_next(NULL),
  m_pin(pin)
{
  eeprom_read_block(m_rom, rom, sizeof(m_rom));
  m_pin->m_devices += 1;
  m_next = m_pin->m_device;
  m_pin->m_device = this;
}

bool
OWI::Driver::update_rom()
{
  if (UNLIKELY(ROM == NULL)) return (false);
  eeprom_write_block(ROM, m_rom, sizeof(m_rom));
  return (true);
}

int8_t
OWI::Driver::search(int8_t last)
{
  uint8_t pos = 0;
  int8_t next = LAST;
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t data = 0;
    for (uint8_t j = 0; j < 8; j++) {
      data >>= 1;
      switch (m_pin->read(2)) {
      case 0b00: // Discrepency between device roms
	if (pos == last) {
	  m_pin->write(1, 1);
	  data |= 0x80;
	  last = FIRST;
	}
	else if (pos > last) {
	  m_pin->write(0, 1);
	  next = pos;
	}
	else if (m_rom[i] & (1 << j)) {
	  m_pin->write(1, 1);
	  data |= 0x80;
	}
	else {
	  m_pin->write(0, 1);
	  next = pos;
	}
	break;
      case 0b01: // Only one's at this position
	m_pin->write(1, 1);
	data |= 0x80;
	break;
      case 0b10: // Only zero's at this position
	m_pin->write(0, 1);
	break;
      case 0b11: // No device detected
	return (ERROR);
      }
      pos += 1;
    }
    m_rom[i] = data;
  }
  return (next);
}

int8_t
OWI::Driver::search_rom(int8_t last)
{
  if (!m_pin->reset()) return (ERROR);
  m_pin->write(OWI::SEARCH_ROM);
  return (search(last));
}

bool
OWI::Driver::read_rom()
{
  if (!m_pin->reset()) return (false);
  m_pin->write(OWI::READ_ROM);
  return (m_pin->read(m_rom, ROM_MAX));
}

bool
OWI::Driver::match_rom()
{
  if (m_rom[0] == 0) return (false);
  if (!m_pin->reset()) return (false);
  if (m_pin->m_devices > 1)
    m_pin->write(OWI::MATCH_ROM, m_rom, ROM_MAX);
  else
    m_pin->write(OWI::SKIP_ROM);
  return (true);
}

bool
OWI::Driver::skip_rom()
{
  if (!m_pin->reset()) return (false);
  m_pin->write(OWI::SKIP_ROM);
  return (true);
}

int8_t
OWI::Driver::alarm_search(int8_t last)
{
  if (!m_pin->reset()) return (ERROR);
  m_pin->write(OWI::ALARM_SEARCH);
  return (search(last));
}

bool
OWI::Driver::connect(uint8_t family, uint8_t index)
{
  int8_t last = FIRST;
  do {
    last = search_rom(last);
    if (last == ERROR) return (false);
    if (m_rom[0] == family) {
      if (index == 0) {
	m_pin->m_devices += 1;
	m_next = m_pin->m_device;
	m_pin->m_device = this;
	return (true);
      }
      index -= 1;
    }
  } while (last != LAST);
  memset(m_rom, 0, ROM_MAX);
  return (false);
}

IOStream& operator<<(IOStream& outs, OWI::Driver& dev)
{
  uint8_t i;
  if (dev.NAME != NULL) outs << dev.NAME << ':';
  outs << PSTR("OWI::rom(family = ") << hex << dev.m_rom[0]
       << PSTR(", id = 0x");
  for (i = OWI::ROM_MAX - 2; i > 0; i--) {
    uint8_t v = dev.m_rom[i];
    outs << tohex(v >> 4) << tohex(v);
  }
  outs << PSTR(", crc = ") << hex << dev.m_rom[OWI::ROM_MAX - 1] << ')';
  return (outs);
}

OWI::Driver*
OWI::Search::next()
{
  do {
    if (m_last == LAST) return (NULL);
    m_last = alarm_search(m_last);
    if (m_last == ERROR) return (NULL);
  } while ((m_family != 0) && (m_rom[0] != m_family));
  return (m_pin->lookup(m_rom));
}

