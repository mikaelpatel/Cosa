/**
 * @file Servo.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_SERVO_HH
#define COSA_SERVO_HH

#include "Cosa/Types.h"
#include "Cosa/OutputPin.hh"

/**
 * Servo motor driver. Uses Timer#1 and the two compare output
 * registers.
 *
 * @section Limitations
 * Cannot be used together with other classes that use Timer#1.
 */
class Servo : private OutputPin {
public:
  /**
   * Construct a servo instance connected to the given pin.
   * Default angle is 90 degree.
   * @param[in] ix index of servo [0..1].
   * @param[in] pin digital pin to use as servo control output pin.
   */
  Servo(uint8_t ix, Board::DigitalPin pin) :
    OutputPin(pin),
    m_min(MIN_WIDTH),
    m_max(MAX_WIDTH)
  {
    angle(INIT_ANGLE);
    servo[ix != 0] = this;
  }

  /**
   * Start servo controller; enable interrupt handlers.
   */
  static bool begin();

  /**
   * Stop servo controller; disable interrupt handlers.
   */
  static bool end();

  /**
   * Set pulse limits; min and max number of micro seconds.
   * These will correspond to angle 0 and 180.
   * @param[in] min number of micro seconds.
   * @param[in] max number of micro seconds.
   */
  void pulse(uint16_t min, uint16_t max)
    __attribute__((always_inline))
  {
    m_min = min;
    m_max = max;
  }

  /**
   * Return current pulse width in micro seconds.
   * @return pulse width.
   */
  uint16_t width() const
  {
    return (m_width);
  }

  /**
   * Set servo to given angle degree.
   * @param[in] degree angle, 0..180.
   */
  void angle(uint8_t degree);

  /**
   * Return servo angle.
   * @return angle in degree, 0..180.
   */
  uint8_t angle() const
  {
    return (m_angle);
  }

private:
  /** Configuration. */
  static const uint16_t PERIOD = 20000;
  static const uint16_t MIN_WIDTH = 650;
  static const uint16_t MAX_WIDTH = 2300;
  static const uint8_t INIT_ANGLE = 90;

  /** Servo map. */
  static Servo* servo[2];

  /**
   * Servo state; Min/Max/Width of pulse, angle.
   */
  uint16_t m_min;
  uint16_t m_max;
  uint16_t m_width;
  uint8_t m_angle;

  /** Interrupt Service Routines. */
  friend void TIMER1_COMPA_vect(void);
  friend void TIMER1_COMPB_vect(void);
};

#endif
