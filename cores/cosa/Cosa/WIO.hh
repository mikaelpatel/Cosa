/**
 * @file Cosa/WIO.hh
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

#ifndef COSA_WIO_HH
#define COSA_WIO_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Wireless.hh"

/**
 * IOStream driver for Wireless Interface. Allow IOStream such as
 * Trace over Wireless connection. Please note that most Wireless
 * device drivers do not provide a reliable link and characters may be
 * lost due to noise, collisions, etc.
 */
class WIO : public IOStream::Device {
public:
  /** Max size of payload. */
  static const uint8_t PAYLOAD_MAX = 30;

  /**
   * Construct Wireless Interface Output Stream.
   * @param[in] dev wireless device driver.
   * @param[in] dest destination device address.
   * @param[in] port message type (Default 0x00).
   */
  WIO(Wireless::Driver* dev, uint8_t dest, uint8_t port = 0x00) :
    IOStream::Device(),
    m_ix(0),
    m_dev(dev),
    m_dest(dest),
    m_port(port)
  {
  }

  /**
   * @override{IOStream::Device}
   * Number of bytes room in output buffer.
   * @return bytes.
   */
  virtual int room()
  {
    return (sizeof(m_buffer) - m_ix);
  }

  /**
   * @override{IOStream::Device}
   * Write character to output buffer. Flush if full or carrage return
   * character. Returns character if successful otherwise on error or
   * buffer full returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c)
  {
    m_buffer[m_ix++] = c;
    if (UNLIKELY(m_ix == sizeof(m_buffer) || c == '\n')) flush();
    return (c & 0xff);
  }

  /**
   * @override{IOStream::Device}
   * Flush internal device buffers. Wait for device to become idle.
   * @return zero(0) or negative error code.
   */
  virtual int flush()
  {
    int res = (m_dev->send(m_dest, m_port, m_buffer, m_ix) == m_ix ? 0 : -1);
    m_ix = 0;
    return (res);
  }

private:
  /** Buffered output. */
  uint8_t m_buffer[PAYLOAD_MAX];

  /** Next position in output buffer. */
  uint8_t m_ix;

  /** Current wireless device driver */
  Wireless::Driver* m_dev;

  /** Current wireless device destination address. */
  uint8_t m_dest;

  /** Current wireless device destination port. */
  uint8_t m_port;
};

#endif
