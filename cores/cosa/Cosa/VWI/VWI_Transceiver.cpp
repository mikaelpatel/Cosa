/**
 * @file Cosa/VWI/VWI_Transceiver.cpp
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

#include "Cosa/VWI.hh"
#include "Cosa/RTC.hh"
#include "Cosa/Power.hh"
#include <util/crc16.h>

VWI::Transceiver::Transceiver(Board::DigitalPin rx_pin, 
			      Board::DigitalPin tx_pin, 
			      VWI::Codec* codec) :
  rx(rx_pin, codec),
  tx(tx_pin, codec)
{
}

bool 
VWI::Transceiver::begin(uint16_t mask)
{
  return (rx.begin(mask) && tx.begin());
}

bool 
VWI::Transceiver::end()
{
  return (rx.end() && tx.end());
}

int8_t 
VWI::Transceiver::recv(void* buf, uint8_t len, uint32_t ms)
{
  // Receiver enhanced message
  rx.await();
  int8_t res = rx.recv(buf, len, ms);
  if (res <= 0) return (res);

  // Check for no acknowledgement
  VWI::header_t* hp = (VWI::header_t*) buf;
  uint8_t nack = hp->cmd & NACK;
  hp->cmd = hp->cmd & ~NACK;
  if (nack) return (res);
  
  // Send acknowledgement; retransmit the header
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, sizeof(VWI::header_t));
  iovec_end(vp);
  tx.send(vec);
  return (res);
}

int8_t
VWI::Transceiver::send(const void* buf, uint8_t len, uint8_t cmd, Mode mode)
{
  uint8_t nr = tx.get_next_nr();
  uint8_t retrans = 0;

  // Adjust command with acknowledge mode and sent the message 
  int8_t res = tx.send(buf, len, cmd | mode);
  if (mode == NACK) return (res);

  // Wait for an acknowledgement. Retransmit if not receive within timeout
  while (retrans < RETRANS_MAX) {
    VWI::header_t ack;
    tx.await();
    int8_t len = rx.recv(&ack, sizeof(ack), TIMEOUT);
    // Check acknowledgement and return if valid with number of transmissions
    if ((len == sizeof(ack)) && (ack.nr == nr) && (ack.addr == VWI::s_addr))
      return (retrans + 1);
    // Otherwise on timeout resend the message
    if (len == 0) {
      retrans += 1;
      tx.resend();
    }
  }
  return (-1);
}
