/**
 * @file Cosa/ExternalInterruptPin.cpp
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

#include "Cosa/ExternalInterruptPin.hh"

#if defined(__ARDUINO_STANDARD__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  m_ix = pin - Board::EXT0;
  ext[m_ix] = this;
  uint8_t ix = (m_ix << 1);
  bit_field_set(EICRA, 0b11 << ix, mode << ix);
}

#elif defined(__ARDUINO_MEGA__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  if (pin <= Board::EXT5) {
    m_ix = pin - Board::EXT4;
    uint8_t ix = (m_ix << 1);
    bit_field_set(EICRB, 0b11 << ix, mode << ix);
    m_ix += 4;
  }
  else {
    m_ix = pin - Board::EXT0;
    uint8_t ix = (m_ix << 1);
    bit_field_set(EICRA, 0b11 << ix, mode << ix);
  } 
  ext[m_ix] = this;
}

#elif defined(__ARDUINO_MIGHTY__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  if (pin == Board::EXT2) {
    m_ix = 2;
  } else {
    m_ix = pin - Board::EXT0;
  } 
  uint8_t ix = (m_ix << 1);
  bit_field_set(EICRA, 0b11 << ix, mode << ix);
  ext[m_ix] = this;
}

#elif defined(__ARDUINO_TINY__)

ExternalInterruptPin::
ExternalInterruptPin(Board::ExternalInterruptPin pin, Mode mode) :
  InputPin((Board::DigitalPin) pin)
{
  if (mode & PULLUP_MODE) {
    synchronized {
      *PORT() |= m_mask; 
    }
  }
  m_ix = 0;
  ext[m_ix] = this;
  bit_field_set(MCUCR, 0b11, mode);
}

#endif

ExternalInterruptPin* ExternalInterruptPin::ext[Board::EXT_MAX] = { 0 };

void 
ExternalInterruptPin::on_interrupt(uint16_t arg) 
{ 
  Event::push(Event::CHANGE_TYPE, this, arg);
}

ISR(INT0_vect)
{
  if (ExternalInterruptPin::ext[0] != 0) 
    ExternalInterruptPin::ext[0]->on_interrupt();
}

#if !defined(__ARDUINO_TINY__)

ISR(INT1_vect)
{
  if (ExternalInterruptPin::ext[1] != 0) 
    ExternalInterruptPin::ext[1]->on_interrupt();
}

#if !defined(__ARDUINO_STANDARD__)

ISR(INT2_vect)
{
  if (ExternalInterruptPin::ext[2] != 0) 
    ExternalInterruptPin::ext[2]->on_interrupt();
}

#if defined(__ARDUINO_MEGA__)

ISR(INT3_vect)
{
  if (ExternalInterruptPin::ext[3] != 0) 
    ExternalInterruptPin::ext[3]->on_interrupt();
}

ISR(INT4_vect)
{
  if (ExternalInterruptPin::ext[4] != 0) 
    ExternalInterruptPin::ext[4]->on_interrupt();
}

ISR(INT5_vect)
{
  if (ExternalInterruptPin::ext[5] != 0) 
    ExternalInterruptPin::ext[5]->on_interrupt();
}
#endif
#endif
#endif
