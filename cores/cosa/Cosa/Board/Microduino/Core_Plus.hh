/**
 * @file Cosa/Board/Microduino/Core_Plus.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_BOARD_MICRODUINO_CORE_PLUS_HH
#define COSA_BOARD_MICRODUINO_CORE_PLUS_HH

/* This board is based on ATmega1284P */
#define BOARD_ATMEGA1248P

/**
 * Cosa pin symbol and hardware definitions for the ATmega1284P based
 * board Microduino Core+. Cosa does not use pin numbers as Arduino/Wiring,
 * instead strong data type is used (enum types) for the specific pin
 * classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The pin numbers for ATmega1284P are mapped as in Arduino. The static
 * inline functions, SFR, BIT and UART, rely on compiler optimizations
 * to be reduced.
 *
 * @section Board
 * @code
 *                       Microduino Core+
 *                +--------------------------+
 *                |    D23 A5 D19 D17 D15    |
 *            GND |[]    [] [] [] [] []    []| 5V
 *          RESET |[]    [] [] [] [] []    []| 3V3
 *             D6 |[]  D22 A4 D18 D16 D14  []| D7
 *             D5 |[]                      []| D8
 *             D4 |[]                      []| D9
 *             D3 |[]                      []| D10/SS/
 *             D2 |[]                      []| D11/MOSI
 *          TX/D1 |[]                      []| D12/MISO
 *          RX/D0 |[]                      []| D13/SCK
 *                |[] [] [] [] [] [] [] [] []|
 *                +--------------------------+
 *                A7 A6 D21 D20 A3 A2 A1 A0 VREF
 *                      SCL SDA
 * @endcode
 *
 * @section References
 * 1. http://www.microduino.cc/wiki/images/7/71/Microduino-Core%2B_Pinout_3.jpg
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
    return (pin < 8  ? &PINB :
	    pin < 16 ? &PIND :
	    pin < 24 ? &PINC :
	               &PINA);
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
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin)
    __attribute__((always_inline))
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
    __attribute__((always_inline))
  {
    return (port == 1 ? &UCSR1A :
	                &UCSR0A);
  }

public:
  /**
   * Initiate board ports. Default void.
   */
  static void init() {}

  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 8,			// PD0
    D1 = 9,			// PD1
    D2 = 10,			// PD2
    D3 = 11,			// PD3
    D4 = 0,			// PB0
    D5 = 1,			// PB1
    D6 = 2,			// PB2
    D7 = 3,			// PB3
    D8 = 14,			// PD6
    D9 = 13,			// PD5
    D10 = 4,			// PB4
    D11 = 5,			// PB5
    D12 = 6,			// PB6
    D13 = 7,			// PB7
    D14 = 23,			// PC7
    D15 = 22,			// PC6
    D16 = 21,			// PC5
    D17 = 20,			// PC4
    D18 = 19,			// PC3
    D19 = 18,			// PC2
    D20 = 17,			// PC1
    D21 = 16,			// PC0
    D22 = 12,			// PD4
    D23 = 15,			// PD7
    D24 = 31,			// PA7
    D25 = 30,			// PA6
    D26 = 29,			// PA5
    D27 = 28,			// PA4
    D28 = 27,			// PA3
    D29 = 26,			// PA2
    D30 = 25,			// PA1
    D31 = 24,			// PA0
    LED = D13
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,
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
    A1V1_REFERENCE = _BV(REFS1),
    A2V56_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile
   * time checking
   */
  enum PWMPin {
    PWM0 = 3,			// PB3 => D7
    PWM1 = 4,			// PB4 => D10
#if defined(__AVR_ATmega1284P__)
    PWM2 = 6,			// PB6 => D12
    PWM3 = 7,			// PB7 => D13
#else
    PWM2 = 255,			// PB6 => NOP
    PWM3 = 255,			// PB7 => NOP
#endif
    PWM4 = 12,			// PD4 => D22
    PWM5 = 13,			// PD5 => D9
    PWM6 = 14,			// PD6 => D8
    PWM7 = 15			// PD7 => D23
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = 8,			// PD0 => D0
    EXT1 = 9,			// PD1 => D1
    EXT2 = 2			// PB2 => D6
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,			// PD0
    PCI1 = D1,			// PD1
    PCI2 = D2,			// PD2
    PCI3 = D3,			// PD3
    PCI4 = D4,			// PB0
    PCI5 = D5,			// PB1
    PCI6 = D6,			// PB2
    PCI7 = D7,			// PB3
    PCI8 = D8,			// PD6
    PCI9 = D9,			// PD5
    PCI10 = D10,		// PD4
    PCI11 = D11,		// PB5
    PCI12 = D12,		// PB6
    PCI13 = D13,		// PB7
    PCI14 = D14,		// PC7
    PCI15 = D15,		// PC6
    PCI16 = D16,		// PC5
    PCI17 = D17,		// PC4
    PCI18 = D18,		// PC3
    PCI19 = D19,		// PC2
    PCI20 = D20,		// PC1
    PCI21 = D21,		// PC0
    PCI22 = D22,		// PD4
    PCI23 = D23,		// PD7
    PCI24 = D24,		// PA7
    PCI25 = D25,		// PA6
    PCI26 = D26,		// PA5
    PCI27 = D27,		// PA4
    PCI28 = D28,		// PA3
    PCI29 = D29,		// PA2
    PCI30 = D30,		// PA1
    PCI31 = D31			// PA0
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 8,
    DIGITAL_PIN_MAX = 32,
    EXT_PIN_MAX = 3,
    PCI_PIN_MAX = 32,
    PWM_PIN_MAX = 8
  };

  /**
   * Pins used for TWI interface (port C, bit 0-1, D20-D21)
   */
  enum TWIPin {
    SCL = 0,			// PC0/D21
    SDA = 1			// PC1/D20
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (port B, bit 4-7, D4-D7).
   */
  enum SPIPin {
    SS = 4,			// PB4/D10
    MOSI = 5,			// PB5/D11
    MISO = 6,			// PB6/D12
    SCK = 7			// PB7/D13
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 2,
    EXT_MAX = 3,
    PCMSK_MAX = 4,
    PCINT_MAX = 32
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
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
  void PCINT0_vect(void) __attribute__ ((signal));
  void PCINT1_vect(void) __attribute__ ((signal));
  void PCINT2_vect(void) __attribute__ ((signal));
  void PCINT3_vect(void) __attribute__ ((signal));
  void SPI_STC_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_CAPT_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void TIMER2_COMPA_vect(void) __attribute__ ((signal));
  void TIMER2_COMPB_vect(void) __attribute__ ((signal));
  void TIMER2_OVF_vect(void) __attribute__ ((signal));
#if defined(__AVR_ATmega1284P__)
  void TIMER3_CAPT_vect(void) __attribute__ ((signal));
  void TIMER3_COMPA_vect(void) __attribute__ ((signal));
  void TIMER3_COMPB_vect(void) __attribute__ ((signal));
  void TIMER3_OVF_vect(void) __attribute__ ((signal));
#endif
  void TWI_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USART_RX_vect(void) __attribute__ ((signal));
  void USART_TX_vect(void) __attribute__ ((signal));
  void USART_UDRE_vect(void) __attribute__ ((signal));
  void USART1_RX_vect(void) __attribute__ ((signal));
  void USART1_UDRE_vect(void) __attribute__ ((signal));
  void USART1_TX_vect(void) __attribute__ ((signal));
}
#endif
