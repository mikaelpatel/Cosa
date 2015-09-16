/**
 * @file Cosa/ExternalInterrupt.hh
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

#ifndef COSA_EXTERNAL_INTERRUPT_HH
#define COSA_EXTERNAL_INTERRUPT_HH

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/IOPin.hh"
#include "Cosa/Interrupt.hh"

/**
 * Abstract external interrupt pin. Allows interrupt handling on
 * the pin value changes.
 */
class ExternalInterrupt : public IOPin, public Interrupt::Handler {
public:
  enum InterruptMode {
    ON_LOW_LEVEL_MODE = 0,
    ON_CHANGE_MODE = _BV(ISC00),
    ON_FALLING_MODE = _BV(ISC01),
    ON_RISING_MODE = (_BV(ISC01) | _BV(ISC00))
  } __attribute__((packed));

  /**
   * Construct external interrupt pin with given pin number, mode and
   * pullup flag.
   * @param[in] pin number.
   * @param[in] mode pin mode (Default ON_CHANGE_MODE).
   * @param[in] pullup flag (Default false).
   */
  ExternalInterrupt(Board::ExternalInterruptPin pin,
		    InterruptMode mode = ON_CHANGE_MODE,
		    bool pullup = false);

  /**
   * @override{Interrupt::Handler}
   * Interrupt service callback on external interrupt pin change.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0)
  {
    UNUSED(arg);
  }

  /**
   * @override{Interrupt::Handler}
   * Enable external interrupt handler.
   * @note atomic
   */
  virtual void enable();

  /**
   * @override{Interrupt::Handler}
   * Disable external interrupt handler.
   * @note atomic
   */
  virtual void disable();

  /**
   * @override{Interrupt::Handler}
   * Clear external interrupt flag.
   * @note atomic
   */
  virtual void clear();

private:
  /** External interrupt pin map. */
  static ExternalInterrupt* ext[Board::EXT_MAX];

  /** External interrupt mask. */
  uint8_t m_ix;

  friend void INT0_vect(void);
#if defined(INT1_vect)
  friend void INT1_vect(void);
#endif
#if defined(INT2_vect)
  friend void INT2_vect(void);
#endif
#if defined(INT3_vect)
  friend void INT3_vect(void);
#endif
#if defined(INT4_vect)
  friend void INT4_vect(void);
#endif
#if defined(INT5_vect)
  friend void INT5_vect(void);
#endif
#if defined(INT6_vect)
  friend void INT6_vect(void);
#endif
#if defined(INT7_vect)
  friend void INT7_vect(void);
#endif
};

#endif
