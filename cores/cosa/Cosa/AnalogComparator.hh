/**
 * @file Cosa/AnalogComparator.hh
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

#ifndef COSA_ANALOG_COMPARATOR_HH
#define COSA_ANALOG_COMPARATOR_HH

#include "Cosa/Event.hh"
#include "Cosa/Interrupt.hh"

/**
 * Analog Comparator; compare input values on the positive pin AIN0 (D6)
 * and negative pin AIN1 (D7), bandgap voltage or ADCn. Note: only one
 * instance can be active/enabled at a time.
 */
class AnalogComparator : public Interrupt::Handler, public Event::Handler {
public:
  enum Mode {
    ON_TOGGLE_MODE = 0,
    ON_FALLING_MODE = _BV(ACIS1),
    ON_RISING_MODE = (_BV(ACIS1) | _BV(ACIS0)),
  } __attribute__((packed));

  /**
   * Construct analog comparator handler. Compare AIN0 (D6) with AIN1
   * (D7) or bandgap voltage (1V1).
   * @param[in] mode comparator mode.
   */
  AnalogComparator(Mode mode = ON_TOGGLE_MODE, bool bandgap = false) :
    m_mode(mode),
    m_pin(bandgap ? VBG : AIN1)
  {}

  /**
   * Construct analog comparator handler. Compare AIN0 (D6) with given
   * analog pin (ADCn).
   * @param[in] pin analog pin to compare with.
   * @param[in] mode comparator mode.
   */
  AnalogComparator(Board::AnalogPin pin, Mode mode = ON_TOGGLE_MODE) :
    m_mode(mode),
    m_pin((uint8_t) (pin - Board::A0))
  {}

  /**
   * @override{Interrupt::Handler}
   * Enable analog comparator handler.
   * @note atomic
   */
  virtual void enable()
  {
    synchronized {
      s_comparator = this;
      ADCSRB = (m_pin >= AIN1) ? 0 : (_BV(ACME) | m_pin);
      if (m_pin == VBG)
	ACSR = _BV(ACBG) | _BV(ACIE) | m_mode;
      else
	ACSR = _BV(ACIE) | m_mode;
    }
  }

  /**
   * @override{Interrupt::Handler}
   * Disable analog comparator handler.
   * @note atomic
   */
  virtual void disable()
  {
    synchronized {
      ACSR = _BV(ACD);
      s_comparator = NULL;
    }
  }

  /**
   * @override{Interrupt::Handler}
   * Default interrupt service on comparator output rise, fall or toggle.
   * @param[in] arg argument from interrupt service routine.
   */
  virtual void on_interrupt(uint16_t arg = 0);

protected:
  static AnalogComparator* s_comparator; //!< Current comparator.
  static const uint8_t AIN1 = 254;	 //!< Default reference voltage.
  static const uint8_t VBG = 255;	 //!< Bandgap voltage reference.
  Mode m_mode;				 //!< Compare mode.
  uint8_t m_pin;			 //!< Analog channel.

  /** Interrupt Service Routine */
  friend void ANALOG_COMP_vect(void);
};

#endif
