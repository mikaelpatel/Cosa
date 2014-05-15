/**
 * @file Cosa/Board/Arduino/Mega2560.hh
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_BOARD_ARDUINO_MEGA2560_HH
#define COSA_BOARD_ARDUINO_MEGA2560_HH

/* This board is based on ATmega2560 */
#define BOARD_ATMEGA2560

/**
 * Cosa MEGA Board pin symbol definitions for the ATmega1280 and
 * ATmega2560 based Arduino boards; Mega 1280/2560. Cosa does not use
 * pin numbers as Arduino/Wiring, instead strong data type is used
 * (enum types) for the specific pin classes; DigitalPin, AnalogPin,
 * etc. 
 *
 * The pin numbers for ATmega1280 and ATmega2560 are only symbolically
 * mapped, i.e. a pin number/digit will not work, symbols must be
 * used, e.g., Board::D42. Avoid iterations assuming that the symbols
 * are in order. 
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
	    pin < 16 ? &PINH : 
	    pin < 24 ? &PINB : 
	    pin < 32 ? &PINA : 
	    pin < 40 ? &PINC : 
	    pin < 48 ? &PIND : 
	    pin < 56 ? &PINL : 
	    pin < 64 ? &PINF : 
	    pin < 72 ? &PINK : 
	    pin < 80 ? &PINJ : 
	               &PING);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register.
   * @param[in] pin number.
   * @return pin bit position.
   */
  static uint8_t BIT(uint8_t pin)
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
    return (pin < 24 ? &PCMSK0 : 
	               &PCMSK2);
  }

  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    return (port == 1 ? &UCSR1A :
	    port == 2 ? &UCSR2A :
	    port == 3 ? &UCSR3A :
	                &UCSR0A);
  }

public:
  /**
   * Digital pin symbols; mapping from name to port<5>:bit<3>.
   */
  enum DigitalPin {
    D0 = 0,
    D1 = 1,
    D2 = 4,
    D3 = 5,
    D4 = 85,
    D5 = 3,
    D6 = 11,
    D7 = 12,
    D8 = 13,
    D9 = 14,
    D10 = 20,
    D11 = 21,
    D12 = 22,
    D13 = 23,
    D14 = 73,
    D15 = 72,
    D16 = 9,
    D17 = 8,
    D18 = 43,
    D19 = 42,
    D20 = 41,
    D21 = 40,
    D22 = 24,
    D23 = 25,
    D24 = 26,
    D25 = 27,
    D26 = 28,
    D27 = 29,
    D28 = 30,
    D29 = 31,
    D30 = 39,
    D31 = 38,
    D32 = 37,
    D33 = 36,
    D34 = 35,
    D35 = 34,
    D36 = 33,
    D37 = 32,
    D38 = 47,
    D39 = 82,
    D40 = 81,
    D41 = 80,
    D42 = 55,
    D43 = 54,
    D44 = 53,
    D45 = 52,
    D46 = 51,
    D47 = 50,
    D48 = 49,
    D49 = 48,
    D50 = 19,
    D51 = 18,
    D52 = 17,
    D53 = 16,
    LED = D13
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,
    A1 = 1,
    A2 = 2,
    A3 = 3,
    A4 = 4,
    A5 = 5,
    A6 = 6,
    A7 = 7,
    A8 = 32,
    A9 = 33,
    A10 = 34,
    A11 = 35,
    A12 = 36,
    A13 = 37,
    A14 = 38,
    A15 = 39
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
    PWM4 = D6,
    PWM5 = D7,
    PWM6 = D8,
    PWM7 = D9,
    PWM8 = D10,
    PWM9 = D11,
    PWM10 = D12,
    PWM11 = D13
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D21,
    EXT1 = D20,
    EXT2 = D19,
    EXT3 = D18,
    EXT4 = D2,
    EXT5 = D3
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   * Arduino Mega does not allow access to all pins (PCI8..15).
   */
  enum InterruptPin {
    PCI0 = D53,
    PCI1 = D52,
    PCI2 = D51,
    PCI3 = D50,
    PCI4 = D10,
    PCI5 = D11,
    PCI6 = D12,
    PCI7 = D13,
    PCI16 = A8,
    PCI17 = A9,
    PCI18 = A10,
    PCI19 = A11,
    PCI20 = A12,
    PCI21 = A13,
    PCI22 = A14,
    PCI23 = A15
  } __attribute__((packed));

  /**
   * Pins used for TWI interface (in port D, digital pins 20-21).
   */
  enum TWIPin {
    SCL = 0,
    SDA = 1
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (in port B, digital pins 50-53).
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
    UART_MAX = 4,
    EXT_MAX = 8,
    PCMSK_MAX = 3,
    PCINT_MAX = 24
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
  void INT7_vect(void) __attribute__ ((signal));
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
  void USART2_UDRE_vect(void) __attribute__ ((signal));
  void USART2_RX_vect(void) __attribute__ ((signal));
  void USART2_TX_vect(void) __attribute__ ((signal));
  void USART3_UDRE_vect(void) __attribute__ ((signal));
  void USART3_RX_vect(void) __attribute__ ((signal));
  void USART3_TX_vect(void) __attribute__ ((signal));
}
#endif

