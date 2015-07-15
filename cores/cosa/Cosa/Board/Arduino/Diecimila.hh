/**
 * @file Cosa/Board/Arduino/Diecimila.hh
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

#ifndef COSA_BOARD_ARDUINO_DIECIMILA_HH
#define COSA_BOARD_ARDUINO_DIECIMILA_HH

/* This board is based on ATmega168 */
#define BOARD_ATMEGA168

/**
 * Compiler warning on unused varable.
 */
#if !defined(UNUSED)
#define UNUSED(x) (void) (x)
#endif

/**
 * Cosa pin symbol and hardware definitions for the ATmega168 based
 * board Arduino Diecimila. Cosa does not use pin numbers as
 * Arduino/Wiring, instead strong data type is used (enum types) for
 * the specific pin classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The pin numbers for ATmega168 are mapped as in Arduino. The static
 * inline functions, SFR, BIT and UART, rely on compiler optimizations
 * to be reduced.
 *
 * @section Board
 * @code
 *                      Arduino Diecimila
 *                  -----              -------
 *                +-|(o)|--------------| USB |---+
 *                | |   |              |     |   |
 *                | -----              |     |   |
 *                |                    -------   |
 *                |                              |
 *                |                              |
 *                |                              |
 *                |                            []| AREF
 *                |                            []| GND
 *                |                            []| D13/SCK/LED
 *                |                            []| D12/MISO
 *          RESET |[]                          []| D11/MOSI/PWM5
 *            3V3 |[]                          []| D10/SS/PWM4
 *             5V |[]                          []| D9/PWM3
 *            GND |[]                          []| D8
 *            GND |[]                            |
 *            Vin |[]                          []| D7
 *                |                            []| D6/PWM2
 *         A0/D14 |[]                          []| D5/PWM1
 *         A1/D15 |[]                          []| D4
 *         A2/D16 |[]                          []| D3/EXT1/PWM0
 *         A3/D17 |[]                          []| D2/EXT0
 *     SDA/A4/D18 |[]            ICSP          []| D1/TX
 *     SCL/A5/D19 |[]           o-o-o*         []| D0/RX
 *                 \            o-o-o           /
 *                  +--------------------------+
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
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin)
    __attribute__((always_inline))
  {
    return (pin < 8  ? &PIND :
	    pin < 14 ? &PINB :
	               &PINC);
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
    return (pin < 8  ? pin :
	    pin < 14 ? pin - 8 :
 	               pin - 14);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin)
    __attribute__((always_inline))
  {
    return (pin < 8  ? &PCMSK2 :
	    pin < 14 ? &PCMSK0 :
	               &PCMSK1);
  }

  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port)
    __attribute__((always_inline))
  {
    UNUSED(port);
    return (&UCSR0A);
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
    D0 = 0,			// PD0
    D1,				// PD1
    D2,				// PD2
    D3,				// PD3
    D4,				// PD4
    D5,				// PD5
    D6,				// PD6
    D7,				// PD7
    D8,				// PB0
    D9,				// PB1
    D10,			// PB2
    D11,			// PB3
    D12,			// PB4
    D13,			// PB5
    D14,			// PC0
    D15,			// PC1
    D16,			// PC2
    D17,			// PC3
    D18,			// PC4
    D19,			// PC5
    LED = D13
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PC0/D14
    A1,				// PC1/D15
    A2,				// PC2/D16
    A3,				// PC3/D17
    A4,				// PC4/D18
    A5				// PC5/D19
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
    PWM0 = D3,			// PD3 => OCR2B
    PWM1 = D5,			// PD5 => OCR0B
    PWM2 = D6,			// PD6 => OCR0A
    PWM3 = D9,			// PB1 => OCR1A
    PWM4 = D10,			// PB2 => OCR1B
    PWM5 = D11			// PB3 => OCR2A
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D2,			// PD2
    EXT1 = D3			// PD3
  } __attribute__((packed));

  /**
   * Pin change interrupt (PCI) pins (as digital pin number).
   */
  enum InterruptPin {
    PCI0 = D0,			// PD0
    PCI1 = D1,			// PD1
    PCI2 = D2,			// PD2
    PCI3 = D3,			// PD3
    PCI4 = D4,			// PD4
    PCI5 = D5,			// PD5
    PCI6 = D6,			// PD6
    PCI7 = D7,			// PD7
    PCI8 = D8,			// PB0
    PCI9 = D9,			// PB1
    PCI10 = D10,		// PB2
    PCI11 = D11,		// PB3
    PCI12 = D12,		// PB4
    PCI13 = D13,		// PB5
    PCI14 = D14,		// PC0
    PCI15 = D15,		// PC1
    PCI16 = D16,		// PC2
    PCI17 = D17,		// PC3
    PCI18 = D18,		// PC4
    PCI19 = D19			// PC5
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 6,
    DIGITAL_PIN_MAX = 20,
    EXT_PIN_MAX = 2,
    PCI_PIN_MAX = 20,
    PWM_PIN_MAX = 6
  };

  /**
   * Pins used for TWI interface (port C, analog pins A4-A5).
   */
  enum TWIPin {
    SDA = 4,			// PA4/A4
    SCL = 5			// PA4/A5
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (port B, digital pins D10-D13).
   */
  enum SPIPin {
    SS = 2,			// PB2/D10
    MOSI = 3,			// PB3/D11/ICSP
    MISO = 4,			// PB4/D12/ICSP
    SCK = 5			// PB5/D13/ICSP
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 1,
    EXT_MAX = 2,
    PCMSK_MAX = 3,
    PCINT_MAX = 24
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
