/**
 * @file Cosa/Board/Mighty.hh
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
 * Cosa Board pin symbol definitions for ATmega1248 based Arduino
 * boards; Mighty.
 *
 * Cosa does not use pin numbers are Arduino. Instead strong
 * data type is used (enum types) for the specific pin classes;
 * e.g. InterruptPin, AnalogPin, PWMPin.
 *
 * @section Limitations
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_MIGHTY_HH__
#define __COSA_BOARD_MIGHTY_HH__

#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect 

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
    return (pin < 8  ? &PINB : 
	    pin < 14 ? &PIND : 
	    pin < 24 ? &PINC :
	    &PINA);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin & 0x7);
  }
  
  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin) 
  { 
    return (pin < 8  ? &PCMSK1 : 
	    pin < 14 ? &PCMSK3 : 
	    pin < 24 ? &PCMSK2 :
	    &PCMSK0);
  }

  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    return (port == 1 ? &UCSR1A : &UCSR0A);
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
    D14,
    D15,
    D16,
    D17,
    D18,
    D19,
    D20,
    D21,
    D22,
    D23,
    LED = D13
  } __attribute__((packed));

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 24,
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
    PWM0 = D3,
    PWM1 = D4,
    PWM2 = D6,
    PWM3 = D7,
    PWM4 = D12,
    PWM5 = D13,
    PWM6 = D14,
    PWM7 = D15
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D10,
    EXT1 = D11,
    EXT2 = D2,
    EXT_MAX = 3
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum {
    PCINT_MAX = 3
  } __attribute__((packed));

  /**
   * Pins used for TWI interface (in port C, digital pin 16, 17).
   */
  enum TWIPin {
    SDA = 1,
    SCL = 0
  } __attribute__((packed));

 /**
   * Pins used for SPI interface (in port B, digital pins 4-7).
   */
  enum SPIPin {
    SS = 4,
    MOSI = 5,
    MISO = 6,
    SCK = 7
  } __attribute__((packed));
};

#endif

