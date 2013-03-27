/**
 * @file Cosa/Board/TinyX5.hh
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
 * Cosa Board pin symbol definitions for the ATtinyX5 processors.
 *
 * Cosa does not use pin numbers as Arduino/Wiring, instead strong
 * data type is used (enum types) for the specific pin classes;
 * e.g. DigitalPin, AnalogPin, PWMPin.
 *
 * @section Limitations
 * The pin numbers are only symbolically mapped, i.e. a pin
 * number/digit will not work, symbols must be used, 
 * e.g., Board::D2.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_TINYX5_HH__
#define __COSA_BOARD_TINYX5_HH__

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
    return (&PINB);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin) 
  { 
    return (&PCMSK);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin);
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
    LED = D4
  } __attribute__((packed));

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 0,
    A1,
    A2,
    A3,
    A4,
    A5,
    PIN_MAX = A5
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
    PWM0 = D0,
    PWM1 = D1
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D2,
    EXT_MAX = 1
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,
    PCI1 = D1,
    PCI2 = D2,
    PCI3 = D3,
    PCI4 = D4,
    PCI5 = D5,
    PCINT_MAX = 1
  } __attribute__((packed));

  /**
   * Pins used for TWI interface.
   */
  enum TWIPin {
    SDA = 0,
    SCL = 2
  } __attribute__((packed));

 /**
   * Pins used for SPI interface.
   */
  enum SPIPin {
    SS = 4,
    MOSI = 0,
    MISO = 1,
    SCK = 2
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
 */
#define ANALOG_COMP_vect ANA_COMP_vect
#define PCMSK0 PCMSK
#define TIMSK0 TIMSK
#define TIMSK1 TIMSK
#define TIFR0 TIFR
#define WDTCSR WDTCR

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void INT0_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
}
#endif

