/**
 * @file Cosa/Continua.h
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
 * The Cosa data stream handler. Please see CONTINUA.txt for details.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CONTINUA_H__
#define __COSA_CONTINUA_H__

#include "Cosa/IOStream.h"

class Continua {
  
private:
  /**
   * Data type tag.
   */
  enum {
    UINT8_TYPE = 0x00,
    UINT16_TYPE = 0x10,
    UINT32_TYPE = 0x20,
    UINT64_TYPE = 0x30,
    USER8_DECL_START = 0x40,
    USER8_DECL_END = 0x4f,
    USER16_DECL_START = 0x50,
    USER16_END_START = 0x5f,
    USER8_TYPE = 0x60,
    USER16_TYPE = 0x70,
    INT8_TYPE = 0x80,
    INT16_TYPE = 0x90,
    INT32_TYPE = 0xa0,
    INT64_TYPE = 0xb0,
    FLOAT16_TYPE = 0xc0,
    FLOAT32_TYPE = 0xd0,
    FLOAT64_TYPE = 0xe0,
    FLOAT80_TYPE = 0xf0
  };

  /**
   * Number for data values in sequence[count].
   */
  enum {
    COUNT_MASK = 0x0f,
    COUNT0_TYPE = 0x00,
    COUNT4_MAX = 0x07,
    COUNT8_TYPE = 0x08,
    COUNT16_TYPE = 0x09,
    USER8_NAME_TYPE = 0x0a,
    USER16_NAME_TYPE = 0x0b,
    RESERVED1_TYPE = 0x0c,
    RESERVED2_TYPE = 0x0d,
    RESERVED3_TYPE = 0x0e,
    END_SEQUENCE_TYPE = 0x0f
  };

  /**
   * Data type declaraction member
   */
  struct decl_member_t {	// user data type member declaration
    uint8_t type;		// data type tag
    uint16_t id;		// user identity
    uint16_t count;		// number of elements in sequence
    const char* name;		// name of member
  };
  struct decl_user_t {		// user data type declaration
    uint16_t id;		// user identity
    decl_member_t* member;	// member declaractions
    uint8_t count;		// number of members
    const char* name;		// name of user type
  };

  IOStream::Device* _dev;

  void write(uint8_t type, uint16_t count);

public:
  Continua(IOStream::Device* dev) : _dev(dev) {}

  void write(char* buf)
  {
    write(UINT8_TYPE, 0);
    _dev->puts(buf);
    _dev->putchar(0);
  }

  void write_P(const char* buf)
  {
    write(UINT8_TYPE, 0);
    _dev->puts_P(buf);
    _dev->putchar(0);
  }

  void write(uint8_t value)
  {
    write(UINT8_TYPE, 1);
    _dev->putchar(value);
  }

  void write(uint8_t* buf, uint16_t count)
  {
    write(UINT8_TYPE, count);
    _dev->write(buf, count * sizeof(uint8_t));
  }

  void write(uint16_t value)
  {
    write(UINT16_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  void write(uint16_t* buf, uint16_t count)
  {
    write(UINT16_TYPE, count);
    _dev->write(buf, count * sizeof(uint16_t));
  }

  void write(uint32_t value)
  {
    write(UINT32_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  void write(uint32_t* buf, uint16_t count)
  {
    write(UINT32_TYPE, count);
    _dev->write(buf, count * sizeof(uint32_t));
  }

  void write(int8_t value)
  {
    write(INT8_TYPE, 1);
    _dev->putchar(value);
  }

  void write(int8_t* buf, int16_t count)
  {
    write(INT8_TYPE, count);
    _dev->write(buf, count * sizeof(int8_t));
  }

  void write(int16_t value)
  {
    write(INT16_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  void write(int16_t* buf, int16_t count)
  {
    write(INT16_TYPE, count);
    _dev->write(buf, count * sizeof(int16_t));
  }

  void write(int32_t value)
  {
    write(INT32_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  void write(int32_t* buf, int16_t count)
  {
    write(INT32_TYPE, count);
    _dev->write(buf, count * sizeof(int32_t));
  }

  void write(decl_user_t* decl, void* buf, uint16_t count);
};

#endif
