/**
 * @file Cosa/Board/Arduino/Mega.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_BOARD_ARDUINO_MEGA_HH
#define COSA_BOARD_ARDUINO_MEGA_HH

/* This board is based on ATmega1280/2560 */
#if defined(ARDUINO_MEGA2560)
# define BOARD_ATMEGA2560
#else
# define BOARD_ATMEG1280
#endif

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
  friend class GPIO;
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
    __attribute__((always_inline))
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
    __attribute__((always_inline))
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
    __attribute__((always_inline))
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
    __attribute__((always_inline))
  {
    return (port == 1 ? &UCSR1A :
	    port == 2 ? &UCSR2A :
	    port == 3 ? &UCSR3A :
	                &UCSR0A);
  }

public:
  /**
   * Initiate board ports. Default void.
   */
  static void init() {}

  /**
   * Digital pin symbols; mapping from name to port<5>:bit<3>.
   */
  enum DigitalPin {
    D0 = 0,			// PE0
    D1 = 1,			// PE1
    D2 = 4,			// PE4
    D3 = 5,			// PE5
    D4 = 85,			// PG5
    D5 = 3,			// PE3
    D6 = 11,			// PH3
    D7 = 12,			// PH4
    D8 = 13,			// PH5
    D9 = 14,			// PH6
    D10 = 20,			// PB4
    D11 = 21,			// PB5
    D12 = 22,			// PB6
    D13 = 23,			// PB7
    D14 = 73,			// PJ1
    D15 = 72,			// PJ0
    D16 = 9,			// PH1
    D17 = 8,			// PH0
    D18 = 43,			// PD3
    D19 = 42,			// PD2
    D20 = 41,			// PD1
    D21 = 40,			// PD0
    D22 = 24,			// PA0
    D23 = 25,			// PA1
    D24 = 26,			// PA2
    D25 = 27,			// PA3
    D26 = 28,			// PA4
    D27 = 29,			// PA5
    D28 = 30,			// PA6
    D29 = 31,			// PA7
    D30 = 39,			// PC7
    D31 = 38,			// PC6
    D32 = 37,			// PC5
    D33 = 36,			// PC4
    D34 = 35,			// PC3
    D35 = 34,			// PC2
    D36 = 33,			// PC1
    D37 = 32,			// PC0
    D38 = 47,			// PD7
    D39 = 82,			// PG2
    D40 = 81,			// PG1
    D41 = 80,			// PG0
    D42 = 55,			// PL7
    D43 = 54,			// PL6
    D44 = 53,			// PL5
    D45 = 52,			// PL4
    D46 = 51,			// PL3
    D47 = 50,			// PL2
    D48 = 49,			// PL1
    D49 = 48,			// PL0
    D50 = 19,			// PB3/MISO
    D51 = 18,			// PB2/MOSI
    D52 = 17,			// PB1/SCK
    D53 = 16,			// PB0/SS
    D54 = 56,			// PF0/A0
    D55 = 57,			// PF1/A1
    D56 = 58,			// PF2/A2
    D57 = 59,			// PF3/A3
    D58 = 60,			// PF4/A4
    D59 = 61,			// PF5/A5
    D60 = 62,			// PF6/A6
    D61 = 63,			// PF7/A7
    D62 = 64,			// PK0/A8
    D63 = 65,			// PK1/A9
    D64 = 66,			// PK2/A10
    D65 = 67,			// PK3/A11
    D66 = 68,			// PK4/A12
    D67 = 69,			// PK5/A13
    D68 = 70,			// PK6/A14
    D69 = 71,			// PK7/A15
    LED = D13
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PF0
    A1 = 1,			// PF1
    A2 = 2,			// PF2
    A3 = 3,			// PF3
    A4 = 4,			// PF4
    A5 = 5,			// PF5
    A6 = 6,			// PF6
    A7 = 7,			// PF7
    A8 = 32,			// PK0
    A9 = 33,			// PK1
    A10 = 34,			// PK2
    A11 = 35,			// PK3
    A12 = 36,			// PK4
    A13 = 37,			// PK5
    A14 = 38,			// PK6
    A15 = 39			// PK7
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
    PWM0 = D2,			// PE4 => OCR3B
    PWM1 = D3,			// PE5 => 0CR3C
    PWM2 = D4,			// PG5 => OCR0B
    PWM3 = D5,			// PE3 => OCR3A
    PWM4 = D6,			// PH3 => OCR4A
    PWM5 = D7,			// PH4 => OCR4B
    PWM6 = D8,			// PH5 => OCR4C
    PWM7 = D9,			// PH6 => OCR2B
    PWM8 = D10,			// PB4 => OCR2A
    PWM9 = D11,			// PB5 => OCR1A
    PWM10 = D12,		// PB6 => OCR1B
    PWM11 = D13			// PB7 => OCR0A
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D21,			// PD0
    EXT1 = D20,			// PD1
    EXT2 = D19,			// PD2
    EXT3 = D18,			// PD3
    EXT4 = D2,			// PE4
    EXT5 = D3			// PE5
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   * Arduino Mega does not allow access to all PCI pins.
   */
  enum InterruptPin {
    PCI0 = D10,			// PB4
    PCI1 = D11,			// PB5
    PCI2 = D12,			// PB6
    PCI3 = D13,			// PB7
    PCI4 = D50,			// PB3
    PCI5 = D51,			// PB2
    PCI6 = D52,			// PB1
    PCI7 = D53,			// PB0
    PCI8 = D62,			// PK0/A8
    PCI9 = D63,			// PK1/A9
    PCI10 = D64,		// PK2/A10
    PCI11 = D65,		// PK3/A11
    PCI12 = D66,		// PK4/A12
    PCI13 = D67,		// PK5/A13
    PCI14 = D68,		// PK6/A14
    PCI15 = D69			// PK7/A15
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 16,
    DIGITAL_PIN_MAX = 70,
    EXT_PIN_MAX = 6,
    PCI_PIN_MAX = 24,
    PWM_PIN_MAX = 13
  };

  /**
   * Pins used for TWI interface (port D, D20-D21)
   */
  enum TWIPin {
    SCL = 0,			// PD0/D21
    SDA = 1			// PD1/D20
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (port B, D50-D53)
   */
  enum SPIPin {
    SS = 0,			// PB0/D53
    SCK = 1,			// PB1/D52
    MOSI = 2,			// PB2/D51
    MISO = 3			// PB3/D50
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
  void TIMER1_CAPT_vect(void)  __attribute__ ((signal));
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
