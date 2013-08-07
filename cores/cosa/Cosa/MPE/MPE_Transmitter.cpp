/**
 * @file Cosa/MPE/MPE_Transmitter.cpp
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

#include "Cosa/MPE.hh"
#include "Cosa/Power.hh"
#include <util/crc16.h>

MPE::Transmitter* MPE::Transmitter::transmitter = 0;

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
  uint8_t i;
  for (i = membersof(prescale) - 1; i > 0; i--) {
    uint16_t scale = (uint16_t) pgm_read_word(&prescale[i]);
    uint16_t count = (F_CPU / scale) / speed;
    if (count > res && count < max_ticks) {
      *nticks = count;
      res = i;
    }
  }
  return (res);
}

bool 
MPE::Transmitter::begin(uint16_t speed, uint8_t mode)
{
  uint16_t nticks = 0;
  uint8_t prescaler;
  m_mode = mode;

  // Calculate a valid prescale and set timer
#if defined(__ARDUINO_TINYX5__)
  prescaler = timer_setting(speed, 8, &nticks);
  if (!prescaler) return (false);
  TCCR1 = _BV(PWM1A) | prescaler;
  OCR1A = uint8_t(nticks);
#else
  prescaler = timer_setting(speed, 16, &nticks);
  if (!prescaler) return (false);
  TCCR1A = 0; 
  TCCR1B = _BV(WGM12) | prescaler;
  OCR1A = nticks;
#endif
  transmitter = this;
  return (true);
}

MPE::Transmitter::Transmitter(Board::DigitalPin pin) :
  OutputPin(pin),
  Interrupt::Handler(),
  m_enabled(false)
{
  // The preamble for the frame. Inspired by Ethernet with a twist for RF433
  // modules. First byte is a wakeup call, second a synchronization byte,
  // and the third the start symbol (which is an illegal MPE code).
  m_buffer[0] = 0x7f;
  m_buffer[1] = 0x55;
  m_buffer[2] = 0x57;
}

void 
MPE::Transmitter::await()
{
  while (m_enabled) Power::sleep(m_mode);
}

// Mapping table from 4b to 8b Manchester Phase Encoding (MPE)
const uint8_t symbols[] __PROGMEM = {
  0b01010101,
  0b01010110,
  0b01011001,
  0b01011010,
  0b01100101,
  0b01100110,
  0b01101001,
  0b01101010,
  0b10010101,
  0b10010110,
  0b10011001,
  0b10011010,
  0b10100101,
  0b10100110,
  0b10101001,
  0b10101010
};

/**
 * Table driven 4b to 8b Manchester Phase Encoding (MPE).
 * @param[in] nibble to encode.
 * @return 8b encoding.
 */
inline uint8_t encode(uint8_t nibble)
{
  return (pgm_read_byte(&symbols[nibble & 0xf]));
}

bool 
MPE::Transmitter::send(const void* buf, uint8_t len)
{
  // Check that the payload is valid size
  if (len > PAYLOAD_MAX) return (false);

  // Wait for the transitter to become available
  await();

  // Encode the frame into the transmitter buffer; first the frame size
  uint8_t *tp = m_buffer + PREAMBLE;
  uint8_t count = len + FRAMING;
  uint16_t crc = 0xffff;
  crc = _crc_ccitt_update(crc, count);
  *tp++ = encode(count >> 4);
  *tp++ = encode(count);

  // Second the payload
  uint8_t *bp = (uint8_t*) buf;
  for (uint8_t i = 0; i < len; i++) {
    uint8_t data = *bp++;
    crc = _crc_ccitt_update(crc, data);
    *tp++ = encode(data >> 4);
    *tp++ = encode(data);
  }

  // And last the checksum; this is little-endian
  crc = ~crc;
  *tp++ = encode(crc >> 4);
  *tp++ = encode(crc);
  *tp++ = encode(crc >> 12);
  *tp++ = encode(crc >> 8);

  // Set up the transmitter state
  m_length = (count * 2) + PREAMBLE;
  m_index = 0;
  m_bit = 0x80;
  m_enabled = true;
  TIMSK1 |= _BV(OCIE1A);

  return (true);
}

void 
MPE::Transmitter::on_interrupt(uint16_t arg)
{
  // Transmit the next bit. Check if there are more bits in this byte
  write(m_buffer[m_index] & m_bit);
  m_bit >>= 1;
  if (m_bit != 0) return;

  // Setup for the next byte. Check if there are more bytes in the buffer
  m_bit = 0x80;
  m_index += 1;
  if (m_index < m_length) return;

  // Turn off the transmitter. Signal ready for next message
  TIMSK1 &= ~_BV(OCIE1A);
  clear();
  m_enabled = false;
}

ISR(TIMER1_COMPA_vect)
{
  MPE::Transmitter* transmitter = MPE::Transmitter::transmitter;
  if (transmitter == 0) return;
  if (!transmitter->m_enabled) return;
  transmitter->on_interrupt();
}
