/**
 * @file Cosa/ProtocolBuffer.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_PROTOCOLBUFFER_HH__
#define __COSA_PROTOCOLBUFFER_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Google Protocol Buffers data stream encoder/decoder. Adapted to
 * AVR/8-bit processors to allow simple data exchange with hosts.
 */
class ProtocolBuffer {
public:
  /**
   * Encoding type
   */
  enum Type {
    VARINT,
    FIXED64,
    LENGTH_DELIMITED,
    START_GROUP,
    END_GROUP,
    FIXED32
  } __attribute__((packed));

  /** Max value of tag */
  static const uint8_t TAG_MAX = 31;

protected:
  /** Input stream */
  IOStream::Device* m_ins;

  /** Output stream */
  IOStream::Device* m_outs;

  /**
   * Read next byte from input stream.
   * @return next byte or negative error code.
   */
  int getchar();

  /**
   * Write byte from input stream.
   * @param[in] c character to write.
   * @return bytes written or negative error code.
   */
  int putchar(char c)
  {
    return (m_outs->putchar(c));
  }

public:
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
   * Read tag and type from input stream. Return number of bytes 
   * read or negative error code.
   * @param[out] tag.
   * @param[out] type.
   * @return number of bytes read or negative error code.
   */
  int read(uint8_t& tag, Type& type)
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
  {
    uint32_t v;
    int res = read(v);
    if (res < 0) return (-1);
    value = ((v & 1) ? ~(v >> 1) : (v >> 1));
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
  {
    if (tag > TAG_MAX) return (-1);
    if (putchar(tag << 3 | type) < 0) return (-1);
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
  {
    return (m_outs->write(buf, count));
  }

  /**
   * Write given signed integer value and tag into the output stream. 
   * Return number of bytes written or negative error code.
   * @param[in] tag.
   * @param[in] value.
   * @return number of bytes written or negative error code.
   */
  int write(uint8_t tag, int32_t value)
  {
    if (write(tag, VARINT) < 0) return (-1);
    int res = write(value);
    if (res < 0) return (-1);
    return (res + 1);
  }
  int write(uint8_t tag, int16_t value) 
  { 
    return (write(tag, (int32_t) value)); 
  }
  int write(uint8_t tag, int8_t value) 
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
  {
    if (write(tag, VARINT) < 0) return (-1);
    int res = write(value);
    if (res < 0) return (-1);
    return (res + 1);
  }
  int write(uint8_t tag, uint16_t value) 
  { 
    return (write(tag, (uint32_t) value)); 
  }
  int write(uint8_t tag, uint8_t value) 
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
    if (write(tag, LENGTH_DELIMITED) < 0) return (-1);
    if (putchar(count) < 0) return (-1);
    if (write(buf, count) < 0) return (-1);
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
    if (write(tag, FIXED32) < 0) return (-1);
    int res = write(value);
    if (res < 0) return (-1);
    return (res + 1);
  }
};

#endif
