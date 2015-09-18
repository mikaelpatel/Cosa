/**
 * @file VWI_Transmitter.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire rev. 1.19)
 * Copyright (C) 2013-2015, Mikael Patel (Cosa C++ port and refactoring)
 * Copyright (C) 2015, Mikael Patel (RF433 link quality indicator)
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

#include "VWI.hh"
#include <util/crc16.h>

int
VWI::Transmitter::send(uint8_t dest, uint8_t port, const iovec_t* vec)
{
  // Santiy check the io vector
  if (UNLIKELY(vec == NULL)) return (EINVAL);

  // Check that the message is not too large
  size_t len = iovec_size(vec);
  if (UNLIKELY(len > PAYLOAD_MAX)) return (EMSGSIZE);

  uint8_t *tp = m_buffer + m_codec->PREAMBLE_MAX;
  uint16_t crc = 0xffff;

  // Wait for transmitter to become available. Might be transmitting
  while (m_enabled) yield();

  // Encode the message total length = length(1)+header(4)+payload(len)+crc(2)
  uint8_t count = 1 + sizeof(header_t) + len + 2;
  crc = _crc_ccitt_update(crc, count);
  *tp++ = m_codec->encode4(count >> 4);
  *tp++ = m_codec->encode4(count);

  // Encode the message header
  header_t header;
  header.network = s_rf->m_addr.network;
  header.src = s_rf->m_addr.device;
  header.dest = dest;
  header.port = port;
  uint8_t* bp = (uint8_t*) &header;
  for (uint8_t i = 0; i < sizeof(header); i++) {
    uint8_t data = *bp++;
    crc = _crc_ccitt_update(crc, data);
    *tp++ = m_codec->encode4(data >> 4);
    *tp++ = m_codec->encode4(data);
  }

  // Encode the message into symbols. Each byte is converted into
  // 2 symbols, high nybble first, low nybble second
  for (const iovec_t* vp = vec; vp->buf != NULL; vp++) {
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
  begin();
  return (len);
}

int
VWI::Transmitter::send(uint8_t dest, uint8_t port, const void* buf, size_t len)
{
  if (UNLIKELY(len > PAYLOAD_MAX)) return (EMSGSIZE);
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(dest, port, vec));
}
