/**
 * @file Cosa/Board/Pinoccio.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef __COSA_BOARD_PINOCCIO_HH__
#define __COSA_BOARD_PINOCCIO_HH__

/**
 * Cosa Pinoccio Board pin symbol definitions for the ATmega256rfr
 * based Pinoccio board; Scout. Cosa does not use pin numbers as
 * Arduino/Wiring, instead strong data type is used (enum types) for
 * the specific pin classes; DigitalPin, AnalogPin, etc. 
 *
 * The pin numbers for ATmega256rfr are only symbolically mapped,
 * i.e. a pin number/digit will not work, symbols must be used, e.g.,
 * Board::D42. Avoid iterations assuming that the symbols are in order. 
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
    return (pin < 8  ? &PINE : 
	    pin < 16 ? &PINB : 
	    pin < 24 ? &PIND : 
	    pin < 32 ? &PINE : &PINF);
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
   * Arduino Mega does not allow access to all pins.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin) 
  { 
    return (pin < 24 ? &PCMSK0 : &PCMSK2);
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
   * Digital pin symbols; mapping from name to port<5>:bit<3>.
   */
  enum DigitalPin {
    D0 = 0,
    D1 = 1,
    D2 = 15,
    D3 = 3,
    D4 = 4,
    D5 = 5,
    D6 = 2,
    D7 = 6,
    D8 = 21,
    D9 = 8,
    D10 = 10,
    D11 = 11,
    D12 = 9,
    D13 = 18,
    D14 = 19,
    D15 = 16,
    D16 = 17,
    D17 = 20,
    D18 = 31,
    D19 = 22,
    D20 = 23,
    D21 = 12,
    D22 = 13,
    D23 = 14,
    LED = D23,
    LED_BLUE = D21,
    LED_RED = D22,
    LED_GREEN = D23
  } __attribute__((packed));

  /**
   * Analog pin symbols; mapping from name to port<5>:bit<3>.
   */
  enum AnalogPin {
    A0 = 32,
    A1 = 33,
    A2 = 34,
    A3 = 35,
    A4 = 36,
    A5 = 37,
    A6 = 38,
    A7 = 39
  } __attribute__((packed));

  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1.
   */
  enum Reference {
    APIN_REFERENCE = 0,
    AVCC_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = _BV(REFS1),
    A2V56_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking.
   */
  enum PWMPin {
    PWM0 = D2,
    PWM1 = D3,
    PWM2 = D4,
    PWM3 = D5,
    PWM4 = D21,
    PWM5 = D22,
    PWM6 = D23
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D4,
    EXT1 = D5,
    EXT2 = D7,
    EXT3 = D13,
    EXT4 = D14,
    EXT5 = D15,
    EXT6 = D16,
    EXT7 = D18
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   * Arduino Mega does not allow access to all pins (PCI8..15).
   */
  enum InterruptPin {
    PCI0 = D20,
    PCI1 = D21,
    PCI2 = D22,
    PCI3 = D23,
    PCI4 = D9,
    PCI5 = D10,
    PCI6 = D11,
    PCI7 = D12
  } __attribute__((packed));

  /**
   * Pins used for TWI interface (in port D, digital pins 15-16).
   */
  enum TWIPin {
    SCL = 0,
    SDA = 1
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (in port B, digital pins 9-12).
   */
  enum SPIPin {
    SS = 0,
    SCK = 1,
    MOSI = 2,
    MISO = 3
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 2,
    EXT_MAX = 6,
    PCINT_MAX = 3,
    PIN_MAX = A7
  } __attribute__((packed));
};

/**
 * Redefined symbols to allow generic code.
 */
#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect 
#define USART_TX_vect USART0_TX_vect

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void INT1_vect(void) __attribute__ ((signal));
  void INT2_vect(void) __attribute__ ((signal));
  void INT3_vect(void) __attribute__ ((signal));
  void INT4_vect(void) __attribute__ ((signal));
  void INT5_vect(void) __attribute__ ((signal));
  void INT6_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void PCINT1_vect(void) __attribute__ ((signal));
  void PCINT2_vect(void) __attribute__ ((signal));
  void SPI_STC_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_COMPC_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void TIMER2_COMPA_vect(void) __attribute__ ((signal));
  void TIMER2_COMPB_vect(void) __attribute__ ((signal));
  void TIMER2_OVF_vect(void) __attribute__ ((signal));
  void TWI_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USART_UDRE_vect(void) __attribute__ ((signal));
  void USART_RX_vect(void) __attribute__ ((signal));
  void USART_TX_vect(void) __attribute__ ((signal));
  void USART1_UDRE_vect(void) __attribute__ ((signal));
  void USART1_RX_vect(void) __attribute__ ((signal));
  void USART1_TX_vect(void) __attribute__ ((signal));
}
#endif

