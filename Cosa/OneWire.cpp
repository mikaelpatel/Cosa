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
  for (i = 1; i < ROM_MAX; i++) _rom[i] = 0;
  return (0);
}

int
OneWire::Device::read(uint8_t bits)
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
	mix = (_crc ^ 1);
      }
      else {
	mix = (_crc ^ 0);
      }
      _crc >>= 1;
      if (mix & 1) _crc ^= 0x8C;
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
OneWire::Device::write(uint8_t value, uint8_t bits)
{
  uint8_t mix = 0;
  const uint8_t RETRY_MAX = 40;
  uint8_t retry = RETRY_MAX;
  synchronized {
    while (bits--) {
      retry = RETRY_MAX;
      do {
	while (is_set() && retry--) DELAY(1);
	if (retry == 0) synchronized_return (0);
	DELAY(1);
      } while (is_set());
      if (value & 1) {
	mix = (_crc ^ 1);
	DELAY(45);
      }
      else {
	set_mode(OUTPUT_MODE);
	set();
	clear();
	DELAY(45);
	set_mode(INPUT_MODE);
	mix = (_crc ^ 0);
      }
      value >>= 1;
      _crc >>= 1;
      if (mix & 1) _crc ^= 0x8C;
    }
  }
  return (1);
}

// FIX: Should become independent of Arduino timers
extern volatile unsigned long timer0_overflow_count;

static unsigned long 
micros() 
{
  unsigned long m;
  uint8_t t;
  synchronized {
    m = timer0_overflow_count;
    t = TCNT0;
    if ((TIFR0 & _BV(TOV0)) && (t < 255))
      m++;
  }	
  return ((m << 8) + t) << 2;
}

void 
OneWire::Device::service_request(Thing* it, uint8_t type, uint16_t value)
{
  OneWire::Device* dev = (OneWire::Device*) it;
  uint32_t stop = micros() + 440;

  static uint16_t req = 0;
  static uint16_t fns = 0;
  static uint16_t err = 0;

  req++;
  DELAY(200);
  dev->set_mode(INPUT_MODE);
  synchronized {
    DELAY(stop - micros());

    dev->_state = ROM_STATE;
    int cmd = dev->read(8);
    if (cmd == READ_ROM) {
      dev->_crc = 0;
      for (uint8_t i = 0; i < ROM_MAX - 1; i++)
	if (!dev->write(dev->_rom[i], 8)) synchronized_goto(error);
      if (!dev->write(dev->_crc, 8)) synchronized_goto(error);
    }

    else {

      if (cmd == SEARCH_ROM) {
	for (uint8_t i = 0; i < ROM_MAX; i++) {
	  uint8_t bits = dev->_rom[i];
	  for (uint8_t j = 0; j < CHARBITS; j++) {
	    uint8_t bit = (bits & 0x01);
	    bit |= (~bit << 1);
	    if (!dev->write(bit, 2)) synchronized_goto(error);
	    int value = dev->read(1);
	    if (value != (bits & 0x01)) synchronized_goto(error);
	    bits >>= 1;
	  }
	}
      }

      else if (cmd == MATCH_ROM) {
	for (uint8_t i = 0; i < ROM_MAX - 1; i++)
	  if (dev->read(8) != dev->_rom[i]) synchronized_goto(error);
	if (dev->read(8) < 0) synchronized_goto(error);
      } 

      else if (cmd != SKIP_ROM) synchronized_goto(error);

      dev->_state = FUNCTION_STATE;
      cmd = dev->read(8);
      if (cmd < 0) synchronized_goto(error);
      fns++;
      if (cmd == STATUS) {
	dev->_crc = 0;
	dev->write(req >> 8, 8);
	dev->write(req, 8);
	dev->write(fns >> 8, 8);
	dev->write(fns, 8);
	dev->write(err >> 8, 8);
	dev->write(err, 8);
	dev->write(dev->_crc, 8);
      }
    }
  }

 final:
  dev->_state = IDLE_STATE;
  dev->enable();
  return;

 error:
  err++;
  goto final;
}

void 
OneWire::Device::interrupt_handler(InterruptPin* pin, void* env)
{
  OneWire::Device* dev = (OneWire::Device*) pin;
  volatile uint32_t now = micros();
  if (dev->_state == IDLE_STATE) {
    if (dev->is_clear()) {
      dev->_time = now + 400L;
      dev->_state = RESET_STATE;
    }
  } 
  else if (dev->_state == RESET_STATE && now > dev->_time) {
    dev->_state = PRESENCE_STATE;
    dev->_time = now;
    dev->disable();
    dev->set_mode(OUTPUT_MODE);
    dev->set();
    dev->clear();
    InterruptPin::push_event(pin, env);
  }
  else dev->_state = IDLE_STATE;
}
