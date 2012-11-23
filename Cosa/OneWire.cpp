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
 * 1-Wire device driver support class. Note: this is for single
 * slave without search and match of rom codes.
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

  // Check that there is a slave device presence
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
  uint8_t adjust = CHARBITS - bits;
  while (bits--) {
    synchronized {

      // Generate the read slot; LSB to MSB order
      set_mode(OUTPUT_MODE);
      clear();
      DELAY(6);
      set_mode(INPUT_MODE);
      DELAY(9);
      res >>= 1;

      // Sample the data from the slave and generate CRC
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
  // Adjust result to align with LSB 
  while (adjust--) res >>= 1;
  return (res);
}

void
OneWire::write(uint8_t value, uint8_t bits)
{
  set_mode(OUTPUT_MODE);
  while (bits--) {
    synchronized {

      // Generate the write slot; LSB to MSB order
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

int8_t
OneWire::Device::search_rom(int8_t last)
{
  if (!_pin->reset()) return (0);
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
	} 
	else if (pos > last) {
	  _pin->write(0, 1); 
	  next = pos;
	} 
	else {
	  if (_rom[i] & (1 << j)) {
	    _pin->write(1, 1);
	    data |= 0x80; 
	  } 
	  else {
	    _pin->write(0, 1);
	  }
	  next = pos;
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
  return (-1);
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

bool
OneWire::Device::match_rom()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::MATCH_ROM);
  for (uint8_t i = 0; i < ROM_MAX; i++) {
    _pin->write(_rom[i]);
  }
  return (1);
}

bool
OneWire::Device::skip_rom()
{
  if (!_pin->reset()) return (0);
  _pin->write(OneWire::SKIP_ROM);
  return (1);
}

void
OneWire::Device::print_rom(IOStream& stream)
{
  for (uint8_t i = 0; i < ROM_MAX; i++)
    stream.printf_P(PSTR("rom[%d] = %hd\n"), i, _rom[i]);
}

bool 
OneWire::Device::connect(uint8_t code, uint8_t index)
{
  int8_t last = FIRST;
  do {
    last = search_rom(last);
    if (last == -1) return (0);
    if (_rom[0] == code && index == 0) return (1);
    index -= 1;
  } while (last != LAST);
  return (0);
}

