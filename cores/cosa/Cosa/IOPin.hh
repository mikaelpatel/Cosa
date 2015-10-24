/**
 * @file Cosa/IOPin.hh
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

#ifndef COSA_IO_PIN_HH
#define COSA_IO_PIN_HH

#include "Cosa/OutputPin.hh"

/**
 * Abstract IO-pin that may switch between input and output pin.
 */
class IOPin : public OutputPin {
public:
  enum Mode {
    OUTPUT_MODE = 0,
    INPUT_MODE = 1
  } __attribute__((packed));

  /**
   * Construct abstract in/output pin given Arduino pin number.
   * @param[in] pin number.
   * @param[in] mode pin mode (normal or pullup).
   */
  IOPin(Board::DigitalPin pin, Mode mode = INPUT_MODE, bool pullup = false) :
    OutputPin(pin)
  {
    synchronized {
      if (pullup)
	*PORT() |= m_mask;
    }
    this->mode(mode);
  }

  /**
   * Change IO-pin to given mode.
   * @param[in] mode new operation mode.
   * @note atomic
   */
  void mode(Mode mode)
    __attribute__((always_inline))
  {
    synchronized {
      if (mode == OUTPUT_MODE)
	*DDR() |= m_mask;
      else
	*DDR() &= ~m_mask;
    }
  }

  /**
   * Get current IO-pin mode.
   * @return mode.
   */
  Mode mode() const
    __attribute__((always_inline))
  {
    return ((*DDR() & m_mask) == 0 ? INPUT_MODE : OUTPUT_MODE);
  }

  /**
   * Change IO-pin to given mode.
   * @param[in] pin number.
   * @param[in] mode new operation mode.
   * @note atomic
   */
  static void mode(Board::DigitalPin pin, Mode mode)
    __attribute__((always_inline))
  {
    volatile uint8_t* ddr = DDR(pin);
    const uint8_t mask = MASK(pin);
    synchronized {
      if (mode == OUTPUT_MODE)
	*ddr |= mask;
      else
	*ddr &= ~mask;
    }
  }

  /**
   * Get IO-pin mode.
   * @param[in] pin number.
   * @return mode.
   */
  static Mode mode(Board::DigitalPin pin)
    __attribute__((always_inline))
  {
    return ((*DDR(pin) & MASK(pin)) == 0 ? INPUT_MODE : OUTPUT_MODE);
  }
};

#endif
