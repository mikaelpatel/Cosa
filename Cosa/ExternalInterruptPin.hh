/**
 * @file Cosa/ExternalInterruptPin.hh
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

#ifndef __COSA_EXTERNALINTERRUPTPIN_HH__
#define __COSA_EXTERNALINTERRUPTPIN_HH__

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/Pins.hh"
#include "Cosa/Event.hh"
#include "Cosa/Interrupt.hh"
#include "Cosa/Board.hh"

/**
 * Abstract external interrupt pin. Allows interrupt handling on 
 * the pin value changes. 
 */
class ExternalInterruptPin : 
  public InputPin, 
  public Event::Handler, 
  public Interrupt::Handler 
{
  friend void INT0_vect(void);
#if !defined(__ARDUINO_TINY__)
  friend void INT1_vect(void);
#if !defined(__ARDUINO_STANDARD__)
  friend void INT2_vect(void);
#if defined(__ARDUINO_MEGA__)
  friend void INT3_vect(void);
  friend void INT4_vect(void);
  friend void INT5_vect(void);
#endif
#endif
#endif
private:
  static ExternalInterruptPin* ext[Board::EXT_MAX];
  uint8_t m_ix;

public:
  enum Mode {
    ON_LOW_LEVEL_MODE = 0,
    ON_CHANGE_MODE = _BV(ISC00),
    ON_FALLING_MODE = _BV(ISC01),
    ON_RISING_MODE = (_BV(ISC01) | _BV(ISC00)),
    PULLUP_MODE = 4
  } __attribute__((packed));

  /**
   * Construct external interrupt pin with given pin number and mode.
   * @param[in] pin pin number.
   * @param[in] mode pin mode.
   */
  ExternalInterruptPin(Board::ExternalInterruptPin pin, 
		       Mode mode = ON_CHANGE_MODE);

  /**
   * Enable interrupt pin change detection and interrupt handler.
   */
  void enable() 
  { 
    synchronized {
#if defined(__ARDUINO_TINY__)
      bit_set(GIMSK, INT0); 
#else
      bit_set(EIMSK, m_ix); 
#endif
    }
  }

  /**
   * Disable interrupt pin change detection.
   */
  void disable() 
  { 
    synchronized {
#if defined(__ARDUINO_TINY__)
      bit_clear(GIMSK, INT0);
#else
      bit_clear(EIMSK, m_ix); 
#endif
    }
  }

  /**
   * @override
   * Default interrupt service on external interrupt pin change.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0);
};
#endif
