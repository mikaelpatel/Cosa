/**
 * @file Cosa/VWI/VWI_Receiver.cpp
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
	    m_bad++;
	    return;
	  }
	}
	m_buffer[m_length++] = data;
	if (m_length >= m_count) {
	  // Got all the bytes now
	  m_active = false;
	  m_good++;
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

VWI::Receiver::Receiver(Board::DigitalPin pin, Codec* codec) : 
  InputPin(pin),
  m_codec(codec),
  m_mask(0xffffU)
{
  VWI::s_receiver = this;
}

bool 
VWI::Receiver::begin(uint16_t mask)
{
  RTC::begin();
  m_mask = mask;
  m_enabled = true;
  m_active = false;
  return (true);
}

bool 
VWI::Receiver::end()
{
  m_enabled = false;
  return (true);
}

bool 
VWI::Receiver::await(uint32_t ms)
{
  // Allow low power mode while waiting
  uint32_t start = RTC::millis();
  while (!m_done && (ms == 0 || (RTC::since(start) < ms))) 
    Power::sleep(s_mode);
  return (m_done);
}

int8_t
VWI::Receiver::recv(void* buf, uint8_t len, uint32_t ms)
{
  // Message available?
  if (!m_done && (ms == 0 || !await(ms))) return (0);

  // Check if enhanced mode and correct sub-net address
  if (VWI::s_addr != 0) {
    VWI::header_t* hp = (VWI::header_t*) (m_buffer + 1);
    if ((hp->addr & m_mask) != VWI::s_addr) {
      m_done = false;
      return (-2);
    }
  }
  uint8_t rxlen = m_length - 3;
  if (len > rxlen) len = rxlen;
  memcpy(buf, m_buffer + 1, len);
  
  // OK, got that message thanks
  m_done = false;
    
  // Message check-sum error
  if (CRC(m_buffer, m_length) != CHECK_SUM) return (-1);
    
  // Return actual number of bytes received
  return (len);
}
