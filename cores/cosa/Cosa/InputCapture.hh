/**
 * @file Cosa/InputCapture.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_INPUT_CAPTURE_HH
#define COSA_INPUT_CAPTURE_HH

#include "Cosa/Types.h"

#if !defined(BOARD_ATTINY)
#include "Cosa/Interrupt.hh"
#include "Cosa/Power.hh"

/**
 * Input Capture Unit. Allows timer capture and interrupt handling on
 * the input capture pin value changes. The pin is hardwired (ICP1/PB0
 * on ATmega328p, D8).
 *
 * @section Limitations
 * Uses Timer1. Cannot be used with libraries that use the same Timer;
 * Tone, VWI. Compiles for Mega but the ICP1 pin is not available on
 * the Arduino Mega board.
 */
class InputCapture : public Interrupt::Handler {
public:
  enum InterruptMode {
    ON_FALLING_MODE,
    ON_RISING_MODE
  } __attribute__((packed));

  /**
   * Construct input capture unit with given capture mode and
   * with no prescale. Pin is D8 on ATmega328 based boards.
   * @param[in] mode capture mode (Default ON_FALLING_MODE).
   */
  InputCapture(InterruptMode mode = ON_FALLING_MODE);

  /**
   * Start input capture timer.
   */
  static void begin()
  {
    Power::timer1_enable();
  }

  /**
   * Stop input capture timer.
   */
  static void end()
  {
    Power::timer1_disable();
  }

  /**
   * Get current capture mode.
   * @return mode.
   */
  InterruptMode mode() const
  {
    // Check trigger mode setting
    if (TCCR1B & _BV(ICES1))
      return (ON_RISING_MODE);
    return (ON_FALLING_MODE);
  }

  /**
   * Set capture mode.
   * @param[in] new_mode.
   */
  void mode(InterruptMode mode)
  {
    // Set trigger on rising or falling on input capture pin
    if (mode == ON_RISING_MODE)
      TCCR1B |= _BV(ICES1);
    else
      TCCR1B &= ~_BV(ICES1);
  }

  /**
   * Get current timer count.
   * @return mode.
   */
  uint16_t count()
  {
    uint16_t res;
    synchronized res = TCNT1;
    return (res);
  }

  /**
   * @override{Interrupt::Handler}
   * Interrupt service callback on input capture. The parameter arg
   * contains the timer value. The timer is incremented with the
   * system clock (prescale is 1).
   * @param[in] arg timer count on event.
   */
  virtual void on_interrupt(uint16_t arg = 0);

  /**
   * @override{Interrupt::Handler}
   * Enable input capture handler.
   * @note atomic
   */
  virtual void enable();

  /**
   * @override{Interrupt::Handler}
   * Disable input capture handler.
   * @note atomic
   */
  virtual void disable();

  /**
   * @override{Interrupt::Handler}
   * Clear input capture interrupt flag.
   * @note atomic
   */
  virtual void clear();

private:
  static InputCapture* s_capture;
  friend void TIMER1_CAPT_vect(void);
};

#endif
#endif
