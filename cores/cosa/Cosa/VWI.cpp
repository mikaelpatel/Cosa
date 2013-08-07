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

uint8_t VWI::s_mode = 0;
uint16_t VWI::s_addr = 0L;

uint16_t 
VWI::CRC(uint8_t* ptr, uint8_t count)
{
  uint16_t crc = 0xffff;
  while (count-- > 0) 
    crc = _crc_ccitt_update(crc, *ptr++);
  return (crc);
}

/** Current transmitter/receiver for interrupt handler access */
VWI::Transmitter* VWI::s_transmitter = 0;
VWI::Receiver* VWI::s_receiver = 0;

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
VWI::begin(uint16_t speed, uint8_t mode)
{
  // Number of prescaled ticks needed
  uint16_t nticks = 0;

  // Bit values for prescale register: CS0[2:0]
  uint8_t prescaler;

  // Set sleep mode
  s_mode = mode;

#if defined(__ARDUINO_TINYX5__)
  // Figure out prescaler value and counter match value
  prescaler = timer_setting(speed * SAMPLES_PER_BIT, 8, &nticks);
  if (!prescaler) return (false);

  // Turn on CTC mode / Output Compare pins disconnected
  TCCR1 = _BV(PWM1A) | prescaler;

  // Number of ticks to count before firing interrupt
  OCR1A = uint8_t(nticks);
#else
  // Figure out prescaler value and counter match value
  prescaler = timer_setting(speed * SAMPLES_PER_BIT, 16, &nticks);
  if (!prescaler) return (false);

  // Output Compare pins disconnected, and turn on CTC mode
  TCCR1A = 0; 
  TCCR1B = _BV(WGM12) | prescaler;

  // Caution: special procedures for setting 16 bit regs
  // is handled by the compiler
  OCR1A = nticks;
#endif
  // Enable the interrupt handler
  enable();

  return (true);
}

void
VWI::enable()
{
  TIMSK1 |= _BV(OCIE1A);
}

void
VWI::disable()
{
  TIMSK1 &= ~_BV(OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
  VWI::Receiver* receiver = VWI::s_receiver;
  VWI::Transmitter* transmitter = VWI::s_transmitter;

  // Check if the receiver pin should be sampled
  if ((receiver != 0 && receiver->m_enabled)
      && (transmitter == 0 || !transmitter->m_enabled))
    receiver->m_sample = receiver->read();
    
  // Do transmitter stuff first to reduce transmitter bit jitter due 
  // to variable receiver processing
  if (transmitter != 0) {
    if (transmitter->m_enabled && transmitter->m_sample++ == 0) {
      // Send next bit. Symbols are sent LSB first. Finished sending the
      // whole message? (after waiting one bit period since the last bit)
      if (transmitter->m_index >= transmitter->m_length) {
	transmitter->end();
	transmitter->m_count++;
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
  }
  if ((receiver != 0 && receiver->m_enabled)
      && (transmitter == 0 || !transmitter->m_enabled))
    receiver->PLL();
}
