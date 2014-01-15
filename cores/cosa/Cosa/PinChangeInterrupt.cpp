/**
 * @file Cosa/PinChangeInterrupt.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/PinChangeInterrupt.hh"

PinChangeInterrupt* PinChangeInterrupt::instance[Board::PCINT_MAX * CHARBITS] = { 0 };
uint8_t             PinChangeInterrupt::state   [Board::PCINT_MAX           ] = { 0 };

void 
PinChangeInterrupt::enable() 
{ 
  synchronized {
    *PCIMR() |= m_mask;
#if defined(__ARDUINO_MEGA__)
    uint8_t ix   = m_pin - (m_pin < 24 ? 16 : 48);
    instance[ix] = this;
#else
    instance[m_pin] = this;
#endif
  }
}

void 
PinChangeInterrupt::disable() 
{ 
  synchronized {
    *PCIMR() &= ~m_mask;
#if defined(__ARDUINO_MEGA__)
    uint8_t ix   = m_pin - (m_pin < 24 ? 16 : 48);
    instance[ix] = 0;
#else
    instance[m_pin] = 0;
#endif
  }
}

void 
PinChangeInterrupt::begin()
{
#if defined(__ARDUINO_MEGA__)
  state[0] = *Pin::PIN(16);
  state[1] = 0;
  state[2] = *Pin::PIN(64);
#else
  for (uint8_t i = 0; i < Board::PCINT_MAX; i++)
    state[i] = *Pin::PIN(i << 3);
#endif

  synchronized {
#if defined(__ARDUINO_TINYX5__)
    bit_set(GIMSK, PCIE);
#elif defined(__ARDUINO_TINYX4__) || defined(__ARDUINO_TINYX61__)
    bit_mask_set(GIMSK, _BV(PCIE1) | _BV(PCIE0));
#elif defined(__ARDUINO_STANDARD_USB__)
    bit_mask_set(PCICR, _BV(PCIE0));
#elif defined(__ARDUINO_MIGHTY__)
    bit_mask_set(PCICR, _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#else
    bit_mask_set(PCICR, _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#endif
  }
}

void 
PinChangeInterrupt::end()
{
  synchronized {
#if defined(__ARDUINO_TINYX5__)
    bit_clear(GIMSK, PCIE);
#elif defined(__ARDUINO_TINYX4__) || defined(__ARDUINO_TINYX61__)
    bit_mask_clear(GIMSK, _BV(PCIE1) | _BV(PCIE0));
#elif defined(__ARDUINO_STANDARD_USB__)
    bit_mask_clear(PCICR, _BV(PCIE0));
#elif defined(__ARDUINO_MIGHTY__)
    bit_mask_clear(PCICR, _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#else
    bit_mask_clear(PCICR, _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#endif
  }
}

void
PinChangeInterrupt::on_interrupt( uint8_t pcint, uint8_t mask, uint8_t base_pin )
{
  uint8_t newState      = *Pin::PIN(base_pin);
  uint8_t changed       = (newState ^ state[pcint]) & mask;
  uint8_t base_instance = pcint << 3;

  for (uint8_t i = 0; changed && (i < CHARBITS); i++) {
    if ((changed & 1) && (instance[base_instance + i] != NULL)) {
      instance[base_instance + i]->on_interrupt();
    }
    changed >>= 1;
  }

  state[pcint] = newState;
}

#if defined(__ARDUINO_TINYX5__)

ISR(PCINT0_vect)
{
  PinChangeInterrupt::on_interrupt( 0, PCMSK0, 0 );
}

#elif defined(__ARDUINO_TINYX4__)

ISR(PCINT0_vect)
{
  PinChangeInterrupt::on_interrupt(0, PCMSK0, 0);
}

ISR(PCINT1_vect)
{
  PinChangeInterrupt::on_interrupt(1, PCMSK1, 8);
}

#elif defined(__ARDUINO_TINYX61__)

ISR(PCINT0_vect)
{
  uint8_t mask;
  uint8_t ix;

  if (GIFR & _BV(INTF0)) {
    mask = PCMSK0;
    ix   = 0;
  } else {
    mask = PCMSK1;
    ix   = 1;
  }
  PinChangeInterrupt::on_interrupt( ix, mask, (ix << 3) );
}

#elif defined(__ARDUINO_STANDARD__)

ISR(PCINT0_vect)
{
  PinChangeInterrupt::on_interrupt(1, PCMSK0, 8); // only 8..13 are available
}

ISR(PCINT1_vect)
{
  PinChangeInterrupt::on_interrupt(2, PCMSK1, 14); // instance[22..23] not used
}

ISR(PCINT2_vect)
{
  PinChangeInterrupt::on_interrupt(0, PCMSK2, 0);
}

#elif defined(__ARDUINO_STANDARD_USB__)

ISR(PCINT0_vect)
{
  PinChangeInterrupt::on_interrupt(0, PCMSK0, 0);
}

#elif defined(__ARDUINO_MEGA__)

ISR(PCINT0_vect)
{
  PinChangeInterrupt::on_interrupt(0, PCMSK0, 16);
}

/*
  Although not implemented, these PCINTs are actually available:
    PCINT8  is PE0 (RXD0) aka RX0 aka D0
    PCINT9  is PJ0 (RXD3) aka D15
    PCINT10 is PJ1 (TXD3) aka D14
    (PCINT11..15 are not available)

  Because of the non-sequential pins, handling would be quite different
*/

ISR(PCINT1_vect)
{
//  PinChangeInterrupt::on_interrupt(1, PCMSK1, ??);
}

ISR(PCINT2_vect)
{
  PinChangeInterrupt::on_interrupt(2, PCMSK2, 64);
}

#elif defined(__ARDUINO_MIGHTY__)

ISR(PCINT0_vect)
{
  PinChangeInterrupt::on_interrupt(0, PCMSK0, 0);
}

ISR(PCINT1_vect)
{
  PinChangeInterrupt::on_interrupt(1, PCMSK1, 8);
}

ISR(PCINT2_vect)
{
  PinChangeInterrupt::on_interrupt(2, PCMSK2, 16);
}

ISR(PCINT3_vect)
{
  PinChangeInterrupt::on_interrupt(3, PCMSK3);
}
#endif
