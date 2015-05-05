/**
 * @file Ciao.cpp
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

#include "Ciao.hh"

// Ciao configuration
static char MAGIC[] = "Cosa::Ciao";
static const uint8_t MAJOR = 1;
static const uint8_t MINOR = 0;

// Ciao header descriptor
#if defined(NREFECTION)
#define descr_name 0
#define magic_name 0
#define major_name 0
#define minor_name 0
#define endian_name 0
#else
static const char descr_name[] __PROGMEM = "Ciao::header_t";
static const char magic_name[] __PROGMEM= "magic";
static const char major_name[] __PROGMEM= "major";
static const char minor_name[] __PROGMEM= "minor";
static const char endian_name[] __PROGMEM = "endian";
#endif
static const Ciao::Descriptor::member_t descr_members[] __PROGMEM = {
  {
    Ciao::UINT8_TYPE,
    0,
    magic_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    major_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    minor_name,
    0
  },
  {
    Ciao::UINT8_TYPE,
    1,
    endian_name,
    0
  }
};
const Ciao::Descriptor::user_t Ciao::Descriptor::header_t __PROGMEM = {
  Ciao::Descriptor::HEADER_ID,
  descr_name,
  descr_members,
  membersof(descr_members)
};

void
Ciao::begin()
{
  header_t header;
  header.magic = MAGIC;
  header.major = MAJOR;
  header.minor = MINOR;
  header.endian = LITTLE_ENDIAN;
  write(&Descriptor::header_t, &header, 1);
}

void
Ciao::write(char* s)
{
  write(UINT8_TYPE, 0);
  m_dev->puts(s);
  m_dev->putchar(0);
}

void
Ciao::write(str_P buf)
{
  write(UINT8_TYPE, 0);
  m_dev->puts(buf);
  m_dev->putchar(0);
}

void
Ciao::write(uint8_t value)
{
  write(UINT8_TYPE, 1);
  m_dev->putchar(value);
}

void
Ciao::write(uint8_t* buf, uint16_t count)
{
  write(UINT8_TYPE, count);
  m_dev->write(buf, count * sizeof(uint8_t));
}

void
Ciao::write(uint16_t value)
{
  write(UINT16_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void
Ciao::write(uint16_t* buf, uint16_t count)
{
  write(UINT16_TYPE, count);
  m_dev->write(buf, count * sizeof(uint16_t));
}

void
Ciao::write(uint32_t value)
{
  write(UINT32_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void
Ciao::write(uint32_t* buf, uint16_t count)
{
  write(UINT32_TYPE, count);
  m_dev->write(buf, count * sizeof(uint32_t));
}

void
Ciao::write(uint64_t value)
{
  write(UINT64_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void
Ciao::write(uint64_t* buf, uint16_t count)
{
  write(UINT64_TYPE, count);
  m_dev->write(buf, count * sizeof(uint32_t));
}

void
Ciao::write(int8_t value)
{
  write(INT8_TYPE, 1);
  m_dev->putchar(value);
}

void
Ciao::write(int8_t* buf, uint16_t count)
{
  write(INT8_TYPE, count);
  m_dev->write(buf, count * sizeof(int8_t));
}

void
Ciao::write(int16_t value)
{
  write(INT16_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void Ciao::write(int16_t* buf, uint16_t count)
{
  write(INT16_TYPE, count);
  m_dev->write(buf, count * sizeof(int16_t));
}

void
Ciao::write(int32_t value)
{
  write(INT32_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void
Ciao::write(int32_t* buf, uint16_t count)
{
  write(INT32_TYPE, count);
  m_dev->write(buf, count * sizeof(int32_t));
}

void
Ciao::write(int64_t value)
{
  write(INT64_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void
Ciao::write(int64_t* buf, uint16_t count)
{
  write(INT64_TYPE, count);
  m_dev->write(buf, count * sizeof(int32_t));
}

void
Ciao::write(float value)
{
  write(FLOAT32_TYPE, 1);
  m_dev->write(&value, sizeof(value));
}

void
Ciao::write(float* buf, uint16_t count)
{
  write(FLOAT32_TYPE, count);
  m_dev->write(buf, count * sizeof(float));
}

void
Ciao::write(uint8_t type, uint16_t count)
{
  // Tag byte contains count[0..7]
  if (count < 8) {
    count |= type;
  }

  // Else tag byte contains marker. Succeeding byte counter[8..255]
  else if (count < 256) {
    m_dev->putchar(type | COUNT8_ATTR);
  }

  // Else tag byte contains marker. Succeeding two bytes counter[256..64K]
  else {
    m_dev->putchar(type | COUNT16_ATTR);
    m_dev->putchar(count >> 8);
  }

  m_dev->putchar(count);
}

void
Ciao::write(const Descriptor::user_t* desc)
{
  // Read descriptor from program memory
  Descriptor::user_t d;
  memcpy_P(&d, desc, sizeof(d));

  // Write descriptor start tag and identity number (8 or 16-bit)
  if (d.id < 256) {
    m_dev->putchar(USER8_DESC_START);
  }
  else {
    m_dev->putchar(USER16_DESC_START);
    m_dev->putchar(d.id >> 8);
  }
  m_dev->putchar(d.id);

  // Write descriptor null terminated name null
  m_dev->puts((str_P) d.name);
  m_dev->putchar(0);

  // Write members with name null terminated
  const Descriptor::member_t* mp = d.member;
  for (uint16_t i = 0; i < d.count; i++) {
    Descriptor::member_t m;
    memcpy_P(&m, mp++, sizeof(m));
    write(m.type, m.count);
    m_dev->puts((str_P) m.name);
    m_dev->putchar(0);
  }

  // Write descriptor end tag
  m_dev->putchar(d.id < 256 ? USER8_DESC_END : USER16_DESC_END);
}

static const uint8_t sizeoftype[] __PROGMEM = {
  sizeof(uint8_t),
  sizeof(uint16_t),
  sizeof(uint32_t),
  sizeof(uint64_t),
  0,
  0,
  0,
  0,
  sizeof(int8_t),
  sizeof(int16_t),
  sizeof(int32_t),
  sizeof(int64_t),
  0,
  sizeof(float32_t),
  0,
  0
};

void
Ciao::write(const Descriptor::user_t* desc, void* buf, uint16_t count)
{
  // Read descriptor from program memory
  Descriptor::user_t d;
  memcpy_P(&d, desc, sizeof(d));

  // Write type tag for user data with count and type identity
  if (d.id < 256) {
    write(USER8_TYPE, count);
  }
  else {
    write(USER16_TYPE, count);
    m_dev->putchar(d.id >> 8);
  }
  m_dev->putchar(d.id);

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
	  m_dev->putchar(d);
	} while (d != 0);
	dp += sizeof(sp);
      }
      else {
	size_t s = pgm_read_byte(&sizeoftype[m.type >> 4]) * m.count;
	if (s == 0) return;
	m_dev->write(dp, s);
	dp += s;
      }
    }
  }
}
