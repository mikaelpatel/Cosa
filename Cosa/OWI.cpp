/**
 * @file Cosa/OWI.cpp
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
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI.hh"
#include "Cosa/RTC.hh"

bool
OWI::reset()
{
  uint8_t res = 0;
  uint8_t retry = 4;
  do {
    set_mode(OUTPUT_MODE);
    set();
    clear();
    DELAY(480);
    set();
    synchronized {
      set_mode(INPUT_MODE);
      DELAY(70);
      res = is_clear();
    }
  } while (retry-- && !res);
  DELAY(410);
  return (res != 0);
}

uint8_t
OWI::read(uint8_t bits)
{
  uint8_t res = 0;
  uint8_t mix = 0;
  uint8_t adjust = CHARBITS - bits;
  DELAY(5);
  while (bits--) {
    synchronized {
      set_mode(OUTPUT_MODE);
      set();
      clear();
      DELAY(6);
      set_mode(INPUT_MODE);
      DELAY(9);
      res >>= 1;
      if (is_set()) {
	res |= 0x80;
	mix = (m_crc ^ 1);
      }
      else {
	mix = (m_crc ^ 0);
      }
      m_crc >>= 1;
      if (mix & 1) m_crc ^= 0x8C;
      DELAY(55);
    }
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
  synchronized {
    set_mode(OUTPUT_MODE);
    set();
    DELAY(5);
    while (bits--) {
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
      value >>= 1;
      m_crc >>= 1;
      if (mix & 1) m_crc ^= 0x8C;
    }
    if (!power) power_off();
  }
  DELAY(10);
}

void 
OWI::write(uint8_t value, void* buf, uint8_t size)
{
  write(value);
  uint8_t* bp = (uint8_t*) buf;
  while (size--) write(*bp++);
}

OWI::Driver::Driver(OWI* pin, const uint8_t* rom) : 
  ROM(rom),
  m_pin(pin)
{
  if (rom != 0) eeprom_read_block(m_rom, rom, sizeof(m_rom));
}

bool 
OWI::Driver::update_rom()
{
  if (ROM == 0) return (false);
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
	} else if (m_rom[i] & (1 << j)) {
	  m_pin->write(1, 1);
	  data |= 0x80; 
	} 
	else {
	  m_pin->write(0, 1);
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
  if (!m_pin->reset()) return (false);
  m_pin->write(OWI::MATCH_ROM, m_rom, ROM_MAX);
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
      if (index == 0) return (true);
      index -= 1;
    }
  } while (last != LAST);
  memset(m_rom, 0, ROM_MAX);
  return (false);
}

int
OWI::Device::read(uint8_t bits)
{
  uint8_t res = 0;
  uint8_t mix = 0;
  uint8_t adjust = CHARBITS - bits;
  const uint8_t RETRY_MAX = 40;
  uint8_t retry = RETRY_MAX;
  synchronized {
    while (bits--) {
      retry = RETRY_MAX;
      do {
	while (is_set() && retry--) DELAY(1);
	if (retry == 0) synchronized_return (-1);
	DELAY(1);
      } while (is_set());
      DELAY(8);
      res >>= 1;
      if (is_set()) {
	res |= 0x80;
	mix = (m_crc ^ 1);
      }
      else {
	mix = (m_crc ^ 0);
      }
      m_crc >>= 1;
      if (mix & 1) m_crc ^= 0x8C;
      DELAY(40);
      retry = RETRY_MAX;
      do {
	while (is_clear() && retry--) DELAY(1);
	if (retry == 0) synchronized_return (-1);
	DELAY(1);
      } while (is_clear());
    }
  }
  res >>= adjust;
  return (res);
}

bool
OWI::Device::write(uint8_t value, uint8_t bits)
{
  uint8_t mix = 0;
  const uint8_t RETRY_MAX = 40;
  uint8_t retry = RETRY_MAX;
  synchronized {
    while (bits--) {
      retry = RETRY_MAX;
      do {
	while (is_set() && retry--) DELAY(1);
	if (retry == 0) synchronized_return (false);
	DELAY(1);
      } while (is_set());
      if (value & 1) {
	mix = (m_crc ^ 1);
	DELAY(45);
      }
      else {
	set_mode(OUTPUT_MODE);
	set();
	clear();
	DELAY(45);
	set_mode(INPUT_MODE);
	mix = (m_crc ^ 0);
      }
      value >>= 1;
      m_crc >>= 1;
      if (mix & 1) m_crc ^= 0x8C;
    }
  }
  return (true);
}

void 
OWI::Device::on_event(uint8_t type, uint16_t value)
{
  uint32_t stop = RTC::micros() + 440;

  static uint16_t req = 0;
  static uint16_t fns = 0;
  static uint16_t err = 0;

  // Complete the presence pulse
  req++;
  DELAY(200);
  set();
  set_mode(INPUT_MODE);
  synchronized {
    DELAY(stop - RTC::micros());

    m_state = ROM_STATE;
    int cmd = read(8);

    // Check for READ ROM command. Only possible when single device
    if (cmd == READ_ROM) {
      m_crc = 0;
      for (uint8_t i = 0; i < ROM_MAX - 1; i++)
	if (!write(m_rom[i], 8)) synchronized_goto(error);
      if (!write(m_crc, 8)) synchronized_goto(error);
    }

    else {

      // Check for SEARCH ROM command. Match slave device rom identity
      if (cmd == SEARCH_ROM) {
	for (uint8_t i = 0; i < ROM_MAX; i++) {
	  uint8_t bits = m_rom[i];
	  for (uint8_t j = 0; j < CHARBITS; j++) {
	    uint8_t bit = (bits & 0x01);
	    bit |= (~bit << 1);
	    if (!write(bit, 2)) synchronized_goto(error);
	    int value = read(1);
	    if (value != (bits & 0x01)) synchronized_goto(error);
	    bits >>= 1;
	  }
	}
      }

      // Check for MATCH ROM command. Match slave device rom identity
      else if (cmd == MATCH_ROM) {
	for (uint8_t i = 0; i < ROM_MAX - 1; i++)
	  if (read(8) != m_rom[i]) synchronized_goto(error);
	if (read(8) < 0) synchronized_goto(error);
      } 

      // Check for error commands
      else if (cmd != SKIP_ROM) synchronized_goto(error);

      // Get the function command
      m_state = FUNCTION_STATE;
      cmd = read(8);
      if (cmd < 0) synchronized_goto(error);
      fns++;

      // Check for STATUS function command. Return statistics
      if (cmd == STATUS) {
	m_crc = 0;
	write(req >> 8, 8);
	write(req, 8);
	write(fns >> 8, 8);
	write(fns, 8);
	write(err >> 8, 8);
	write(err, 8);
	write(m_crc, 8);
      }
    }
  }

 final:
  m_state = IDLE_STATE;
  enable();
  return;

 error:
  err++;
  goto final;
}

void 
OWI::Device::on_interrupt(uint16_t arg)
{
  volatile uint32_t now = RTC::micros();
  if (m_state == IDLE_STATE) {
    if (is_clear()) {
      m_time = now + 400L;
      m_state = RESET_STATE;
    }
  } 
  else if (m_state == RESET_STATE && now > m_time) {
    m_state = PRESENCE_STATE;
    m_time = now;
    disable();
    set_mode(OUTPUT_MODE);
    set();
    clear();
    Event::push(Event::CHANGE_TYPE, this);
  }
  else m_state = IDLE_STATE;
}

IOStream& operator<<(IOStream& outs, OWI::Driver& dev)
{
  uint8_t i;
  outs << PSTR("OWI::rom(family = ") << hex << dev.m_rom[0] << PSTR(", id = ");
  for (i = 1; i < OWI::ROM_MAX - 1; i++)
    outs << hex << dev.m_rom[i] << PSTR(", ");
  outs << PSTR("crc = ") << hex << dev.m_rom[i] << ')';
  return (outs);
}

IOStream& operator<<(IOStream& outs, OWI& owi)
{
  OWI::Driver dev(&owi);
  int8_t last = OWI::Driver::FIRST;
  do {
    last = dev.search_rom(last);
    if (last == OWI::Driver::ERROR) return (outs);
    outs << dev << endl;
  } while (last != OWI::Driver::LAST);
  return (outs);
}
