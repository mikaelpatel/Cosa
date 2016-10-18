/**
 * @file Cosa/PWMPin.hh
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

#ifndef COSA_PWM_PIN_HH
#define COSA_PWM_PIN_HH

#include "Cosa/OutputPin.hh"

/**
 * Abstract pulse width modulation pin.
 */
class PWMPin : public OutputPin {
public:
  /**
   * Construct an abstract pwm output pin for given Arduino pin number.
   * @param[in] pin number.
   * @param[in] duty cycle (0..255)
   */
  PWMPin(Board::PWMPin pin, uint8_t duty = 0);

  /**
   * Start pwm pin. Power up timer.
   */
  void begin();

  /**
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   */
  void set(uint8_t duty);

  /**
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   */
  void write(uint8_t duty)
    __attribute__((always_inline))
  {
    set(duty);
  }

  /**
   * Set duty cycle for pwm output pin.
   * @param[in] duty cycle (0..255)
   * @return pwm pin.
   */
  PWMPin& operator=(uint8_t duty)
    __attribute__((always_inline))
  {
    set(duty);
    return (*this);
  }

  /**
   * Set duty cycle for pwm output pin with given value mapping.
   * The value is mapped from ]min..max[ to duty [0..255]. Value
   * below min is mapped to zero(0) and above max to 255.
   * @param[in] value to map.
   * @param[in] min value.
   * @param[in] max value.
   */
  void set(uint16_t value, uint16_t min, uint16_t max);

  /**
   * Set duty cycle for pwm output pin with given value mapping.
   * The value is mapped from ]min..max[ to duty [0..255]. Value
   * below min is mapped to zero(0) and above max to 255.
   * @param[in] value to map.
   * @param[in] min value.
   * @param[in] max value.
   */
  void write(uint16_t value, uint16_t min, uint16_t max)
    __attribute__((always_inline))
  {
    set(value, min, max);
  }

  /**
   * Return duty setting for pwm output pin.
   * @return duty
   */
  uint8_t duty();
};

#endif
