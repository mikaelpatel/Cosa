/**
 * @file Cosa/Pin.hh
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

#ifndef COSA_PIN_HH
#define COSA_PIN_HH

#include "Cosa/Types.h"
#include "Cosa/Bits.h"
#include "Cosa/IOStream.hh"

class OutputPin;

/**
 * Arduino pins abstractions; abstract, input, output, interrupt and
 * analog pin. Captures the mapping from Arduino to processor pins.
 * Forces declarative programming of pins in sketches.
 */
class Pin {
public:
  /**
   * Return bit mask for given Arduino pin number.
   * @param[in] pin number.
   * @return pin bit mask.
   */
  static uint8_t MASK(uint8_t pin)
    __attribute__((always_inline))
  {
    return (_BV(Board::BIT(pin)));
  }

  /**
   * Return pointer to PIN register.
   * @param[in] pin number.
   * @return PIN register pointer.
   */
  static volatile uint8_t* PIN(uint8_t pin)
    __attribute__((always_inline))
  {
    return (Board::SFR(pin));
  }

  /**
   * Return pointer to Data Direction Register.
   * @param[in] pin number.
   * @return DDR register pointer.
   */
  static volatile uint8_t* DDR(uint8_t pin)
    __attribute__((always_inline))
  {
    return (Board::SFR(pin) + 1);
  }

  /**
   * Return pointer to data PORT register.
   * @param[in] pin number.
   * @return PORT register pointer.
   */
  static volatile uint8_t* PORT(uint8_t pin)
    __attribute__((always_inline))
  {
    return (Board::SFR(pin) + 2);
  }

  /**
   * Serialization directions; most or least significant bit first.
   */
  enum Direction {
    MSB_FIRST = 0,
    LSB_FIRST = 1
  } __attribute__((packed));

  /**
   * Construct abstract pin given Arduino pin number.
   * @param[in] pin number.
   */
  Pin(uint8_t pin) :
    m_sfr(Board::SFR(pin)),
    m_mask(MASK(pin)),
    m_pin(pin)
  {}

  /**
   * Return Arduino pin number of abstract pin.
   * @return pin number.
   */
  uint8_t pin() const
  {
    return (m_pin);
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_set() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) != 0);
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_high() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) != 0);
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool is_on() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) != 0);
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_clear() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) == 0);
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_low() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) == 0);
  }

  /**
   * Return true(1) if the pin is clear otherwise false(0).
   * @return boolean.
   */
  bool is_off() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) == 0);
  }

  /**
   * Return true(1) if the pin is set otherwise false(0).
   * @return boolean.
   */
  bool read() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) != 0);
  }

  /**
   * Shift in a byte from the input pin using the given clock
   * output pin. Shift in according to given direction.
   * @param[in] clk output pin.
   * @param[in] order bit first.
   * @return value.
   */
  uint8_t read(OutputPin& clk, Direction order = MSB_FIRST) const;

  /**
   * Use pin number directly to read value. Does not require an instance.
   * Return true(1) if the pin is set otherwise false(0).
   * @param[in] pin number.
   * @return boolean.
   */
  static bool read(Board::DigitalPin pin)
    __attribute__((always_inline))
  {
    return ((*Board::SFR(pin) & MASK(pin)) != 0);
  }

  /**
   * Read input pin and assign variable.
   * @param[out] var to assign.
   * @return pin.
   */
  operator bool() const
    __attribute__((always_inline))
  {
    return ((*PIN() & m_mask) != 0);
  }

  /**
   * Read input pin and assign variable.
   * @param[out] var to assign.
   * @return pin.
   */
  Pin& operator>>(uint8_t& var)
    __attribute__((always_inline))
  {
    var = ((*PIN() & m_mask) != 0);
    return (*this);
  }

protected:
  /** Special function register pointer. */
  volatile uint8_t* const m_sfr;

  /** Pin mask in port. */
  const uint8_t m_mask;

  /** Pin identity. */
  const uint8_t m_pin;

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
   * Return pin change interrupt mask register.
   * @return pin change mask register pointer.
   */
  volatile uint8_t* PCIMR() const
    __attribute__((always_inline))
  {
    return (Board::PCIMR(m_pin));
  }
};

/**
 * Syntactic sugar for an asserted block. The given pin will be toggled.
 * Initiating the pin to zero(0) will give active high logic.
 * @param[in] pin to assert.
 */
#define asserted(pin)							\
  for (uint8_t __i = (pin.toggle(), 1); __i != 0; __i--, pin.toggle())

#endif
