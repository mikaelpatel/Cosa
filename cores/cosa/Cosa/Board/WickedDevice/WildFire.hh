/**
 * @file Cosa/Board/WickedDevice/WildFire.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#ifndef COSA_BOARD_WICKEDDDEVICE_WILDFIRE_HH
#define COSA_BOARD_WICKEDDDEVICE_WILDFIRE_HH

/* This board is based on ATmega1248P */
#define BOARD_ATMEGA1248P

/**
 * Cosa pin symbol definitions for Wicked Device WildFire board. Cosa
 * does not use pin numbers as Arduino/Wiring. Instead strong data
 * type is used (enum types) for the specific pin classes; DigitalPin,
 * AnalogPin, PWMPin, etc.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced.
 * @section Board
 * @code
 *                    Wicked Device WildFire
 *                  -----
 *                +-|(o)|------------------------+
 *                | |   |                        |
 *                | -----                        |
 *                |o-o U8                        |
 *                |o-o                           |
 *                |o-o JTAG                    []| SCL
 *                |o-o                         []| SDA
 *                |o-o U9                      []| AREF
 *                |                            []| GND
 *             NC |[]                          []| D13/SCK/LED
 *          IOREF |[]                          []| D12/MISO
 *          RESET |[]                          []| D11/MOSI/PWM5
 *            3V3 |[]                          []| D10/SS/PWM4
 *             5V |[]                          []| D9/PWM3
 *            GND |[]                          []| D8
 *            GND |[]                            |
 *            Vin |[]                          []| D7
 *                |                            []| D6/PWM2
 *         A0/D14 |[]                          []| D5/PWM1
 *         A1/D15 |[]                          []| D4
 *         A2/D16 |[]  +--------+              []| D3/EXT1/PWM0
 *         A3/D17 |[]  |        |              []| D2/EXT0
 *     SDA/A4/D18 |[]  |   SD   |      +-----+ []| D1/TX
 *     SCL/A5/D19 |[]  |        |      | USB | []| D0/RX
 *         A6/D24 |[]  | || ||  |      +-----+---/
 *         A7/D25 |[]  +--------+     /
 *                 \------------------
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
    return (pin < 8  ? &PINA :
	    pin < 16 ? &PINB :
	    pin < 24 ? &PINC :
	               &PIND);
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
    return (pin < 8  ? &PCMSK0 :
	    pin < 16 ? &PCMSK1 :
	    pin < 24 ? &PCMSK2 :
	               &PCMSK3);
  }

  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port)
    __attribute__((always_inline))
  {
    return (port == 1 ? &UCSR1A : &UCSR0A);
  }

public:
  /**
   * Initiate board ports. Disable SPI chip select pins, i.e.
   * defined as output pins and set high.
   */
  static void init();

  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 24,			// PD0
    D1 = 25,			// PD1
    D2 = 26,			// PD2 => RFM69 IRQ
    D3 = 27,			// PD3
    D4 = 11,			// PB3
    D5 = 29,			// PD5
    D6 = 30,			// PD6 => LED
    D7 = 19,			// PC3 => RFM69 CS
    D8 = 28,			// PD4
    D9 = 12,			// PB4
    D10 = 31,			// PD7
    D11 = 13,			// PB5
    D12 = 14,			// PB6
    D13 = 15,			// PB7
    D14 = 20,			// PC4 => TinyWDT Pet
    D15 = 18,			// PC2 => Flash CS
    D16 = 21,			// PC5 => microSD CS
    D17 = 22,			// PC6 => RTC crystal
    D18 = 23,			// PC7 => RTC crystal
    D19 = 17,			// PC1 => TWI SDA
    D20 = 16,			// PC0 => TWI SCL
    D21 = 8,			// PB0 => CC3000 CS
    D22 = 10,			// PB2 => CC3000 IRQ
    D23 = 9,			// PB1 => CC3000 VBAT
    D24 = 0,			// PA0
    D25 = 1,			// PA1
    D26 = 2,			// PA2
    D27 = 3,			// PA3
    D28 = 4,			// PA4
    D29 = 5,			// PA5
    D30 = 6,			// PA6
    D31 = 7,			// PA7
    LED = D6
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PA0
    A1,				// PA1
    A2,				// PA2
    A3,				// PA3
    A4,				// PA4
    A5,				// PA5
    A6,				// PA6
    A7				// PA7
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
    PWM0 = D4,			// PB3 => OCR0A
    PWM1 = D9,			// PB4 => OCR0B
    PWM2 = D12,			// PB6 => OCR3A
    PWM3 = D13,			// PB7 => OCR3B
    PWM4 = D8,			// PD4 => OCR1B
    PWM5 = D5,			// PD5 => OCR1A
    PWM6 = D6,			// PD6 => OCR2B
    PWM7 = D10			// PD7 => OCR2A
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D2,			// PD2
    EXT1 = D3,			// PD3
    EXT2 = D22			// PB2
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,			// PD0
    PCI1 = D1,			// PD1
    PCI2 = D2,			// PD2
    PCI3 = D3,			// PD3
    PCI4 = D4,			// PB3
    PCI5 = D5,			// PD5
    PCI6 = D6,			// PD6
    PCI7 = D7,			// PC3
    PCI8 = D8,			// PD4
    PCI9 = D9,			// PB4
    PCI10 = D10,		// PD7
    PCI11 = D11,		// PB5
    PCI12 = D12,		// PB6
    PCI13 = D13,		// PB7
    PCI14 = D14,		// PC4
    PCI15 = D15,		// PC2
    PCI16 = D16,		// PC5
    PCI17 = D17,		// PC6
    PCI18 = D18,		// PC7
    PCI19 = D19,		// PC1
    PCI20 = D20,		// PC0
    PCI21 = D21,		// PB0
    PCI22 = D22,		// PB2
    PCI23 = D23,		// PB0
    PCI24 = D24,		// PA0
    PCI25 = D25,		// PA1
    PCI26 = D26,		// PA2
    PCI27 = D27,		// PA3
    PCI28 = D28,		// PA4
    PCI29 = D29,		// PA5
    PCI30 = D30,		// PA6
    PCI31 = D31			// PA7
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
   * Pins used for TWI interface (in port C, bit 0-1, D20-D19)
   */
  enum TWIPin {
    SDA = 1,			// PC1/D20
    SCL = 0			// PC0/D19
  } __attribute__((packed));

 /**
   * Pins used for SPI interface (in port B, bit 4-7, D9,D11-D13)
   */
  enum SPIPin {
    SS = 4,			// PB4/D9
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
  void TIMER3_CAPT_vect(void) __attribute__ ((signal));
  void TIMER3_COMPA_vect(void) __attribute__ ((signal));
  void TIMER3_COMPB_vect(void) __attribute__ ((signal));
  void TIMER3_OVF_vect(void) __attribute__ ((signal));
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
