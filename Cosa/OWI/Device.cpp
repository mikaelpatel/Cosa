/**
 * @file Cosa/OWI/Device.cpp
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
 * 1-Wire slave device support class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OWI.hh"
#include "Cosa/RTC.hh"

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

  // Complete the precense pulse
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
