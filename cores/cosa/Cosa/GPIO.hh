/**
 * @file Cosa/GPIO.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015-2016, Mikael Patel
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

#ifndef COSA_GPIO_HH
#define COSA_GPIO_HH

#include "Cosa/Types.h"

/**
 * High Performance General Purpose Input Output (GPIO) Pin
 * abstraction.
 */
class GPIO {
public:
  /**
   * Pin modes.
   */
  enum Mode {
    OUTPUT_MODE,		// Output pin mode.
    INPUT_MODE,			// Input pin mode.
    PULLUP_INPUT_MODE		// Input pin with pullup mode.
  };

  /**
   * Construct general purpose pin object for given digital pin
   * with given input/output mode and initial value.
   * @param[in] pin identity.
   * @param[in] mode input/output mode.
   * @param[in] value initial value of port.
   * @note atomic
   */
  GPIO(Board::DigitalPin pin, Mode mode = INPUT_MODE, bool value = 0) :
    m_sfr(Board::SFR(pin)),
    m_mask(MASK(pin))
  {
    synchronized {
      if (mode == OUTPUT_MODE) {
	*DDR() |= m_mask;
      }
      else {
	*DDR() &= ~m_mask;
      }
      if (value || (mode == PULLUP_INPUT_MODE)) {
	*PORT() |= m_mask;
      }
      else {
	*PORT() &= ~m_mask;
      }
    }
  }

  /**
   * Set pin input/output mode.
   * @param mode input or output mode.
   * @note atomic
   */
  void mode(Mode mode) const
    __attribute__((always_inline))
  {
    synchronized {
      if (mode == OUTPUT_MODE)
	*DDR() |= m_mask;
      else {
	*DDR() &= ~m_mask;
	if (mode == PULLUP_INPUT_MODE)
	  *PORT() |= m_mask;
      }
    }
  }

  /**
   * Get pin input/output mode.
   * @return mode.
   */
  Mode mode() const
    __attribute__((always_inline))
  {
    if ((*DDR() & m_mask) != 0)
      return (OUTPUT_MODE);
    if ((*PORT() & m_mask) != 0)
      return (PULLUP_INPUT_MODE);
    return (INPUT_MODE);
  }

  /**
   * Read pin state and return true(1) if set else false(0).
   * @return bool.
   */
  operator bool() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) != 0);
  }

  /**
   * Set the pin to the given value.
   * @param[in] value to set.
   * @note atomic
   */
  GPIO& operator=(bool value)
    __attribute__((always_inline))
  {
    synchronized {
      if (value) {
	*PORT() |= m_mask;
      }
      else {
	*PORT() &= ~m_mask;
      }
    }
    return (*this);
  }

  /**
   * Set the pin state with value of given pin.
   * @param[in] rhs pin value to set.
   * @note atomic
   */
  GPIO& operator=(const GPIO& rhs)
    __attribute__((always_inline))
  {
    bool value = rhs;
    synchronized {
      if (value) {
	*PORT() |= m_mask;
      }
      else {
	*PORT() &= ~m_mask;
      }
    }
    return (*this);
  }

  /**
   * Toggle the pin state.
   * @note atomic
   */
  void operator~() const
    __attribute__((always_inline))
  {
    *PIN() = m_mask;
  }

  /**
   * Set pin input/output mode. Does not require an instance.
   * @param mode input or output mode.
   * @note atomic
   */
  static void mode(Board::DigitalPin pin, Mode mode)
    __attribute__((always_inline))
  {
    const uint8_t mask = MASK(pin);
    synchronized {
      if (mode == OUTPUT_MODE)
	*DDR(pin) |= mask;
      else {
	*DDR(pin) &= ~mask;
	if (mode == PULLUP_INPUT_MODE)
	  *PORT(pin) |= mask;
      }
    }
  }

  /**
   * Get pin input/output mode. Does not require an instance.
   * @return mode.
   */
  static Mode mode(Board::DigitalPin pin)
    __attribute__((always_inline))
  {
    const uint8_t mask = MASK(pin);
    if ((*DDR(pin) & mask) != 0)
      return (OUTPUT_MODE);
    if ((*PORT(pin) & mask) != 0)
      return (PULLUP_INPUT_MODE);
    return (INPUT_MODE);
  }

  /**
   * Use pin identity directly to read pin state. Does not require an
   * instance. Return true(1) if the pin is set otherwise false(0).
   * @param[in] pin identity.
   * @return bool.
   */
  static bool read(Board::DigitalPin pin)
    __attribute__((always_inline))
  {
    return ((*PIN(pin) & MASK(pin)) != 0);
  }

  /**
   * Use pin identity directly to write pin state. Does not require an
   * instance.
   * @param[in] pin identity..
   * @param[in] value to write.
   * @note atomic
   */
  static void write(Board::DigitalPin pin, bool value)
    __attribute__((always_inline))
  {
    volatile uint8_t* port = PORT(pin);
    const uint8_t mask = MASK(pin);
#if (ARDUINO > 150)
    // Synchronized not needed when constant values (and lower ports)
#if defined(PORTH)
    if (((int) port < PORTH)
	&& __builtin_constant_p(pin)
	&& __builtin_constant_p(value)) {
#else
    if (__builtin_constant_p(pin) && __builtin_constant_p(value)) {
#endif
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
   * Use pin identity directly to toggle pin state. Does not require an
   * instance.
   * @param[in] pin identity.
   * @note atomic
   */
  static void toggle(Board::DigitalPin pin)
    __attribute__((always_inline))
  {
    volatile uint8_t* port = PIN(pin);
    const uint8_t mask = MASK(pin);
#if (ARDUINO > 150)
    // Synchronized not needed when constant values (and lower ports)
#if defined(PORTH)
    if (((int) port < PORTH) && __builtin_constant_p(pin))
#else
    if (__builtin_constant_p(pin))
#endif
      *port = mask;
    else
#endif
      synchronized {
	*port = mask;
      }
    }

protected:
  /** Special function register pointer. */
  volatile uint8_t* const m_sfr;

  /** Pin mask in port. */
  const uint8_t m_mask;

  /**
   * Return pointer to PIN register.
   * @return PIN register pointer.
   */
  volatile uint8_t* PIN() const
  {
    return (m_sfr);
  }

  /**
   * Return pointer to Data Direction Register.
   * @return DDR register pointer.
   */
  volatile uint8_t* DDR() const
  {
    return (m_sfr + 1);
  }

  /**
   * Return pointer to data PORT register.
   * @return PORT register pointer.
   */
  volatile uint8_t* PORT() const
  {
    return (m_sfr + 2);
  }

  /**
   * Return bit mask for given pin identity.
   * @param[in] pin identity.
   * @return pin bit mask.
   */
  static uint8_t MASK(uint8_t pin)
    __attribute__((always_inline))
  {
    return (_BV(Board::BIT(pin)));
  }
  /**
   * Return pointer to PIN register for given pin identity.
   * @param[in] pin identity.
   * @return PIN register pointer.
   */
  static volatile uint8_t* PIN(uint8_t pin)
    __attribute__((always_inline))
  {
    return (Board::SFR(pin));
  }

  /**
   * Return pointer to Data Direction Register for given pin
   * identity.
   * @param[in] pin identity.
   * @return DDR register pointer.
   */
  static volatile uint8_t* DDR(uint8_t pin)
    __attribute__((always_inline))
  {
    return (Board::SFR(pin) + 1);
  }

  /**
   * Return pointer to data PORT register for given pin identity.
   * @param[in] pin identity.
   * @return PORT register pointer.
   */
  static volatile uint8_t* PORT(uint8_t pin)
    __attribute__((always_inline))
  {
    return (Board::SFR(pin) + 2);
  }
};

#endif
