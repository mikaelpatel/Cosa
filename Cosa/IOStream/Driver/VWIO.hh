/**
 * @file Cosa/IOStream/Driver/VWIO.hh
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
 * @section Description
 * IOStream driver for Virtual Wire Interface. Allow IOStream
 * such as Trace over Virtual Wire connection.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_DRIVER_VWIO_HH__
#define __COSA_IOSTREAM_DRIVER_VWIO_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/VWI.hh"

class VWIO : public IOStream::Device {
private:
  VWI::Transmitter m_tx;
  uint8_t m_buffer[VWI::PAYLOAD_MAX];
  uint8_t m_ix;

public:
  /**
   * Construct Virtual Wire Interface Output Stream.
   * @param[in] pin output pin.
   * @param[in] codec from receiver.
   */
  VWIO(Board::DigitalPin pin, VWI::Codec* codec) :
    IOStream::Device(),
    m_tx(pin, codec),
    m_ix(0)
  {
  }

  /**
   * @override
   * Write character to output buffer. Flush if full or carrage return
   * character. Returns character if successful otherwise on error or
   * buffer full returns EOF(-1),
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c)
  {
    m_buffer[m_ix++] = c;
    if (m_ix == sizeof(m_buffer) || c == '\n') flush();
    return (c & 0xff);
  }

  /**
   * @override
   * Flush internal device buffers. Wait for device to become idle.
   * @return zero(0) or negative error code.
   */
  virtual int flush()
  {
    int res = (m_tx.send(m_buffer, m_ix) == m_ix ? 0 : -1);
    m_tx.await();
    m_ix = 0;
    return (res);
  }

  /**
   * Start VWI transmitter driver.
   * @param[in] baudrate serial bitrate (default 2000).
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint16_t baudrate = 2000) 
  {
    return (VWI::begin(baudrate) && m_tx.begin());
  }

  /**
   * Stop VWI transitter device driver.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    return (m_tx.end());
  }
};

#endif
