/**
 * @file Ciao.hh
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

#ifndef COSA_CIAO_HH
#define COSA_CIAO_HH

#include "Cosa/IOStream.hh"

/**
 * The Cosa Ciao data stream handler. Please see CIAO.txt for details.
 *
 * @section Limitations
 * The Ciao class handles only output. The data types 16, 64 and 80-bit
 * floating point are not supported.
 *
 * @section See Also
 * Requires an IOSteam::Device. This is used in binary/8-bit character
 * mode. See also Cosa/Fai.hh for details on board state reporting.
 *
 * @section References
 * [1] Sun Microsystems (1987). "XDR: External Data Representation
 * Standard". RFC 1014. Network Working Group. Retrieved July 11,
 * 2011. http://tools.ietf.org/html/rfc1014<br>
 * [2] Boost Serialization,
 * http://www.boost.org/doc/libs/1_46_1/libs/serialization/doc/index.html<br>
 * [3] Java Stream Format,
 * http://docs.oracle.com/javase/7/docs/platform/serialization/spec/protocol.html#10258<br>
 * [4] Arduino/Firmata, http://www.firmata.org<br>
 */
class Ciao {
public:
  /**
   * Tag attribute: Number for data values in sequence[count]. User
   * defined data type name as value or end of used defined data type
   * sequence.
   */
  enum {
    MASK_ATTR = 0x0f,
    COUNT0_ATTR = 0x00,
    COUNT4_MASK = 0x07,
    COUNT8_ATTR = 0x08,
    COUNT16_ATTR = 0x09,
    END_SEQUENCE_ATTR = 0x0f
  } __attribute__((packed));

  /**
   * Data type tag: Predefined data types and tags for extension.
   * NB: Arduino/AVR does not implement FLOAT16/64/80.
   */
  enum {
    MASK_TYPE = 0xf0,
    UINT8_TYPE = 0x00,
    UINT16_TYPE = 0x10,
    UINT32_TYPE = 0x20,
    UINT64_TYPE = 0x30,
    USER8_DESC_START = 0x40,
    USER8_DESC_END = USER8_DESC_START | END_SEQUENCE_ATTR,
    USER16_DESC_START = 0x50,
    USER16_DESC_END = USER16_DESC_START | END_SEQUENCE_ATTR,
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
  } __attribute__((packed));

  /**
   * Ciao Data type descriptor structures (program memory)
   */
  class Descriptor {
  public:
    /**
     * Predefined data type identity; System data type tags are 0..255.
     * User data type tags are 256..64K.
     */
    enum {
      HEADER_ID = 0x00,	       //!< Ciao header descriptor (8-bit, 1-15).
      COSA_FAI_ID = 0x10,      //!< Cosa Fai descriptors (8-bit, 16-).
      USER_ID = 0x0100	       //!< User descriptors (16-bit, 256-).
    };
    struct member_t {
      uint8_t type;
      uint16_t count;
      const char* name;
      const struct user_t* desc;
    };
    struct user_t {
      uint16_t id;
      const char* name;
      const member_t* member;
      uint8_t count;
    };
    static const user_t header_t PROGMEM;
  };

  /**
   * Stream header with magic string, revision and endian information
   * The identity code is HEADER_ID(0x00).
   */
  struct header_t {
    char* magic;
    uint8_t major;
    uint8_t minor;
    uint8_t endian;
  };
  enum {
    LITTLE_ENDIAN = 0,
    BIG_ENDIAN = 1
  } __attribute__((packed));

public:
  /**
   * Construct data streaming for given device.
   * @param[in] dev output device.
   */
  Ciao(IOStream::Device* dev = NULL) : m_dev(dev) {}

  /**
   * Set io-stream device.
   * @param[in] dev stream device.
   */
  void set(IOStream::Device* dev)
    __attribute__((always_inline))
  {
    if (dev == NULL) return;
    m_dev = dev;
  }

  /**
   * Start the data stream with a version string and endian information.
   */
  void begin();

  /**
   * Write given string to data stream.
   * @param[in] s string to write
   */
  void write(char* s);

  /**
   * Write given string from program memory to data stream.
   * @param[in] s program memory string to write
   */
  void write(str_P s);

  /**
   * Write given unsigned 8-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint8_t value);

  /**
   * Write given unsigned 8-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint8_t* buf, uint16_t count);

  /**
   * Write given unsigned 16-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint16_t value);

  /**
   * Write given unsigned 16-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint16_t* buf, uint16_t count);

  /**
   * Write given unsigned 32-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint32_t value);

  /**
   * Write given unsigned 32-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint32_t* buf, uint16_t count);

  /**
   * Write given unsigned 64-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(uint64_t value);

  /**
   * Write given unsigned 64-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(uint64_t* buf, uint16_t count);

  /**
   * Write given signed 8-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int8_t value);

  /**
   * Write given signed 8-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int8_t* buf, uint16_t count);

  /**
   * Write given signed 16-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int16_t value);

  /**
   * Write given signed 16-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int16_t* buf, uint16_t count);

  /**
   * Write given signed 32-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int32_t value);

  /**
   * Write given signed 32-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int32_t* buf, uint16_t count);

  /**
   * Write given signed 64-bit integer to data stream.
   * @param[in] value to write to data stream.
   */
  void write(int64_t value);

  /**
   * Write given signed 64-bit integer vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(int64_t* buf, uint16_t count);

  /**
   * Write given 32-bit floating point to data stream.
   * @param[in] value to write to data stream.
   */
  void write(float value);

  /**
   * Write given 32-bit floating vector to data stream.
   * @param[in] buf pointer to integer vector.
   * @param[in] count size of vector.
   */
  void write(float* buf, uint16_t count);

  /**
   * Write given user defined data type descriptor to data stream.
   * @param[in] desc descriptor structure to write (progam memory).
   */
  void write(const Descriptor::user_t* desc);

  /**
   * Write given user defined data type value to data stream.
   * @param[in] desc user defined data type (program memory).
   * @param[in] buf pointer to value(s) to write.
   * @param[in] count size of sequence to write.
   */
  void write(const Descriptor::user_t* desc, void* buf, uint16_t count);

protected:
  /**
   * Write data tag to given stream.
   * @param[in] type data type tag.
   * @param[in] count number of elements in sequence.
   */
  void write(uint8_t type, uint16_t count);

  IOStream::Device* m_dev;
};

#endif
