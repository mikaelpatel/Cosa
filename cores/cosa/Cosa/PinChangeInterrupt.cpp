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
  // Enable in pin change mask register and add to list of handlers
  synchronized {
    *PCIMR() |= m_mask;
    if (m_next == NULL) {
      uint8_t ix = PCIMR() - &PCMSK0;
      if (ix >= Board::PCMSK_MAX) ix = Board::PCMSK_MAX - 1;
      m_next = s_pin[ix];
      s_pin[ix] = this;
    }
  }
}

void
PinChangeInterrupt::disable()
{
  synchronized *PCIMR() &= ~m_mask;
}

void
PinChangeInterrupt::begin()
{
  // Initiate the pin state vector
#if defined(BOARD_ATTINYX61)
  s_state[0] = PINA;
  s_state[1] = PINB;
#elif defined(BOARD_ATTINYX5)
  s_state[0] = PINB;
#elif defined(BOARD_ATTINYX4)
  s_state[0] = PINA;
  s_state[1] = PINB;
#elif defined(BOARD_ATMEGA328P)
  s_state[0] = PINB;
  s_state[1] = PINC;
  s_state[2] = PIND;
#elif defined(BOARD_ATMEGA32U4)
  s_state[0] = PINB;
#elif defined(BOARD_AT90USB1286)
  s_state[0] = PINB;
#elif defined(BOARD_ATMEGA2560)
  s_state[0] = PINB;
  s_state[1] = PINJ;
  s_state[2] = PINK;
#elif defined(BOARD_ATMEGA1248P)
  s_state[0] = PINA;
  s_state[1] = PINB;
  s_state[2] = PINC;
  s_state[3] = PIND;
#elif defined(BOARD_ATMEGA256RFR2)
  s_state[0] = PINB;
  s_state[1] = PINE;
#endif

  // Enable the pin change interrupt(s)
  synchronized bit_mask_set(PCICR, PCIEN);
}

void
PinChangeInterrupt::end()
{
  // Disable the pin change interrupt(s)
  synchronized bit_mask_clear(PCICR, PCIEN);
}

void
PinChangeInterrupt::on_interrupt(uint8_t vec, uint8_t mask, uint8_t port)
{
  uint8_t old_state = s_state[vec];
  uint8_t new_state = port;
  uint8_t changed = (new_state ^ old_state) & mask;

  // Find the interrupt handler for the changed value and check mode
  for (PinChangeInterrupt* pin = s_pin[vec]; pin != NULL; pin = pin->m_next)
    if ((pin->m_mask & changed)
	&& ((pin->m_mode == ON_CHANGE_MODE)
	    || pin->m_mode == ((pin->m_mask & new_state) == 0)))
      pin->on_interrupt();

  // Save the new pin state
  s_state[vec] = new_state;
}

#define PCINT_ISR(vec,pin)					\
ISR(PCINT ## vec ## _vect)					\
{								\
  PinChangeInterrupt::on_interrupt(vec, PCMSK ## vec, pin);	\
}

#if defined(BOARD_ATTINYX61)

ISR(PCINT0_vect)
{
  if (GIFR & _BV(INTF0)) {
    PinChangeInterrupt::on_interrupt(0, PCMSK0, PINA);
  } else {
    PinChangeInterrupt::on_interrupt(1, PCMSK1, PINB);
  }
}

#elif defined(BOARD_ATTINYX5)

PCINT_ISR(0, PINB);

#elif defined(BOARD_ATTINYX4)

PCINT_ISR(0, PINA);
PCINT_ISR(1, PINB);

#elif defined(BOARD_ATMEGA328P)

PCINT_ISR(0, PINB);
PCINT_ISR(1, PINC);
PCINT_ISR(2, PIND);

#elif defined(BOARD_ATMEGA32U4)

PCINT_ISR(0, PINB);

#elif defined(BOARD_AT90USB1286)

PCINT_ISR(0, PINB);

#elif defined(BOARD_ATMEGA2560)

PCINT_ISR(0, PINB);
PCINT_ISR(1, PINJ);
PCINT_ISR(2, PINK);

#elif defined(BOARD_ATMEGA1248P)

PCINT_ISR(0, PINA);
PCINT_ISR(1, PINB);
PCINT_ISR(2, PINC);
PCINT_ISR(3, PIND);

#elif defined(BOARD_ATMEGA256RFR2)

PCINT_ISR(0, PINB);
PCINT_ISR(1, PINE);

#endif
