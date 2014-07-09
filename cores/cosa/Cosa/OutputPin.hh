/**
 * @file Cosa/OutputPin.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#ifndef COSA_OUTPUT_PIN_HH
#define COSA_OUTPUT_PIN_HH

#include "Cosa/Pin.hh"

/**
 * Abstract output pin. 
 */
class OutputPin : public Pin {
public:
  /**
   * Construct an abstract output pin for given Arduino pin number.
   * @param[in] pin number.
   * @param[in] initial value.
   */
  OutputPin(Board::DigitalPin pin, uint8_t initial = 0) :
    Pin((uint8_t) pin) 
  { 
    synchronized {
      *DDR() |= m_mask; 
      if (initial) {
	*PORT() |= m_mask; 
      }
      else {
	*PORT() &= ~m_mask; 
      }
    }
  }

  /**
   * Set output pin to mode.
   * @param[in] pin number.
   * @param[in] initial value.
   */
  static void set_mode(Board::DigitalPin pin, uint8_t initial = 0)
  {
    volatile uint8_t* port = PORT(pin);
    volatile uint8_t* ddr = DDR(pin);
    const uint8_t mask = MASK(pin);
    synchronized {
      *ddr |= mask;
      if (initial) {
	*port |= mask;
      }
      else {
	*port &= ~mask;
      }
    }
  }

  /**
   * Set the output pin. Unprotected version.
   */
  void _set() const __attribute__((always_inline)) 
  { 
    *PORT() |= m_mask; 
  }

  /**
   * Set the output pin.
   */
  void set() const __attribute__((always_inline)) 
  { 
    synchronized {
      *PORT() |= m_mask; 
    }
  }

  /**
   * Set the output pin.
   */
  void high() const __attribute__((always_inline)) 
  { 
    synchronized {
      *PORT() |= m_mask; 
    }
  }

  /**
   * Set the output pin.
   */
  void on() const __attribute__((always_inline))   
  { 
    synchronized {
      *PORT() |= m_mask; 
    }
  }

  /**
   * Clear the output pin. Unprotected version.
   */
  void _clear() const __attribute__((always_inline)) 
  { 
    *PORT() &= ~m_mask; 
  }

  /**
   * Clear the output pin.
   */
  void clear() const __attribute__((always_inline)) 
  { 
    synchronized {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Clear the output pin.
   */
  void low() const __attribute__((always_inline))
  { 
    synchronized {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Clear the output pin.
   */
  void off() const __attribute__((always_inline)) 
  { 
    synchronized {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Toggle the output pin. Actually atomic but defined for
   * consistent naming convension.
   */
  void _toggle() const __attribute__((always_inline)) 
  { 
    *PIN() = m_mask; 
  }

  /**
   * Toggle the output pin. Atomic per definition.
   */
  void toggle() const __attribute__((always_inline)) 
  { 
    *PIN() = m_mask; 
  }

  /**
   * Toggle the output pin. Atomic per definition
   * @param[in] pin number.
   */
  static void toggle(Board::DigitalPin pin) __attribute__((always_inline))
  { 
    *PIN(pin) = MASK(pin); 
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set. Unprotected version.
   * @param[in] value to set.
   */
  void _set(bool value) const __attribute__((always_inline)) 
  { 
    if (value) {
      *PORT() |= m_mask; 
    }
    else {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to set.
   */
  void set(bool value) const __attribute__((always_inline)) 
  { 
    synchronized {
      if (value) {
	*PORT() |= m_mask; 
      }
      else {
	*PORT() &= ~m_mask; 
      }
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set. Unprotected version.
   * @param[in] value to set.
   */
  void _write(bool value) const __attribute__((always_inline))
  { 
    if (value) {
      *PORT() |= m_mask; 
    }
    else {
      *PORT() &= ~m_mask; 
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set.
   * @param[in] value to write.
   */
  void write(uint8_t value) const __attribute__((always_inline))
  { 
    synchronized {
      if (value) {
	*PORT() |= m_mask; 
      }
      else {
	*PORT() &= ~m_mask; 
      }
    }
  }

  /**
   * Shift out given byte to the output pin using the given clock
   * output pin. Shift out according to given direction.
   * @param[in] value to write.
   * @param[in] clk output pin.
   * @param[in] order bit first.
   */
  void write(uint8_t value, OutputPin& clk, Direction order = MSB_FIRST) const;

  /**
   * Shift out given value and number of bits to the output pin using
   * the given pulse length in micro-seconds. Shift out from LSB(0) to
   * MSB(bits-1). This operation is synchronized and will turn off
   * interrupt handling during the transmission period.
   * @param[in] value to write.
   * @param[in] bits to write.
   * @param[in] us micro-second bit period.
   */
  void write(uint16_t value, uint8_t bits, uint16_t us) const;

  /**
   * Set the given output pin with the given value. Zero(0) to 
   * clear and non-zero to set.
   * @param[in] pin number.
   * @param[in] value to write.
   */
  static void write(Board::DigitalPin pin, uint8_t value) __attribute__((always_inline))
  { 
    volatile uint8_t* port = PORT(pin);
    const uint8_t mask = MASK(pin);
#if ARDUINO > 150
    if (__builtin_constant_p(pin) && __builtin_constant_p(value)) {
      if (value) {
	*port |= mask;
      }
      else {
	*port &= ~mask;
      }
    }
    else 
#endif
    synchronized {
      if (value) {
	*port |= mask;
      }
      else {
	*port &= ~mask;
      }
    }
  }

  /**
   * Set the given output pin with the given value. Zero(0) to 
   * clear and non-zero to set. Unprotected version when pin and value 
   * are constants.
   * @param[in] pin number (must be constant).
   * @param[in] value to write (must be constant).
   */
  static void _write(Board::DigitalPin pin, uint8_t value) __attribute__((always_inline))
  { 
    volatile uint8_t* port = PORT(pin);
    const uint8_t mask = MASK(pin);
    if (value) {
      *port |= mask;
    }
    else {
      *port &= ~mask;
    }
  }

  /**
   * Set the output pin with the given value. Zero(0) to clear
   * and non-zero to set. Output operator syntax.
   * @param[in] value to write.
   * @return output pin.
   */
  OutputPin& operator<<(uint8_t value) __attribute__((always_inline))
  {
    set(value);
    return (*this);
  }

  /**
   * Toggle the output pin to form a pulse with given width in
   * micro-seconds.
   * @param[in] us pulse width in micro seconds
   */
  void pulse(uint16_t us) const __attribute__((always_inline))
  {
    toggle();
    DELAY(us);
    toggle();
  }
};

#endif
