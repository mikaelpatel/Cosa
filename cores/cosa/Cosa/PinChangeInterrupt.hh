/**
 * @file Cosa/PinChangeInterrupt.hh
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

#ifndef __COSA_PIN_CHANGE_INTERRUPT_HH__
#define __COSA_PIN_CHANGE_INTERRUPT_HH__

#include "Cosa/Types.h"
#include "Cosa/Event.hh"
#include "Cosa/IOPin.hh"
#include "Cosa/Interrupt.hh"

/**
 * Abstract interrupt pin. Allows interrupt handling on 
 * the pin value changes. 
 */
class PinChangeInterrupt : public IOPin, public Interrupt::Handler {
public:
  /**
   * Start handling of pin change interrupt handling.
   */
  static void begin();

  /**
   * End handling of pin change interrupt handling.
   */
  static void end();

  /**
   * Construct interrupt pin with given pin number.
   * @param[in] pin.
   * @param[in] mode.
   */
  PinChangeInterrupt(Board::InterruptPin pin, bool pullup = false) :
    IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup)
  {
  }

  /**
   * @override Interrupt::Handler
   * Enable interrupt pin change detection and interrupt handler.
   */
  virtual void enable();

  /**
   * @override Interrupt::Handler
   * Disable interrupt pin change detection.
   */
  virtual void disable();

  /**
   * @override Interrupt::Handler
   * Default interrupt service on pin change interrupt.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0) = 0;

private:
  static const uint8_t INSTANCE_MAX = Board::PCINT_MAX * CHARBITS;
  static PinChangeInterrupt* instance[INSTANCE_MAX];
  static uint8_t state[Board::PCINT_MAX];

  /**
   * Map interrupt source: Check which pin(s) are the source of the
   * pin change interrupt and call the corresponding interrupt handler
   * per pin.
   * @param[in] ix port index.
   * @param[in] mask pin mask.
   * @param[in] base pin number from IDE.
   */
  static void on_interrupt(uint8_t ix, uint8_t mask, uint8_t base);
  
  friend void PCINT0_vect(void);
#if defined(PCINT1_vect)
  friend void PCINT1_vect(void);
#if defined(PCINT2_vect)
  friend void PCINT2_vect(void);
#if defined(PCINT3_vect)
  friend void PCINT3_vect(void);
#endif
#endif
#endif
};
#endif
