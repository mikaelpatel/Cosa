/**
 * @file Cosa/InterruptPin.cpp
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

#include "Cosa/InterruptPin.hh"

InterruptPin* InterruptPin::pin[Board::PIN_MAX] = { 0 };
uint8_t InterruptPin::state[Board::PCINT_MAX] = { 0 };

void 
InterruptPin::begin()
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
#elif defined(__ARDUINO_TINYX4__)
    bit_mask_set(GIMSK, _BV(PCIE1) | _BV(PCIE0));
#elif defined(__ARDUINO_MIGHTY__)
    bit_mask_set(PCICR, _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#else
    bit_mask_set(PCICR, _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#endif
  }
}

void 
InterruptPin::end()
{
  synchronized {
#if defined(__ARDUINO_TINYX5__)
    bit_clear(GIMSK, PCIE);
#elif defined(__ARDUINO_TINYX4__)
    bit_mask_clear(GIMSK, _BV(PCIE1) | _BV(PCIE0));
#elif defined(__ARDUINO_MIGHTY__)
    bit_mask_clear(PCICR, _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#else
    bit_mask_clear(PCICR, _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0));
#endif
  }
}

void
InterruptPin::on_interrupt(uint16_t arg)
{ 
  Event::push(Event::CHANGE_TYPE, this, arg);
}

#if defined(__ARDUINO_TINYX5__)

ISR(PCINT0_vect)
{
  uint8_t mask = PCMSK0;
  uint8_t state = *Pin::PIN(0);
  uint8_t changed = (state ^ InterruptPin::state[0]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i] != 0)) {
      InterruptPin::pin[i]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[0] = state;
}

#elif defined(__ARDUINO_TINYX4__)

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
  uint8_t px = (ix << 3);
  uint8_t state = *Pin::PIN(px);
  uint8_t changed = (state ^ InterruptPin::state[ix]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i + px] != 0)) {
      InterruptPin::pin[i + px]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[ix] = state;
}

ISR(PCINT0_vect)
{
  InterruptPin::on_interrupt(0, PCMSK0);
}

ISR(PCINT1_vect)
{
  InterruptPin::on_interrupt(1, PCMSK1);
}

#elif defined(__ARDUINO_STANDARD__)

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
  uint8_t px = (ix << 3) - (ix < 2 ? 0 : 2);
  uint8_t state = *Pin::PIN(px);
  uint8_t changed = (state ^ InterruptPin::state[ix]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i + px] != 0)) {
      InterruptPin::pin[i + px]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[ix] = state;
}

ISR(PCINT0_vect)
{
  InterruptPin::on_interrupt(1, PCMSK0);
}

ISR(PCINT1_vect)
{
  InterruptPin::on_interrupt(2, PCMSK1);
}

ISR(PCINT2_vect)
{
  InterruptPin::on_interrupt(0, PCMSK2);
}

#elif defined(__ARDUINO_MEGA__)

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
  uint8_t px = (ix << 3);
  uint8_t rx = (ix == 0 ? 16 : 64);
  uint8_t state = *Pin::PIN(rx);
  uint8_t changed = (state ^ InterruptPin::state[ix]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i + px] != 0)) {
      InterruptPin::pin[i + px]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[ix] = state;
}

ISR(PCINT0_vect)
{
  InterruptPin::on_interrupt(0, PCMSK0);
}

ISR(PCINT1_vect)
{
  InterruptPin::on_interrupt(1, PCMSK1);
}

ISR(PCINT2_vect)
{
  InterruptPin::on_interrupt(2, PCMSK2);
}

#elif defined(__ARDUINO_MIGHTY__)

void
InterruptPin::on_interrupt(uint8_t ix, uint8_t mask)
{
  uint8_t px = (ix << 3);
  uint8_t state = *Pin::PIN(px);
  uint8_t changed = (state ^ InterruptPin::state[ix]) & mask;
  for (uint8_t i = 0; i < CHARBITS; i++) {
    if ((changed & 1) && (InterruptPin::pin[i + px] != 0)) {
      InterruptPin::pin[i + px]->on_interrupt();
    }
    changed >>= 1;
  }
  InterruptPin::state[ix] = state;
}

ISR(PCINT0_vect)
{
  InterruptPin::on_interrupt(0, PCMSK0);
}

ISR(PCINT1_vect)
{
  InterruptPin::on_interrupt(1, PCMSK1);
}

ISR(PCINT2_vect)
{
  InterruptPin::on_interrupt(2, PCMSK2);
}

ISR(PCINT3_vect)
{
  InterruptPin::on_interrupt(3, PCMSK3);
}
#endif
