/**
 * @file Cosa/Servo.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * @section Description
 * Servo motor driver. Uses Timer1 and the two compare output
 * registers.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_SERVO_HH__
#define __COSA_SERVO_HH__

#include "Cosa/Types.h"
#include "Cosa/Board.hh"
#include "Cosa/Pins.hh"

extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));
extern "C" void TIMER1_COMPB_vect(void) __attribute__ ((signal));

class Servo : private OutputPin {
private:
  /**
   * Interrupt handlers are friends.
   */
  friend void TIMER1_COMPA_vect(void);
  friend void TIMER1_COMPB_vect(void);

  static const uint16_t PERIOD = 20000;
  static const uint16_t MIN_WIDTH = 650;
  static const uint16_t MAX_WIDTH = 2300;
  static const uint8_t INIT_ANGLE = 90;
  static Servo* servo[2];

  /**
   * Servo state; Min/Max/Width of pulse, angle.
   */
  uint16_t m_min;
  uint16_t m_max;
  uint16_t m_width;
  uint8_t m_angle;

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
    set_angle(INIT_ANGLE);
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
  void set_pulse(uint16_t min, uint16_t max)
  {
    m_min = min;
    m_max = max;
  }

  /**
   * Return current pulse width in micro seconds.
   * @return pulse width.
   */
  uint16_t get_width()
  {
    return (m_width);
  }

  /**
   * Set servo to given angle degree.
   * @param[in] degree angle, 0..180.
   */
  void set_angle(uint8_t degree);

  /**
   * Return servo angle.
   * @return angle in degree, 0..180.
   */
  uint8_t get_angle() 
  {
    return (m_angle);
  }
};

#endif
