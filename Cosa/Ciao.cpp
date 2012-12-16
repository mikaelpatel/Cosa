/**
 * @file Cosa/Ciao.cpp
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
 * The Cosa Ciao data stream handler. Please see CIAO.txt for details.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Ciao.h"
#include "Cosa/Trace.h"
#include <avr/pgmspace.h>

void 
Ciao::write(char* s)
{
  write(UINT8_TYPE, 0);
  _dev->puts(s);
  _dev->putchar(0);
}

void 
Ciao::write_P(const char* buf)
{
  write(UINT8_TYPE, 0);
  _dev->puts_P(buf);
  _dev->putchar(0);
}

void 
Ciao::write(uint8_t value)
{
  write(UINT8_TYPE, 1);
  _dev->putchar(value);
}

void 
Ciao::write(uint8_t* buf, uint16_t count)
{
  write(UINT8_TYPE, count);
  _dev->write(buf, count * sizeof(uint8_t));
}

void 
Ciao::write(uint16_t value)
{
  write(UINT16_TYPE, 1);
  _dev->write(&value, sizeof(value));
}

void 
Ciao::write(uint16_t* buf, uint16_t count)
{
  write(UINT16_TYPE, count);
  _dev->write(buf, count * sizeof(uint16_t));
}

void Ciao::write(uint32_t value)
{
  write(UINT32_TYPE, 1);
  _dev->write(&value, sizeof(value));
}

void Ciao::write(uint32_t* buf, uint16_t count)
{
  write(UINT32_TYPE, count);
  _dev->write(buf, count * sizeof(uint32_t));
}

void Ciao::write(int8_t value)
{
  write(INT8_TYPE, 1);
  _dev->putchar(value);
}

void 
Ciao::write(int8_t* buf, int16_t count)
{
  write(INT8_TYPE, count);
  _dev->write(buf, count * sizeof(int8_t));
}

void 
Ciao::write(int16_t value)
{
  write(INT16_TYPE, 1);
  _dev->write(&value, sizeof(value));
}

void Ciao::write(int16_t* buf, int16_t count)
{
  write(INT16_TYPE, count);
  _dev->write(buf, count * sizeof(int16_t));
}

void 
Ciao::write(int32_t value)
{
  write(INT32_TYPE, 1);
  _dev->write(&value, sizeof(value));
}

void Ciao::write(int32_t* buf, int16_t count)
{
  write(INT32_TYPE, count);
  _dev->write(buf, count * sizeof(int32_t));
}

void 
Ciao::write(float value)
{
  write(FLOAT32_TYPE, 1);
  _dev->write(&value, sizeof(value));
}

void 
Ciao::write(float* buf, int16_t count)
{
  write(FLOAT32_TYPE, count);
  _dev->write(buf, count * sizeof(float));
}

void
Ciao::write(uint8_t type, uint16_t count)
{
  // Tag byte contains count[0..7]
  if (count < 8) {
    count |= type;
  }

  // Tag byte contains marker. Succeeding byte counter[8..255]
  else if (count < 256) {
    _dev->putchar(type | COUNT8_ATTR);
  }
  
  // Tag byte contains marker. Succeeding two bytes counter[255..64K]
  else {
    _dev->putchar(type | COUNT16_ATTR);
    _dev->putchar(count >> 8);
  }

  _dev->putchar(count);
}

void 
Ciao::write(const Descriptor::user_t* desc)
{
  // Read descaration from program memory
  Descriptor::user_t d;
  memcpy_P(&d, desc, sizeof(d));
  
  // Write descaration start tag and identity number (8 or 16-bit)
  if (d.id < 256) {
    _dev->putchar(USER8_DESC_START);
  }
  else {
    _dev->putchar(USER16_DESC_START);
    _dev->putchar(d.id >> 8);
  }
  _dev->putchar(d.id);

  // Write descaration name null terminated
  _dev->puts_P(d.name);
  _dev->putchar(0);
  
  // Write members with name null terminated
  const Descriptor::member_t* mp = d.member;
  for (uint16_t i = 0; i < d.count; i++) {
    Descriptor::member_t m;
    memcpy_P(&m, mp++, sizeof(m));
    write(m.type, m.count);
    _dev->puts_P(m.name);
    _dev->putchar(0);
  }
	 
  // Write descaration end tag
  if (d.id < 256) {
    _dev->putchar(USER8_DESC_END);
  }
  else {
    _dev->putchar(USER16_DESC_END);
  }
}

static const uint8_t sizeoftype[] PROGMEM = {
  1,
  2,
  4,
  8, 
  0, 
  0, 
  0, 
  0, 
  1, 
  2, 
  4, 
  8, 
  2, 
  4, 
  8, 
  10
};

void 
Ciao::write(const Descriptor::user_t* desc, void* buf, uint16_t count)
{
  // Read descaration from program memory
  Descriptor::user_t d;
  memcpy_P(&d, desc, sizeof(d));
  
  // Write type tag for user data with count and type identity
  if (d.id < 256) {
    write(USER8_TYPE, count);
  }
  else {
    write(USER16_TYPE, count);
    _dev->putchar(d.id >> 8);
  }
  _dev->putchar(d.id);

  // Write data buffer to stream
  uint8_t* dp = (uint8_t*) buf;
  while (count--) {
    const Descriptor::member_t* mp = d.member;
    for (uint16_t i = 0; i < d.count; i++) {
      Descriptor::member_t m;
      memcpy_P(&m, mp++, sizeof(m));
      // Allow strings and data elements vectors only
      // Fix: Add table with user defined types
      if (m.count == 0 && m.type == UINT8_TYPE) {
	uint8_t* sp = *((uint8_t**) dp);
	uint8_t d;
	do {
	  d = *sp++;
	  _dev->putchar(d);
	} while (d != 0);
	dp += sizeof(sp);
      } 
      else {
	size_t s = pgm_read_byte(&sizeoftype[m.type >> 4]) * m.count;
	_dev->write(dp, s);
	dp += s;
      }
    }
  }
}
