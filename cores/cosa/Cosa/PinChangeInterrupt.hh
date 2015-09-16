/**
 * @file Cosa/PinChangeInterrupt.hh
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

#ifndef COSA_PIN_CHANGE_INTERRUPT_HH
#define COSA_PIN_CHANGE_INTERRUPT_HH

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

  /** Interrupt modes. */
  enum InterruptMode {
    ON_FALLING_MODE = 0,
    ON_RISING_MODE = 1,
    ON_CHANGE_MODE = 2
  } __attribute__((packed));

  /**
   * Construct interrupt pin with given pin identity, mode and pullup
   * resistor flag.
   * @param[in] pin identity.
   * @param[in] mode of operation (default ON_CHANGE_MODE).
   * @param[in] pullup flag (default false).
   */
  PinChangeInterrupt(Board::InterruptPin pin,
		     InterruptMode mode = ON_CHANGE_MODE,
		     bool pullup = false) :
    IOPin((Board::DigitalPin) pin, INPUT_MODE, pullup),
    m_mode(mode),
    m_next(NULL)
  {}

  /**
   * @override{Interrupt::Handler}
   * Enable interrupt pin change detection and interrupt handler.
   * @note atomic
   */
  virtual void enable();

  /**
   * @override{Interrupt::Handler}
   * Disable interrupt pin change detection.
   * @note atomic
   */
  virtual void disable();

  /**
   * @override{Interrupt::Handler}
   * Default interrupt service on pin change interrupt.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0) = 0;

private:
  static PinChangeInterrupt* s_pin[Board::PCMSK_MAX];
  static uint8_t s_state[Board::PCMSK_MAX];

  /** Interrupt Mode. */
  InterruptMode m_mode;

  /** Linked list of pin change interrupt handlers. */
  PinChangeInterrupt* m_next;

  /**
   * Map interrupt source: Check which pin(s) are the source of the
   * pin change interrupt and call the corresponding interrupt handler
   * per pin.
   * @param[in] ix port index.
   * @param[in] mask pin mask.
   * @param[in] base pin number.
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
