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
    return (pin < 8  ? &PINB : 
	    pin < 16 ? &PIND : 
	    pin < 24 ? &PINE : 
	    &PINF);
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
    return (pin < 8 ? &PCMSK0 : &PCMSK1);
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
   * Digital pin symbols; mapping from name to port<5>:bit<3> (BDEF0..7)
   */
  enum DigitalPin {
    D0 = 16,
    D1 = 17,
    D2 = 7,
    D3 = 19,
    D4 = 20,
    D5 = 21,
    D6 = 18,
    D7 = 22,
    D8 = 14,
    D9 = 0,
    D10 = 2,
    D11 = 3,
    D12 = 1,
    D13 = 11,
    D14 = 12,
    D15 = 9,
    D16 = 10,
    D17 = 13,
    D18 = 23,
    D19 = 14,
    D20 = 15,
    D21 = 4,
    D22 = 5,
    D23 = 6,
    D24 = 24,
    D25 = 25,
    D26 = 26,
    D27 = 27,
    D28 = 28,
    D29 = 29,
    D30 = 30,
    D31 = 31,
    LED = D23,
    LED_BLUE = D21,
    LED_RED = D22,
    LED_GREEN = D23
  } __attribute__((packed));

  /**
   * Analog pin symbols; mapping from name to port<5>:bit<3>.
   */
  enum AnalogPin {
    A0 = D24,
    A1 = D25,
    A2 = D26,
    A3 = D27,
    A4 = D28,
    A5 = D29,
    A6 = D30,
    A7 = D31
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
    EXT0 = D15,
    EXT1 = D16,
    EXT2 = D13,
    EXT3 = D14,
    EXT4 = D4,
    EXT5 = D5,
    EXT6 = D7,
    EXT7 = D18
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D9,
    PCI1 = D12,
    PCI2 = D10,
    PCI3 = D11,
    PCI4 = D21,
    PCI5 = D22,
    PCI6 = D23,
    PCI7 = D14
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
    EXT_MAX = 8,
    PCINT_MAX = 2,
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

