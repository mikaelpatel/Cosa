/**
 * @file Cosa/Board/Pinoccio/Scout.hh
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

#ifndef COSA_BOARD_PINOCCIO_SCOUT_HH
#define COSA_BOARD_PINOCCIO_SCOUT_HH

/* This board is based on ATmega256RFR */
#define BOARD_ATMEGA256RFR2

/**
 * Cosa Board pin symbol definitions for the ATmega256rfr based
 * Pinoccio board; Scout. Cosa does not use pin numbers as
 * Arduino/Wiring, instead strong data type is used (enum types) for
 * the specific pin classes; DigitalPin, AnalogPin, etc. 
 *
 * The pin numbers for Pinoccio Scout are only symbolically mapped,
 * i.e. a pin number/digit will not work, symbols must be used, e.g.,
 * Board::D42. Avoid iterations assuming that the symbols are in order. 
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 *
 * @section Board
 * @code
 *                    Pinoccio Scout
 *                       -----
 *                +------| V |------+
 *                |O--O  |   | OFF/\|
 *                ||  |  -----  ON\/|
 *                |----             |
 *                |                 |
 *           VUSB |o               o| VCC (3V)
 *           BKPK |o               o| GND
 *            RST |o               o| VBAT
 *        SCK/D13 |o               o| D22/RX1
 *       MISO/D12 |o               o| D23/TX1
 *       MOSI/D11 |o               o| D24/SCL
 *        SSN/D10 |o               o| D25/SDA
 *         RX0/D0 |o               o| AREF
 *         TX0/D1 |o               o| A0/D14
 *             D2 |o               o| A1/D15
 *             D3 |o               o| A2/D16
 *             D4 |o               o| A3/D17
 *             D5 |o               o| A4/D18
 *             D6 |o               o| A5/D19
 *             D7 |o               o| A6/D20
 *             D8 |o               o| A7/D21
 *                 \    [=====]    /
 *                  \-------------/

 * @endcode
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
   * Return Special Function Register for given Pinoccio Scount pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin)
    __attribute__((always_inline))
  { 
    return (pin < 8  ? &PINB : 
	    pin < 16 ? &PIND : 
	    pin < 24 ? &PINE : 
	               &PINF);
  }

  /**
   * Return bit position for given Pinoccio Scount pin number in Special
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
   * Return Pin Change Mask Register for given Pinoccio Scount pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin)
    __attribute__((always_inline))
  { 
    return (pin < 8 ? &PCMSK0 : 
	              &PCMSK1);
  }

  /**
   * Return UART Register for given Pinoccio Scount serial port.
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
   * Initiate board ports. Disable SPI chip select pins, i.e.
   * defined as output pins and set high.
   */
  static void init() {}

  /**
   * Digital pin symbols; mapping from name to port<5>:bit<3> (BDEF0..7)
   */
  enum DigitalPin {
    D0 = 16,			// PE0/RX0
    D1 = 17,			// PE1/TX0
    D2 = 7,			// PB7
    D3 = 19,			// PE3
    D4 = 20,			// PE4
    D5 = 21,			// PE5
    D6 = 18,			// PE2
    D7 = 22,			// PE6
    D8 = 13,			// PD5
    D9 = 14,			// PD6
    D10 = 0,			// PB0/SSN
    D11 = 2,			// PB2/MOSI
    D12 = 3,			// PB3/MISO
    D13 = 1,			// PB1/SCK
    D14 = 24,			// PF0
    D15 = 25,			// PF1
    D16 = 26,			// PF2
    D17 = 27,			// PF3
    D18 = 28,			// PF4
    D19 = 29,			// PF5
    D20 = 30,			// PF6
    D21 = 31,			// PF6
    D22 = 10,			// PD2/RX1
    D23 = 11,			// PD3/TX1
    D24 = 8,			// PD0/SCL
    D25 = 9,			// PD1/SDA
    LED_BLUE = 4,		// PB4
    LED_RED = 5,		// PB5
    LED_GREEN = 6,		// PB6
    LED = LED_GREEN
  } __attribute__((packed));

  /**
   * Analog pin symbols; analog channel numbers
   */
  enum AnalogPin {
    A0 = 0,			// PF0
    A1,				// PF1
    A2,				// PF2
    A3,				// PF3
    A4,				// PF4
    A5,				// PF5
    A6,				// PF6
    A7				// PF7
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
    PWM4 = LED_BLUE,
    PWM5 = LED_RED,
    PWM6 = LED_GREEN
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D24,			// PD0/D24/SCL
    EXT1 = D25,			// PD1/D25/SDA
    EXT2 = D22,			// PD2/D22/RX1
    EXT3 = D23,			// PD3/D23/TX1
    EXT4 = D4,			// PE4/D4
    EXT5 = D5,			// PE5/D5
    EXT6 = D7			// PE6/D6
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = 0,			// PB0/D10
    PCI1 = 1,			// PB1/D13
    PCI2 = 2,			// PB2/D11
    PCI3 = 3,			// PB3/D12
    PCI4 = 4,			// PB4/D2
    PCI5 = 5,			// PB5/Not used
    PCI6 = 6,			// PB6/Not used
    PCI7 = 7,			// PB7/D2
    PCI8 = 16			// PE0/D0
  } __attribute__((packed));

  /** 
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 8,
    DIGITAL_PIN_MAX = 26,
    EXT_PIN_MAX = 7,
    PCI_PIN_MAX = 9,
    PWM_PIN_MAX = 7
  };

  /**
   * Pins used for TWI interface (port D, D24-D25)
   */
  enum TWIPin {
    SCL = 0,			// PD0
    SDA = 1			// PD1
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (port B, D10-D13)
   */
  enum SPIPin {
    SS = 0,			// PB0/D10
    SCK = 1,			// PB1/D13
    MOSI = 2,			// PB2/D11
    MISO = 3			// PB3/D12
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 2,
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

#undef USART2_UDRE_vect
#undef USART3_UDRE_vect

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
}
#endif
