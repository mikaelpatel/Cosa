/**
 * @file Cosa/Board/PJRC/Teensy_2_0.hh
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

#ifndef COSA_BOARD_PJRC_TEENSY_2_0_HH
#define COSA_BOARD_PJRC_TEENSY_2_0_HH

/* This board is based on ATmega32U4 */
#define BOARD_ATMEGA32U4

/**
 * Compiler warning on unused varable.
 */
#if !defined(UNUSED)
#define UNUSED(x) (void) (x)
#endif

/**
 * Cosa pin symbol definitions for the PJRC Teensy 2.0, ATmega32U4
 * based board. Cosa does not use pin numbers as Arduino/Wiring,
 * instead strong data type is used (enum types) for the specific pin
 * classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The pin numbers for Teensy 2.0 are only symbolically mapped,
 * i.e. a pin number/digit will not work, symbols must be used,
 * e.g., Board::D12. Avoid iterations assuming that the symbols
 * are in order.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced.
 *
 * @section Board
 * @code
 *                   Teensy 2.0
 *                     -----
 *                +----| V |----+
 *            GND |o   |   |   o| VCC
 *             D0 |o   -----   o| D21/A0
 *             D1 |o           o| D20/A1
 *             D2 |o           o| D19/A2
 *             D3 |o o       o o| D18/A3
 *        PWM0/D4 |o D24  AREF o| D17/A4
 *   EXT0/PWM1/D5 |o           o| D16/A5
 *        EXT1/D6 |o           o| D15/A6/PWM3
 *     RX/EXT2/D7 |o           o| D14/A7/PWM2
 *     TX/EXT3/D8 |o           o| D13/A8
 *        PWM4/D9 |o    ( )    o| D12/A9/PWM6
 *       PWM5/D10 |o o o o o o o| D11/A10/LED
 *                +-------------+
 *                 /  /  |  \  \
 *              D23 VCC GND RST D22/A11
 * @endcode
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
   * Return Special Function Register for given Teensy pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin)
    __attribute__((always_inline))
  {
    return (pin < 8  ? &PINB :
	    pin < 16 ? &PINC :
	    pin < 24 ? &PIND :
	    pin < 32 ? &PINE :
	               &PINF);
  }

  /**
   * Return bit position for given Teensy pin number in Special
   * Function Register.
   * @param[in] pin number.
   * @return pin bit position.
   */
  static uint8_t BIT(uint8_t pin)
    __attribute__((always_inline))
  {
    return (pin & 0x07);
  }

  /**
   * Return Pin Change Mask Register for given Teensy pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin)
    __attribute__((always_inline))
  {
    UNUSED(pin);
    return (&PCMSK0);
  }

  /**
   * Return UART Register for given Teensy serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port)
    __attribute__((always_inline))
  {
    UNUSED(port);
    return (&UCSR1A);
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
    D0 = 0,			// PB0
    D1 = 1,			// PB1
    D2 = 2,			// PB2
    D3 = 3,			// PB3
    D4 = 7,			// PB7
    D5 = 16,			// PD0
    D6 = 17,			// PD1
    D7 = 18,			// PD2
    D8 = 19,			// PD3
    D9 = 14,			// PC6
    D10 = 15,			// PC7
    D11 = 22,			// PD6
    D12 = 23,			// PD7
    D13 = 4,			// PB4
    D14 = 5,			// PB5
    D15 = 6,			// PB6
    D16 = 39,			// PF7
    D17 = 38,			// PF6
    D18 = 37,			// PF5
    D19 = 36,			// PF4
    D20 = 33,			// PF1
    D21 = 32,			// PF0
    D22 = 20,			// PD4
    D23 = 21,			// PD5
    D24 = 30,			// PE6
    LED = D11
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PF0/ADC0
    A1 = 1,			// PF1/ADC1
    A2 = 4,			// PF4/ADC4
    A3 = 5,			// PF5/ADC5
    A4 = 6,			// PF6/ADC6
    A5 = 7,			// PF7/ADC7
    A6 = 37,			// PB6/ADC13
    A7 = 36,			// PB5/ADC12
    A8 = 35,			// PB4/ADC11
    A9 = 34,			// PD7/ADC10
    A10 = 33,			// PD6/ADC9
    A11 = 32			// PD4/ADC8
  } __attribute__((packed));

  /**
   * Reference voltage; ARef pin, Vcc or internal 2V56
   */
  enum Reference {
    APIN_REFERENCE = 0,
    AVCC_REFERENCE = _BV(REFS0),
    A2V56_REFERENCE = (_BV(REFS1) | _BV(REFS0))
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile
   * time checking
   */
  enum PWMPin {
    PWM0 = D4,			// PB7 => OCR0A
    PWM1 = D5,			// PD0 => OCR0B
    PWM2 = D14,			// PB5 => OCR1A
    PWM3 = D15,			// PB6 => OCR1B
    PWM4 = D9,			// PC6 => OCR3A
    PWM5 = D10,			// PC7 => OCR4A
    PWM6 = D12			// PD7 => OCR4D
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D5,			// PD0
    EXT1 = D6,			// PD1
    EXT2 = D7,			// PD2
    EXT3 = D8			// PD3
  } __attribute__((packed));

  /**
   * Pin change interrupt (PCI) pins. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,			// PB0
    PCI1 = D1,			// PB1
    PCI2 = D2,			// PB2
    PCI3 = D3,			// PB3
    PCI4 = D13,			// PB4
    PCI5 = D14,			// PB5
    PCI6 = D15,			// PB6
    PCI7 = D4			// PB7
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 12,
    DIGITAL_PIN_MAX = 25,
    EXT_PIN_MAX = 4,
    PCI_PIN_MAX = 8,
    PWM_PIN_MAX = 7
  };

  /**
   * Pins used for TWI interface (port D, bit 0-1, D5-D6)
   */
  enum TWIPin {
    SDA = 1,			// PD1/D6
    SCL = 0			// PD0/D5
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (port B, bit 0-3, D0-D3)
   */
  enum SPIPin {
    SS = 0,			// PB0/D0
    SCK = 1,			// PB1/D1
    MOSI = 2,			// PB2/D2
    MISO = 3			// PB3/D3
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 2,
    EXT_MAX = 7,
    PCMSK_MAX = 1,
    PCINT_MAX = 8
  } __attribute__((packed));
};

/**
 * Redefined symbols to allow generic code.
 */
#define UCSZ00 UCSZ10
#define UCSZ01 UCSZ11
#define UCSZ02 UCSZ12
#define UPM00 UPM10
#define UPM01 UPM11
#define USBS0 USBS1
#define U2X0 U2X1
#define TXC0 TXC1
#define RXCIE0 RXCIE1
#define RXEN0 RXEN1
#define TXEN0 TXEN1
#define UDRE0 UDRE1
#define UDRIE0 UDRIE1
#define TXCIE0 TXCIE1
#if !defined(ADCW)
#define ADCW ADC
#endif

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
  void INT6_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void SPI_STC_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_CAPT_vect(void)  __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_COMPC_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void TIMER3_CAPT_vect(void)  __attribute__ ((signal));
  void TIMER3_COMPA_vect(void) __attribute__ ((signal));
  void TIMER3_COMPB_vect(void) __attribute__ ((signal));
  void TIMER3_COMPC_vect(void) __attribute__ ((signal));
  void TIMER3_OVF_vect(void) __attribute__ ((signal));
  void TIMER4_COMPA_vect(void) __attribute__ ((signal));
  void TIMER4_COMPB_vect(void) __attribute__ ((signal));
  void TIMER4_COMPD_vect(void) __attribute__ ((signal));
  void TIMER4_FPF_vect(void) __attribute__ ((signal));
  void TIMER4_OVF_vect(void) __attribute__ ((signal));
  void TWI_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USART1_RX_vect(void) __attribute__ ((signal));
  void USART1_TX_vect(void) __attribute__ ((signal));
  void USART1_UDRE_vect(void) __attribute__ ((signal));
  void USB_COM_vect(void) __attribute__ ((signal));
  void USB_GEN_vect(void) __attribute__ ((signal));
}
#endif
