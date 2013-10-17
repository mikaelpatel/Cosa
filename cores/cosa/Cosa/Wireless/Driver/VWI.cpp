/**
 * @file Cosa/Wireless/Driver/VWI.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2008-2013, Mike McCauley (Author/VirtualWire rev. 1.19)
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

#include "Cosa/Wireless/Driver/VWI.hh"
#include "Cosa/RTC.hh"
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

/** Prescale table for Timer1. Index is prescale setting */
static const uint16_t prescale[] __PROGMEM = {
#if defined(__ARDUINO_TINYX5__)
  0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384 
#else
  0, 1, 8, 64, 256, 1024
#endif
};

/**
 * Calculate timer setting, prescale and count value, given speed (bps),
 * number of bits in timer. Returns zero(0) if fails otherwise prescale
 * value/index, and timer top.
 * @param[in] speed bits per second, transmitter/receiver.
 * @param[in] bits number of bits in counter (8 or 16 bit timer).
 * @param[out] nticks timer top value.
 * @return prescale or zero(0).
 */
static uint8_t
timer_setting(uint16_t speed, uint8_t bits, uint16_t* nticks) 
{
  uint16_t max_ticks = (1 << bits) - 1;
  uint8_t res = 0;
  for (uint8_t i = membersof(prescale) - 1; i > 0; i--) {
    uint16_t scale = (uint16_t) pgm_read_word(&prescale[i]);
    uint16_t count = (F_CPU / scale) / speed;
    if (count > res && count < max_ticks) {
      *nticks = count;
      res = i;
    }
  }
  return (res);
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
VWI::Receiver::recv(uint8_t& src, void* buf, size_t len, uint32_t ms)
{
  // Wait until a valid message is available or timeout
  uint32_t start = RTC::millis();
  header_t* hp = (header_t*) (m_buffer + 1);
  do {
    while (!m_done && (ms == 0 || (RTC::since(start) < ms))) 
      Power::sleep(s_rf->m_mode);
    if (!m_done) return (-2);
  
    // Check the crc and the network and device destination address
    if (!is_valid_crc(m_buffer, m_length) ||
	(hp->network != s_rf->m_addr.network)  || 
	((hp->dest != 0) && (hp->dest != s_rf->m_addr.device))) {
      m_done = false;
    }
  } while (!m_done);

  // Sanity check message length
  size_t rxlen = m_length - sizeof(header_t) - 3;
  if (rxlen > len) return (-1);

  // Copy payload and source device address
  memcpy(buf, m_buffer + sizeof(header_t) + 1, rxlen);
  s_rf->m_dest = hp->dest;
  src = hp->src;
  
  // OK, got that message thanks
  m_done = false;

  // Return actual number of bytes received
  return (rxlen);
}

int
VWI::Transmitter::send(uint8_t dest, const iovec_t* vec)
{
  // Santiy check the io vector
  if (vec == NULL) return (-1);

  // Check that the message is not too large
  size_t len = 0;
  for (const iovec_t* vp = vec; vp->buf != 0; vp++)
    len += vp->size;
  if (len > PAYLOAD_MAX) return (-1);

  uint8_t *tp = m_buffer + m_codec->PREAMBLE_MAX;
  uint16_t crc = 0xffff;
  
  // Wait for transmitter to become available. Might be transmitting
  while (m_enabled) Power::sleep(s_rf->m_mode);

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
  uint8_t* bp = (uint8_t*) &header;
  for (uint8_t i = 0; i < sizeof(header); i++) {
    uint8_t data = *bp++;
    crc = _crc_ccitt_update(crc, data);
    *tp++ = m_codec->encode4(data >> 4);
    *tp++ = m_codec->encode4(data);
  }

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
  begin();
  return (len);
}

int
VWI::Transmitter::send(uint8_t dest, const void* buf, size_t len)
{
  if (len > PAYLOAD_MAX) return (-1);
  iovec_t vec[2];
  iovec_t* vp = vec;
  iovec_arg(vp, buf, len);
  iovec_end(vp);
  return (send(dest, vec));
}

/** Current transmitter/receiver for interrupt handler access */
VWI* VWI::s_rf = 0;

bool 
VWI::begin(const void* config)
{
  // Number of prescaled ticks needed
  uint16_t nticks = 0;

  // Bit values for prescale register: CS0[2:0]
  uint8_t prescaler;

#if defined(__ARDUINO_TINYX5__)
  // Figure out prescaler value and counter match value
  prescaler = timer_setting(m_speed * SAMPLES_PER_BIT, 8, &nticks);
  if (!prescaler) return (false);

  // Turn on CTC mode / Output Compare pins disconnected
  TCCR1 = _BV(PWM1A) | prescaler;

  // Number of ticks to count before firing interrupt
  OCR1A = uint8_t(nticks);
#else
  // Figure out prescaler value and counter match value
  prescaler = timer_setting(m_speed * SAMPLES_PER_BIT, 16, &nticks);
  if (!prescaler) return (false);

  // Output Compare pins disconnected, and turn on CTC mode
  TCCR1A = 0; 
  TCCR1B = _BV(WGM12) | prescaler;

  // Caution: special procedures for setting 16 bit regs
  // is handled by the compiler
  OCR1A = nticks;
#endif
  // Enable the interrupt handler
  powerup();

  return (true);
}

bool 
VWI::end()
{
  powerdown();
  return (true);
}

void 
VWI::powerup() 
{
  m_rx.begin();
  TIMSK1 |= _BV(OCIE1A);
}

void 
VWI::powerdown()
{
  while (m_tx.is_active()) Power::sleep(m_mode);
  m_tx.end();
  m_rx.end();
  TIMSK1 &= ~_BV(OCIE1A);
}

bool 
VWI::available()
{
  return (m_rx.available());
}

int 
VWI::send(uint8_t dest, const iovec_t* vec)
{
  return (m_tx.send(dest, vec));
}

int 
VWI::send(uint8_t dest, const void* buf, size_t len)
{
  return (m_tx.send(dest, buf, len));
}

int 
VWI::recv(uint8_t& src, void* buf, size_t len, uint32_t ms)
{
  return (m_rx.recv(src, buf, len, ms));
}

ISR(TIMER1_COMPA_vect)
{
  VWI::Transmitter* transmitter = &VWI::s_rf->m_tx;
  VWI::Receiver* receiver = &VWI::s_rf->m_rx;

  // Check if the receiver pin should be sampled
  if (receiver->m_enabled && !transmitter->m_enabled)
    receiver->m_sample = receiver->read();
    
  // Do transmitter stuff first to reduce transmitter bit jitter due 
  // to variable receiver processing
  if (transmitter->m_enabled && transmitter->m_sample++ == 0) {
    // Send next bit. Symbols are sent LSB first. Finished sending the
    // whole message? (after waiting one bit period since the last bit)
    if (transmitter->m_index >= transmitter->m_length) {
      transmitter->end();
    }
    else {
      transmitter->write(transmitter->m_buffer[transmitter->m_index] & 
			 (1 << transmitter->m_bit++));
      if (transmitter->m_bit >= transmitter->m_codec->BITS_PER_SYMBOL) {
	transmitter->m_bit = 0;
	transmitter->m_index++;
      }
    }
  }
  if (transmitter->m_sample >= VWI::SAMPLES_PER_BIT) 
    transmitter->m_sample = 0;
  if (receiver->m_enabled && !transmitter->m_enabled)
    receiver->PLL();
}

