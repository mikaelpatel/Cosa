/**
 * @file Cosa/OneWire/Driver.cpp
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

int8_t
OneWire::Driver::search_rom(int8_t last)
{
  if (!_pin->reset()) return (ERROR);
  _pin->write(OneWire::SEARCH_ROM);
  uint8_t pos = 0;
  int8_t next = LAST;
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t data = 0;
    for (uint8_t j = 0; j < 8; j++) {
      data >>= 1;
      switch (_pin->read(2)) {
      case 0b00: // Discrepency between device roms
	if (pos == last) {
	  _pin->write(1, 1); 
	  data |= 0x80; 
	  last = FIRST;
	} 
	else if (pos > last) {
	  _pin->write(0, 1); 
	  next = pos;
	} else if (_rom[i] & (1 << j)) {
	  _pin->write(1, 1);
	  data |= 0x80; 
	} 
	else {
	  _pin->write(0, 1);
	}
	break;
      case 0b01: // Only one's at this position 
	_pin->write(1, 1); 
	data |= 0x80; 
	break;
      case 0b10: // Only zero's at this position
	_pin->write(0, 1); 
	break;
      case 0b11: // No device detected
	goto error;
      }
      pos += 1;
    }
    _rom[i] = data;
  }
  return (next);
 error:
  return (ERROR);
}

bool
OneWire::Driver::read_rom()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::READ_ROM);
  _pin->begin();
  for (uint8_t i = 0; i < ROM_MAX; i++) {
    _rom[i] = _pin->read();
  }
  return (_pin->end() == 0);
}

bool
OneWire::Driver::match_rom()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::MATCH_ROM);
  for (uint8_t i = 0; i < ROM_MAX; i++) {
    _pin->write(_rom[i]);
  }
  return (1);
}

bool
OneWire::Driver::skip_rom()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::SKIP_ROM);
  return (1);
}

void
OneWire::Driver::print_rom(IOStream& stream)
{
  uint8_t i;
  stream.printf_P(PSTR("OneWire::rom(family = %hd, id = "), _rom[0]);
  for (i = 1; i < ROM_MAX - 1; i++)
    stream.printf_P(PSTR("%hd, "), _rom[i]);
  stream.printf_P(PSTR("crc = %hd)\n"), _rom[i]);
}

bool 
OneWire::Driver::connect(uint8_t family, uint8_t index)
{
  int8_t last = FIRST;
  do {
    last = search_rom(last);
    if (last == ERROR) return (0);
    if (_rom[0] == family) {
      if (index == 0) return (1);
      index -= 1;
    }
  } while (last != LAST);
  for (uint8_t i = 1; i < ROM_MAX; i++) _rom[i] = 0;
  return (0);
}
