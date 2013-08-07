/**
 * @file Cosa/VWI/VWI_Transmitter.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire)
 * Copyright (C) 2013, Mikael Patel (Cosa C++ port and refactoring)
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

#include "Cosa/VWI.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"
#include <util/crc16.h>

VWI::Transmitter::Transmitter(Board::DigitalPin pin, Codec* codec) :
  OutputPin(pin),
  m_codec(codec),
  m_nr(0)
{
  VWI::s_transmitter = this;
  memcpy_P(m_buffer, codec->get_preamble(), codec->PREAMBLE_MAX);
}

bool
VWI::Transmitter::resend()
{
  if (m_enabled) return (false);
  return (begin());
}

bool 
VWI::Transmitter::begin()
{
  m_index = 0;
  m_bit = 0;
  m_sample = 0;
  m_enabled = true;
  return (true);
}

void 
VWI::Transmitter::await()
{
  while (m_enabled) Power::sleep(s_mode);
}

bool 
VWI::Transmitter::send(const iovec_t* vec)
{
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++)
    len += vp->size;

  // Check that the message is not too large
  if (len > PAYLOAD_MAX) return (false);

  uint8_t *tp = m_buffer + m_codec->PREAMBLE_MAX;
  uint16_t crc = 0xffff;
  
  // Wait for transmitter to become available. Might be transmitting
  await();

  // Encode the message length
  uint8_t count = len + 3;
  crc = _crc_ccitt_update(crc, count);
  *tp++ = m_codec->encode4(count >> 4);
  *tp++ = m_codec->encode4(count);

  // Encode the message into symbols. Each byte is converted into 
  // 2 symbols, high nybble first, low nybble second
  for (const iovec_t* vp = vec; vp->buf != 0; vp++) {
    uint8_t *bp = (uint8_t*) vp->buf;
    for (uint8_t i = 0; i < vp->size; i++) {
      uint8_t data = *bp++;
      crc = _crc_ccitt_update(crc, data);
      *tp++ = m_codec->encode4(data >> 4);
      *tp++ = m_codec->encode4(data);
    }
  }
  
  // Append the FCS, 16 bits before encoding (4 symbols after
  // encoding) Caution: VWI expects the _ones_complement_ of the CCITT 
  // CRC-16 as the FCS VWI sends FCS as low byte then hi byte
  crc = ~crc;
  *tp++ = m_codec->encode4(crc >> 4);
  *tp++ = m_codec->encode4(crc);
  *tp++ = m_codec->encode4(crc >> 12);
  *tp++ = m_codec->encode4(crc >> 8);

  // Total number of symbols to send
  m_length = m_codec->PREAMBLE_MAX + (count * 2);

  // Start the low level interrupt handler sending symbols
  return (begin());
}

bool 
VWI::Transmitter::send(const void* buf, uint8_t len, uint8_t cmd)
{
  // Check that the message is not too large
  if (len > PAYLOAD_MAX) return (false);
  VWI::header_t header;
  iovec_t vec[3];
  iovec_t* vp = vec;

  // Check for enhanced mode: add header with address and sequence number
  if (VWI::s_addr != 0L) {
    header.addr = s_addr;
    header.cmd = cmd;
    header.nr = m_nr++;
    iovec_arg(vp, &header, sizeof(header));
  }
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(vec));
}
