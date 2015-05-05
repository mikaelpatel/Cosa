/**
 * @file ProtocolBuffer.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_PROTOCOLBUFFER_HH
#define COSA_PROTOCOLBUFFER_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Google Protocol Buffers data stream encoder/decoder. Adapted for
 * AVR/8-bit processors to allow simple data exchange with hosts.
 * Implements encoding and decoding of signed/unsigned integers and
 * floating point up to 32-bits. 64-bit integers and double are not
 * supported. Support length delimited blocks from both SRAM and
 * PROGMEM.
 */
class ProtocolBuffer {
public:
  /**
   * Encoding type.
   */
  enum Type {
    VARINT,		       //!< int8..32, uint8..32, bool, enum.
    FIXED64,		       //!< not supported.
    LENGTH_DELIMITED,	       //!< string, bytes, embedded messages,
			       //!< and packed repeated fields.
    START_GROUP,	       //!< not supported, deprecated.
    END_GROUP,		       //!< not supported, deprecated.
    FIXED32		       //!< float.
  } __attribute__((packed));

  /** Max value of tag. */
  static const uint8_t TAG_MAX = 31;

  /**
   * Construct stream with given device. Default is the null device.
   * @param[in] ins input stream device.
   * @param[in] outs output stream device.
   */
  ProtocolBuffer(IOStream::Device* ins, IOStream::Device* outs) :
    m_ins(ins),
    m_outs(outs)
  {}

  /**
   * Read next byte from the input stream.
   * @return next byte or negative error code.
   */
  int getchar();

  /**
   * Write byte to the output stream.
   * @param[in] c character to write.
   * @return bytes written or negative error code.
   */
  int putchar(char c)
    __attribute__((always_inline))
  {
    return (m_outs->putchar(c));
  }

  /**
   * Read tag and type from input stream. Return number of bytes
   * read or negative error code.
   * @param[out] tag.
   * @param[out] type.
   * @return number of bytes read or negative error code.
   */
  int read(uint8_t& tag, Type& type)
    __attribute__((always_inline))
  {
    uint8_t prefix = getchar();
    tag = (prefix >> 3);
    type = (Type) (prefix & 0x7);
    return (type <= FIXED32);
  }

  /**
   * Read unsigned value from the input stream. Return number of bytes
   * read or negative error code.
   * @param[out] value.
   * @return number of bytes read or negative error code.
   */
  int read(uint32_t& value);

  /**
   * Read signed value from the input stream. Return number of bytes
   * read or negative error code.
   * @param[out] value.
   * @return number of bytes read or negative error code.
   */
  int read(int32_t& value)
    __attribute__((always_inline))
  {
    uint32_t zigzag;
    int res = read(zigzag);
    if (res < 0) return (res);
    value = ((zigzag & 1) ? ~(zigzag >> 1) : (zigzag >> 1));
    return (res);
  }

  /**
   * Read floating point value from input stream. Return number of bytes
   * read or negative error code.
   * @param[out] value floating point number.
   * @return number of bytes read or negative error code.
   */
  int read(float32_t& value);

  /**
   * Read length delimited string or message from the input stream.
   * Return number of bytes read or negative error code.
   * @param[in] buf buffer to write.
   * @param[in] count size of buffer.
   * @return number of bytes read or negative error code.
   */
  int read(void* buf, uint8_t count);

  /**
   * Write tag and type to output stream. Return number of bytes
   * written or negative error code.
   * @param[in] tag.
   * @param[in] type.
   * @return number of bytes read or negative error code.
   */
  int write(uint8_t tag, Type type)
    __attribute__((always_inline))
  {
    if (tag > TAG_MAX) return (EINVAL);
    if (putchar(tag << 3 | type) < 0) return (EIO);
    return (1);
  }

  /**
   * Write given unsigned integer value into the output stream.
   * Return number of bytes written or negative error code.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(uint32_t value);

  /**
   * Write given signed integer value into the output stream.
   * Return number of bytes written or negative error code.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(int32_t value)
    __attribute__((always_inline))
  {
    uint32_t zigzag = ((value < 0) ? ~(value << 1) : (value << 1));
    return (write(zigzag));
  }

  /**
   * Write given floating point value to the output stream.
   * Return number of bytes written or negative error code.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(float32_t value)
    __attribute__((always_inline))
  {
    return (write(&value, sizeof(value)));
  }

  /**
   * Write count number of bytes from given buffer. Returns number of
   * bytes written or negative error code.
   * @param[in] buf pointer to buffer.
   * @param[in] count number of bytes in buffer.
   * @return byte written or negative error code.
   */
  int write(const void* buf, uint8_t count)
    __attribute__((always_inline))
  {
    if (m_outs == NULL) return (EINVAL);
    return (m_outs->write(buf, count));
  }

  /**
   * Write count number of bytes from given buffer in program
   * memory. Returns number of bytes written or negative error code.
   * @param[in] buf pointer to buffer in program memory.
   * @param[in] count number of bytes in buffer.
   * @return byte written or negative error code.
   */
  int write_P(const void* buf, uint8_t count);

  /**
   * Write given signed integer value and tag into the output stream.
   * Return number of bytes written or negative error code.
   * @param[in] tag.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(uint8_t tag, int32_t value)
    __attribute__((always_inline))
  {
    if (write(tag, VARINT) < 0) return (EIO);
    int res = write(value);
    if (res < 0) return (res);
    return (res + 1);
  }

  int write(uint8_t tag, int16_t value)
    __attribute__((always_inline))
  {
    return (write(tag, (int32_t) value));
  }

  int write(uint8_t tag, int8_t value)
    __attribute__((always_inline))
  {
    return (write(tag, (int32_t) value));
  }

  /**
   * Write given unsigned integer value and tag into the output stream.
   * Return number of bytes written or negative error code.
   * @param[in] tag.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(uint8_t tag, uint32_t value)
    __attribute__((always_inline))
  {
    if (write(tag, VARINT) < 0) return (EIO);
    int res = write(value);
    if (res < 0) return (res);
    return (res + 1);
  }

  int write(uint8_t tag, uint16_t value)
    __attribute__((always_inline))
  {
    return (write(tag, (uint32_t) value));
  }

  int write(uint8_t tag, uint8_t value)
    __attribute__((always_inline))
  {
    return (write(tag, (uint32_t) value));
  }

  /**
   * Write given length delimited string or message with given tag to
   * output stream. Return number of bytes written or negative error
   * code.
   * @param[in] tag for serialized value.
   * @param[in] buf buffer to read from.
   * @param[in] count size of buffer.
   * @return number of bytes written or negative error code.
   */
  int write(uint8_t tag, const void* buf, uint8_t count)
  {
    if (write(tag, LENGTH_DELIMITED) < 0) return (EIO);
    if (putchar(count) < 0) return (EIO);
    int res = write(buf, count);
    if (res != count) return (EIO);
    return (count + 2);
  }

  int write(uint8_t tag, const char* str)
    __attribute__((always_inline))
  {
    return (write(tag, str, strlen(str)));
  }

  int write_P(uint8_t tag, const char* str)
  {
    if (write(tag, LENGTH_DELIMITED) < 0) return (EIO);
    uint8_t count = strlen_P(str);
    if (putchar(count) < 0) return (EIO);
    int res = write_P(str, count);
    if (res != count) return (EIO);
    return (count + 2);
  }

  /**
   * Write floating point value and given tag to the output stream.
   * Return number of bytes written or negative error code.
   * @param[in] tag.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(uint8_t tag, float32_t value)
  {
    if (write(tag, FIXED32) < 0) return (EIO);
    int res = write(value);
    if (res < 0) return (EIO);
    return (res + 1);
  }

protected:
  /** Input stream. */
  IOStream::Device* m_ins;

  /** Output stream. */
  IOStream::Device* m_outs;
};

#endif
