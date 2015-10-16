/**
 * @file VWI.cpp
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

/** Prescale table for Timer1. Index is prescale setting */
static const uint16_t prescale[] __PROGMEM = {
#if defined(BOARD_ATTINYX5) || defined(BOARD_ATTINYX61)
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

VWI::Codec::Codec(uint8_t bits_per_symbol,
		  uint16_t start_symbol,
		  uint8_t preamble_max) :
  BITS_PER_SYMBOL(bits_per_symbol),
  START_SYMBOL(start_symbol),
  PREAMBLE_MAX(preamble_max),
  SYMBOL_MASK((1 << bits_per_symbol) - 1),
  BITS_MSB(1 << (bits_per_symbol*2 - 1))
{
}

/** Current transmitter/receiver for interrupt handler access */
VWI* VWI::s_rf = NULL;

bool
VWI::begin(const void* config)
{
  UNUSED(config);

  // Number of prescaled ticks needed
  uint16_t nticks = 0;

  // Bit values for prescale register: CS0[2:0]
  uint8_t prescaler;

  // Power up the timer
  Power::timer1_enable();

#if defined(BOARD_ATTINYX5)
  // Figure out prescaler value and counter match value
  prescaler = timer_setting(m_speed * SAMPLES_PER_BIT, 8, &nticks);
  if (!prescaler) return (false);

  // Turn on CTC mode / Output Compare pins disconnected
  TCCR1 = _BV(PWM1A) | prescaler;

  // Number of ticks to count before firing interrupt
  OCR1A = uint8_t(nticks);
#elif defined(BOARD_ATTINYX61)
  // Figure out prescaler value and counter match value
  prescaler = timer_setting(m_speed * SAMPLES_PER_BIT, 8, &nticks);
  if (!prescaler) return (false);

  // Turn on CTC mode / Output Compare pins disconnected
  TCCR1A = _BV(PWM1A);
  TCCR1A = prescaler;

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
  Power::timer1_disable();
  return (true);
}

void
VWI::powerup()
{
  if (m_rx != NULL) m_rx->begin();
  TIMSK1 |= _BV(OCIE1A);
}

void
VWI::powerdown()
{
  if (m_tx != NULL) {
    while (m_tx->is_active()) yield();
    m_tx->end();
  }
  if (m_rx != NULL) m_rx->end();
  TIMSK1 &= ~_BV(OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
  VWI::Transmitter* transmitter = VWI::s_rf->m_tx;
  VWI::Receiver* receiver = VWI::s_rf->m_rx;
  bool transmitting = (transmitter != NULL) && (transmitter->m_enabled);
  bool receiving = (receiver != NULL) && (receiver->m_enabled);

  // Check if the receiver pin should be sampled
  if (!transmitting && receiving)
    receiver->m_sample = receiver->read();

  // Do transmitter stuff first to reduce transmitter bit jitter due
  // to variable receiver processing
  if (transmitting && transmitter->m_sample++ == 0) {
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
  if (transmitter != NULL && transmitter->m_sample >= VWI::SAMPLES_PER_BIT)
    transmitter->m_sample = 0;

  // Check if the receiver should run the phase locked loop
  if (!transmitting && receiving)
    receiver->PLL();
}

