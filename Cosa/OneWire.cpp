/**
 * @file Cosa/OneWire.cpp
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
 * 1-Wire device driver support class.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OneWire.h"
#include <util/delay_basic.h>

#define DELAY(us) _delay_loop_2((us) << 2)

bool
OneWire::reset()
{
  uint8_t retry = 64;
  uint8_t res = 0;

  // Check that the bus is not active
  set_mode(INPUT_MODE);
  while (is_clear() && retry--) DELAY(10);
  if (retry == 0) return (0);

  // Generate the reset signal
  set_mode(OUTPUT_MODE);
  clear();
  DELAY(480);

  // Check that there is a slave device
  set_mode(INPUT_MODE);
  DELAY(70);
  res = is_clear();
  DELAY(410);

  return (res);
}

uint8_t
OneWire::read(uint8_t bits)
{
  uint8_t res = 0;
  uint8_t mix = 0;
  while (bits--) {
    synchronized {
      // Generate the read slot
      set_mode(OUTPUT_MODE);
      clear();
      DELAY(6);
      set_mode(INPUT_MODE);
      DELAY(9);
      res >>= 1;
      // Sample the data from the slave and generate crc
      if (is_set()) {
	res |= 0x80;
	mix = (_crc ^ 1);
      }
      else {
	mix = (_crc ^ 0);
      }
      _crc >>= 1;
      if (mix & 1) _crc ^= 0x8C;
      DELAY(55);
    }
  }
  return (res);
}

void
OneWire::write(uint8_t value)
{
  set_mode(OUTPUT_MODE);
  for (uint8_t bits = 0; bits < CHARBITS; bits++) {
    synchronized {
      // Generate the write slot; LSB first
      clear();
      if (value & 1) {
	DELAY(6);
	set();
	DELAY(64);
      }
      else {
	DELAY(60);
	set();
	DELAY(10);
      }
      value >>= 1;
    }
  }
  set_mode(INPUT_MODE);
}

bool
OneWire::Device::read_rom()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::READ_ROM);
  _pin->begin();
  for (uint8_t i = 0; i < ROM_MAX; i++) {
    _rom[i] = _pin->read();
  }
  return (_pin->end() == 0);
}

void
OneWire::Device::print_rom(IOStream& stream)
{
  for (uint8_t i = 0; i < ROM_MAX; i++)
    stream.printf_P(PSTR("rom[%d] = %hd\n"), i, _rom[i]);
}

