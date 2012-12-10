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
 * 1-Wire device driver support class. Allows device rom search,
 * connection to multiple devices and slave devices.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/OneWire.h"
#include <util/delay_basic.h>

#define DELAY(us) _delay_loop_2((us) << 2)

bool
OneWire::reset()
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
  return (res);
}

uint8_t
OneWire::read(uint8_t bits)
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
  res >>= adjust;
  return (res);
}

void
OneWire::write(uint8_t value, uint8_t bits)
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
	mix = (_crc ^ 1);
      }
      else {
	DELAY(60);
	set();
	DELAY(10);
	mix = (_crc ^ 0);
      }
      value >>= 1;
      _crc >>= 1;
      if (mix & 1) _crc ^= 0x8C;
    }
    set_mode(INPUT_MODE);
  }
  DELAY(10);
}

void
OneWire::print_devices(IOStream& stream)
{
  Driver dev(this);
  int8_t last = Driver::FIRST;
  do {
    last = dev.search_rom(last);
    if (last == Driver::ERROR) return;
    dev.print_rom(stream);
  } while (last != Driver::LAST);
}
