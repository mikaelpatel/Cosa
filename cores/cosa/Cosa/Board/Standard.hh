/**
 * @file Cosa/Board/Standard.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#ifndef __COSA_BOARD_STANDARD_HH__
#define __COSA_BOARD_STANDARD_HH__

/**
 * Cosa STANDARD Board pin symbol definitions for the ATmega328P based
 * boards such as Arduino Uno, Mini Pro, Nano, and LilyPad. Cosa does
 * not use pin numbers as Arduino/Wiring, instead strong data type is
 * used (enum types) for the specific pin classes; DigitalPin,
 * AnalogPin, PWMPin, etc. 
 *
 * The pin numbers for ATmega328P are mapped as in Arduino. The static
 * inline functions, SFR, BIT and UART, rely on compiler optimizations
 * to be reduced.  
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
  static uint8_t BIT(uint8_t pin)
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
    D14,
    D15,
    D16,
    D17,
    D18,
    D19,
    D20,
    D21,
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
    A7
  } __attribute__((packed));

  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1.
   */
  enum Reference {
    APIN_REFERENCE = 0,
    AVCC_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
    PWM0 = D3,
    PWM1 = D5,
    PWM2 = D6,
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
    EXT1 = D3
  } __attribute__((packed));

  /**
   * Pin change interrupt (PCI) pins. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,
    PCI1 = D1,
    PCI2 = D2,
    PCI3 = D3,
    PCI4 = D4,
    PCI5 = D5,
    PCI6 = D6,
    PCI7 = D7,
    PCI8 = D8,
    PCI9 = D9,
    PCI10 = D10,
    PCI11 = D11,
    PCI12 = D12,
    PCI13 = D13,
    PCI14 = A0,
    PCI15 = A1,
    PCI16 = A2,
    PCI17 = A3,
    PCI18 = A4,
    PCI19 = A5,
    PCI20 = A6,
    PCI21 = A7
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

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 1,
    EXT_MAX = 2,
    PCMSK_MAX = 3,
    PCINT_MAX = 24,
    PIN_MAX = A7
  } __attribute__((packed));
};

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void INT1_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void PCINT1_vect(void) __attribute__ ((signal));
  void PCINT2_vect(void) __attribute__ ((signal));
  void SPI_STC_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_CAPT_vect(void)  __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void TIMER2_COMPA_vect(void) __attribute__ ((signal));
  void TIMER2_COMPB_vect(void) __attribute__ ((signal));
  void TIMER2_OVF_vect(void) __attribute__ ((signal));
  void TWI_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USART_RX_vect(void) __attribute__ ((signal));
  void USART_TX_vect(void) __attribute__ ((signal));
  void USART_UDRE_vect(void) __attribute__ ((signal));
}
#endif

