/**
 * @file Cosa/Ciao.h
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

#ifndef __COSA_CIAO_H__
#define __COSA_CIAO_H__

#include "Cosa/IOStream.h"

class Ciao {
  
public:
  /**
   * Data type tag:
   * Predefined data types and tags for extension.
   */
  enum {
    MASK_TYPE = 0xf0,
    UINT8_TYPE = 0x00,
    UINT16_TYPE = 0x10,
    UINT32_TYPE = 0x20,
    UINT64_TYPE = 0x30,
    USER8_DECL_START = 0x40,
    USER8_DECL_END = 0x4f,
    USER16_DECL_START = 0x50,
    USER16_DECL_END = 0x5f,
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
   * Tag attribute: 
   * Number for data values in sequence[count]. User data type name as
   * value or end of used defined data type sequence.
   */
  enum {
    MASK_ATTR = 0x0f,
    COUNT0_ATTR = 0x00,
    COUNT4_MASK = 0x07,
    COUNT8_ATTR = 0x08,
    COUNT16_ATTR = 0x09,
    USER8_NAME_ATTR = 0x0a,
    USER16_NAME_ATTR = 0x0b,
    RESERVED1_ATTR = 0x0c,
    RESERVED2_ATTR = 0x0d,
    RESERVED3_ATTR = 0x0e,
    END_SEQUENCE_ATTR = 0x0f
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
    const char* name;		// name of user type
    size_t size;		// size of data type instance (bytes)
    const decl_member_t* member; // member declaractions
    uint16_t count;		// number of members
  };
  
private:
  // Version and header
  static const uint16_t ID;
  static const char VERSION[] PROGMEM;

  // Output streaming device.
  IOStream::Device* _dev;

  /**
   * Write data tag to given stream.
   * @param[in] type data type tag
   * @param[in] count number of elements in sequence
   */
  void write(uint8_t type, uint16_t count);

public:
  /**
   * Construct data streaming for given device.
   * @param[in] dev output device.
   */
  Ciao(IOStream::Device* dev = 0) : _dev(dev) {}

  /**
   * Set io stream device.
   * @param[in] dev stream device.
   */
  void set(IOStream::Device* dev) 
  { 
    _dev = dev;
  }

  /**
   * Start the data stream with a version string and endian information.
   */
  void begin()
  {
    write_P(VERSION);
    write((uint16_t) 1);
  }

  /**
   * Write given string to data stream.
   * @param[in] s string to write
   */
  void write(char* s)
  {
    write(UINT8_TYPE, 0);
    _dev->puts(s);
    _dev->putchar(0);
  }

  /**
   * Write given string from program memory to data stream.
   * @param[in] s program memory string to write
   */
  void write_P(const char* buf)
  {
    write(UINT8_TYPE, 0);
    _dev->puts_P(buf);
    _dev->putchar(0);
  }

  /**
   * Write given unsigned 8-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint8_t value)
  {
    write(UINT8_TYPE, 1);
    _dev->putchar(value);
  }

  /**
   * Write given unsigned 8-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint8_t* buf, uint16_t count)
  {
    write(UINT8_TYPE, count);
    _dev->write(buf, count * sizeof(uint8_t));
  }

  /**
   * Write given unsigned 16-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint16_t value)
  {
    write(UINT16_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  /**
   * Write given unsigned 16-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint16_t* buf, uint16_t count)
  {
    write(UINT16_TYPE, count);
    _dev->write(buf, count * sizeof(uint16_t));
  }

  /**
   * Write given unsigned 32-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint32_t value)
  {
    write(UINT32_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  /**
   * Write given unsigned 32-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint32_t* buf, uint16_t count)
  {
    write(UINT32_TYPE, count);
    _dev->write(buf, count * sizeof(uint32_t));
  }

  /**
   * Write given signed 8-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int8_t value)
  {
    write(INT8_TYPE, 1);
    _dev->putchar(value);
  }

  /**
   * Write given signed 8-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int8_t* buf, int16_t count)
  {
    write(INT8_TYPE, count);
    _dev->write(buf, count * sizeof(int8_t));
  }

  /**
   * Write given signed 16-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int16_t value)
  {
    write(INT16_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  /**
   * Write given signed 16-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int16_t* buf, int16_t count)
  {
    write(INT16_TYPE, count);
    _dev->write(buf, count * sizeof(int16_t));
  }

  /**
   * Write given signed 32-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int32_t value)
  {
    write(INT32_TYPE, 1);
    _dev->write(&value, sizeof(value));
  }

  /**
   * Write given signed 32-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int32_t* buf, int16_t count)
  {
    write(INT32_TYPE, count);
    _dev->write(buf, count * sizeof(int32_t));
  }

  /**
   * Write given user defined data type declaration to data stream.
   * @param[in] decl declaration structure to write (progam memory(
   */
  void write(const decl_user_t* decl);

  /**
   * Write given user defined data type value to data stream.
   * @param[in] decl user defined data type (program memory).
   * @param[in] buf pointer to value(s) to write.
   * @param[in] count size of sequence to write.
   */
  void write(const decl_user_t* decl, void* buf, uint16_t count);
};

#endif
