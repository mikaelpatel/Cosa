/**
 * @file VWI_Receiver.cpp
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
#include "Cosa/RTT.hh"
#include "Cosa/Power.hh"
#include <util/crc16.h>

/**
 * Calculate check sum for given buffer and number of bytes with CRC.
 * Return true(1) if equals correct CCITT 16b check sum else
 * false(0).
 * @param[in] ptr buffer pointer.
 * @param[in] count number of bytes in buffer.
 * @return bool.
 */
static bool
is_valid_crc(uint8_t* ptr, uint8_t count)
{
  uint16_t crc = 0xffff;
  while (count-- > 0)
    crc = _crc_ccitt_update(crc, *ptr++);
  return (crc == 0xf0b8);
}

void
VWI::Receiver::PLL()
{
  // Integrate each sample
  if (m_sample) m_integrator++;

  if (m_sample != m_last_sample) {
    // Transition, advance if ramp > TRANSITION otherwise retard
    m_pll_ramp +=
      ((m_pll_ramp < RAMP_TRANSITION) ? RAMP_INC_RETARD : RAMP_INC_ADVANCE);
    m_last_sample = m_sample;
  }
  else {
    // No transition: Advance ramp by standard INC (== MAX/BITS samples)
    m_pll_ramp += RAMP_INC;
  }
  if (m_pll_ramp >= RAMP_MAX) {
    // Add this to the MSB bit of rx_bits, LSB first. The last bits are kept
    m_bits >>= 1;

    // Check the integrator to see how many samples in this cycle were
    // high. If < 5 out of 8, then its declared a 0 bit, else a 1;
    if (m_integrator >= INTEGRATOR_THRESHOLD)
      m_bits |= m_codec->BITS_MSB;

    m_pll_ramp -= RAMP_MAX;

    // Clear the integral for the next cycle
    m_integrator = 0;

    if (m_active) {
      // We have the start symbol and now we are collecting message
      // bits for two symbols before decoding to a byte
      if (++m_bit_count >= (m_codec->BITS_PER_SYMBOL * 2)) {
	uint8_t data = m_codec->decode8(m_bits);

	// The first decoded byte is the byte count of the following
	// message the count includes the byte count and the 2
	// trailing FCS bytes.
	if (m_length == 0) {
	  // The first byte is the byte count. Check it for
	  // sensibility. It cant be less than min, since it includes
	  // the bytes count itself and the 2 byte FCS
	  m_count = data;
	  if (m_count < MESSAGE_MIN || m_count > MESSAGE_MAX) {
	    // Stupid message length, drop the whole thing
	    m_active = false;
	    return;
	  }
	}
	m_buffer[m_length++] = data;
	if (m_length >= m_count) {
	  // Got all the bytes now
	  m_active = false;
	  // Better come get it before the next one starts
	  m_done = true;
	}
	m_bit_count = 0;
      }
    }

    // Not in a message, see if we have a start symbol
    else if (m_bits == m_codec->START_SYMBOL) {
      // Have start symbol, start collecting message
      m_active = true;
      m_bit_count = 0;
      m_length = 0;
      // Too bad if you missed the last message
      m_done = false;
    }
  }
}

int
VWI::Receiver::recv(uint8_t& src, uint8_t& port,
		    void* buf, size_t len,
		    uint32_t ms)
{
  // Wait until a valid message is available or timeout
  uint32_t start = RTT::millis();
  header_t* hp = (header_t*) (m_buffer + 1);
  do {
    while (!m_done && (ms == 0 || (RTT::since(start) < ms))) yield();
    if (!m_done) return (ETIME);

    // Check the crc and the network and device destination address
    if (!is_valid_crc(m_buffer, m_length)
	|| (hp->network != s_rf->m_addr.network)
	|| ((hp->dest != BROADCAST) && (hp->dest != s_rf->m_addr.device))) {
      m_done = false;
    }
  } while (!m_done);

  // Sanity check message length
  size_t rxlen = m_length - sizeof(header_t) - 3;
  if (rxlen > len) return (EMSGSIZE);

  // Copy payload and source device address
  memcpy(buf, m_buffer + sizeof(header_t) + 1, rxlen);
  s_rf->m_dest = hp->dest;
  src = hp->src;
  port = hp->port;

  // OK, got that message thanks
  m_done = false;

  // Return actual number of bytes received
  return (rxlen);
}

int
VWI::Receiver::link_quality_indicator()
{
  uint32_t start = RTT::millis();
  while (is_clear());
  return (RTT::millis() - start);
}
