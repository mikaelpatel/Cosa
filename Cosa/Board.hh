/**
 * @file Cosa/Board.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Cosa Board pin symbol definitions for the standard Atmega328P 
 * based Arduino boards.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_HH__
#define __COSA_BOARD_HH__

class Board {
  friend class Pin;
private:
  /**
   * Do not allow instances. This is a static singleton.
   */
  Board() {}

  /**
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin) 
  { 
    return (pin < 8 ? &PIND : (pin < 14 ? &PINB : &PINC));
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register.
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin < 8 ? pin : (pin < 14 ? pin - 8 : (pin - 14)));
  }
  
public:
  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,
    D10,
    D11,
    D12,
    D13,
    LED = 13
  };

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 14,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8
  };

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile time checking
   */
  enum PWMPin {
    PWM0 = 3,
    PWM1 = 5,
    PWM2 = 6,
    PWM3 = 9,
    PWM4 = 10,
    PWM5 = 11
  };

  /**
   * External interrupt pin symbols; sub-set of digital pins to allow compile 
   * time checking.
   */
  enum InterruptPin {
    EXT0 = 2,
    EXT1 = 3
  };
};

#endif

