/**
 * @file Cosa/PinChangeInterrupt.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "Cosa/PinChangeInterrupt.hh"

// Define symbols for enable/disable pin change interrupts
#if defined(GIMSK)
#define PCICR GIMSK
#endif
#if !defined(PCIE0)
#define PCIE0 PCIE
#endif
#if defined(PCIE3)
#define PCIEN (_BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0))
#elif defined(PCIE2)
#define PCIEN (_BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0))
#elif defined(PCIE1)
#define PCIEN (_BV(PCIE1) | _BV(PCIE0))
#elif defined(PCIE0)
#define PCIEN (_BV(PCIE0))
#endif

PinChangeInterrupt* PinChangeInterrupt::s_pin[Board::PCMSK_MAX] = { NULL };
uint8_t PinChangeInterrupt::s_state[Board::PCMSK_MAX] = { 0 };

void
PinChangeInterrupt::enable()
{
  synchronized {
    *PCIMR() |= m_mask;
    if (m_next == NULL) {
      uint8_t ix;
#if defined(BOARD_ATMEGA2560)
      ix = m_pin - (m_pin < 24 ? 16 : 48);
#else
      ix = m_pin;
#endif
      ix = (ix >> 3);
      m_next = s_pin[ix];
      s_pin[ix] = this;
    }
  }
}

void
PinChangeInterrupt::disable()
{
  synchronized {
    *PCIMR() &= ~m_mask;
  }
}

void
PinChangeInterrupt::begin()
{
#if defined(BOARD_ATMEGA2560)
  s_state[0] = *Pin::PIN(16);
  s_state[1] = 0;
  s_state[2] = *Pin::PIN(64);
#else
  for (uint8_t i = 0; i < Board::PCMSK_MAX; i++)
    s_state[i] = *Pin::PIN(i << 3);
#endif
  synchronized {
    bit_mask_set(PCICR, PCIEN);
  }
}

void
PinChangeInterrupt::end()
{
  synchronized {
    bit_mask_clear(PCICR, PCIEN);
  }
}

void
PinChangeInterrupt::on_interrupt(uint8_t pcint, uint8_t mask, uint8_t base)
{
  uint8_t new_state = *Pin::PIN(base);
  uint8_t old_state = s_state[pcint];
  uint8_t changed = (new_state ^ old_state) & mask;

  for (PinChangeInterrupt* pin = s_pin[pcint]; pin != NULL; pin = pin->m_next)
    if ((pin->m_mask & changed)
	&& ((pin->m_mode == ON_CHANGE_MODE)
	    || pin->m_mode == ((pin->m_mask & new_state) == 0)))
      pin->on_interrupt();

  s_state[pcint] = new_state;
}

#define PCINT_ISR(vec,pcint,base)				\
ISR(PCINT ## vec ## _vect)					\
{								\
  PinChangeInterrupt::on_interrupt(pcint, PCMSK ## vec, base);	\
}

#if defined(BOARD_ATTINYX61)

ISR(PCINT0_vect)
{
  uint8_t mask;
  uint8_t ix;

  if (GIFR & _BV(INTF0)) {
    mask = PCMSK0;
    ix = 0;
  } else {
    mask = PCMSK1;
    ix = 1;
  }
  PinChangeInterrupt::on_interrupt(ix, mask, (ix << 3));
}

#elif defined(BOARD_ATTINYX5)

PCINT_ISR(0, 0, 0);

#elif defined(BOARD_ATTINYX4)

PCINT_ISR(0, 0, 0);
PCINT_ISR(1, 1, 8);

#elif defined(BOARD_ATMEGA328P)

PCINT_ISR(0, 1, 8);
PCINT_ISR(1, 2, 14);
PCINT_ISR(2, 0, 0);

#elif defined(BOARD_ATMEGA32U4)

PCINT_ISR(0, 0, 0);

#elif defined(BOARD_AT90USB1286)

PCINT_ISR(0, 0, 0);

#elif defined(BOARD_ATMEGA2560)

PCINT_ISR(0, 0, 16);
ISR(PCINT1_vect) {}
PCINT_ISR(2, 2, 64);

#elif defined(BOARD_ATMEGA1248P)

PCINT_ISR(0, 0, 0);
PCINT_ISR(1, 1, 8);
PCINT_ISR(2, 2, 16);
PCINT_ISR(3, 3, 24);

#elif defined(BOARD_ATMEGA256RFR2)

PCINT_ISR(0, 0, 0);
PCINT_ISR(1, 1, 8);
PCINT_ISR(2, 2, 16);

#endif
