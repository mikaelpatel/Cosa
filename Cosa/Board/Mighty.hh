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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_MIGHTY_HH__
#define __COSA_BOARD_MIGHTY_HH__

/**
 * Cosa MIGHTY Board pin symbol definitions for ATmega1248 based
 * Arduino boards; Mighty. Cosa does not use pin numbers as
 * Arduino/Wiring. Instead strong data type is used (enum types) for
 * the specific pin classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 */
class Board {
  friend class Pin;
  friend class UART;
private:
  /**
   * Do not allow instances. This is a static singleton; name space.
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
    A7
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
    EXT2 = D2
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = A0,
    PCI1 = A1,
    PCI2 = A2,
    PCI3 = A3,
    PCI4 = A4,
    PCI5 = A5,
    PCI6 = A6,
    PCI7 = A7,
    PCI8 = D0,
    PCI9 = D1,
    PCI10 = D2,
    PCI11 = D3,
    PCI12 = D4,
    PCI13 = D5,
    PCI14 = D6,
    PCI15 = D7,
    PCI16 = D16,
    PCI17 = D17,
    PCI18 = D18,
    PCI19 = D19,
    PCI20 = D20,
    PCI21 = D21,
    PCI22 = D22,
    PCI23 = D23,
    PCI24 = D8,
    PCI25 = D9,
    PCI26 = D10,
    PCI27 = D11,
    PCI28 = D12,
    PCI29 = D13,
    PCI30 = D14,
    PCI31 = D15
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

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    EXT_MAX = 3,
    PCINT_MAX = 3,
    PIN_MAX = A7
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
 */
#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect 

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void INT1_vect(void) __attribute__ ((signal));
  void INT2_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void PCINT1_vect(void) __attribute__ ((signal));
  void PCINT2_vect(void) __attribute__ ((signal));
  void PCINT3_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TWI_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USART_UDRE_vect(void) __attribute__ ((signal));
  void USART_RX_vect(void) __attribute__ ((signal));
  void USART1_UDRE_vect(void) __attribute__ ((signal));
  void USART1_RX_vect(void) __attribute__ ((signal));
}
#endif

