/**
 * @file Cosa/Board/Standard.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * Cosa Board pin symbol definitions for the ATmega8, ATmega168
 * ATmega328P based boards such as Arduino Uno, Mini Pro, Nano, and
 * LilyPad. Cosa does not use pin numbers are Arduino. Instead strong
 * data type is used (enum types) for the specific pin classes;
 * e.g. InterruptPin, AnalogPin, PWMPin.
 *
 * @section Limitations
 * The pin numbers for ATmega8, ATmega168 and ATmega328P are mapped
 * as in Arduino.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_STANDARD_HH__
#define __COSA_BOARD_STANDARD_HH__

class Board {
  friend class Pin;
  friend class UART;
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
    return (pin < 8  ? &PIND : 
	    pin < 14 ? &PINB : 
	    &PINC);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin) 
  { 
    return (pin < 8  ? &PCMSK2 : 
	    pin < 14 ? &PCMSK0 : 
	    &PCMSK1);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin < 8  ? pin : 
	    pin < 14 ? pin - 8 : 
	    pin - 14);
  }
  
  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    return (&UCSR0A);
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
    LED = D13
  } __attribute__((packed));

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
    A8,
    PIN_MAX = A8
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
#if !defined(__AVR_ATmega8__)
    PWM0 = D3,
    PWM1 = D5,
    PWM2 = D6,
#endif
    PWM3 = D9,
    PWM4 = D10,
    PWM5 = D11
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D2,
    EXT1 = D3,
    EXT_MAX = 2
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum {
    PCINT_MAX = 3
  } __attribute__((packed));

  /**
   * Pins used for TWI interface (in port C, analog pins 18-19).
   */
  enum TWIPin {
    SDA = 4,
    SCL = 5
  } __attribute__((packed));

 /**
   * Pins used for SPI interface (in port B, digital pins 10-13).
   */
  enum SPIPin {
    SS = 2,
    MOSI = 3,
    MISO = 4,
    SCK = 5
  } __attribute__((packed));
};
#endif

