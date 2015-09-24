/**
 * @file Cosa/Board/Arduino/ATtinyX61.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_BOARD_ARDUINO_ATTINYX61_HH
#define COSA_BOARD_ARDUINO_ATTINYX61_HH

/* This board is based on ATtinyX61/ATtiny */
#define BOARD_ATTINYX61
#define BOARD_ATTINY

/**
 * Cosa ATTINYX61 Board pin symbol definitions for the ATtinyX61
 * processors. Cosa does not use pin numbers as Arduino/Wiring,
 * instead strong data type is used (enum types) for the specific pin
 * classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The pin numbers are only symbolically mapped, i.e. a pin
 * number/digit will not work, symbols must be used, Board::D2.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced.
 *
 * @section Circuit
 * @code
 *                    ATinyX61
 *                  +----U----+
 * (D8/SDA)-------1-|PB0   PA0|-20------(D0/A0/MISO)
 * (D9)-----------2-|PB1   PA1|-19------(D1/A1/MOSI)
 * (D10/SCL)------3-|PB2   PA2|-18--(D2/A2/EXT1/SCK)
 * (/RESET)-------4-|PB3   PA3|-17------(D3/AREF/SS)
 * (VCC)----------5-|VCC  AGND|-16------------(AGND)
 * (GND)----------6-|GND  AVCC|-15------------(AVCC)
 * (D12/A7)-------7-|PB4   PA4|-14-----------(D4/A3)
 * (D13/A8)-------8-|PB5   PA5|-13-------(D5/A4/LED)
 * (D14/A9/EXT0)--9-|PB6   PA6|-12-----------(D6/A5)
 * (/RESET)------10-|PB7   PA7|-11-----------(D7/A6)
 *                  +---------+
 * @endcode
 */
class Board {
  friend class Pin;
  friend class GPIO;
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
    return (pin < 8 ? &PINA : &PINB);
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
    return (pin < 8 ? pin : pin - 8);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin)
    __attribute__((always_inline))
  {
    return (pin < 8 ? &PCMSK0 : &PCMSK1);
  }

  /**
   * Return Universal Serial Interface SFR register.
   * @return special register pointer.
   */
  static volatile uint8_t* USI()
    __attribute__((always_inline))
  {
    return (&PINA);
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
    D0 = 0,			// PA0
    D1,				// PA1
    D2,				// PA2
    D3,				// PA3
    D4,				// PA4
    D5,				// PA5
    D6,				// PA6
    D7,				// PA7
    D8,				// PB0
    D9,				// PB1
    D10,			// PB2
    D11,			// PB3
    D12,			// PB4
    D13,			// PB5
    D14,			// PB6
    D15,			// PB7
    LED = D5
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PA0/D0
    A1,				// PA1/D1
    A2,				// PA2/D2
    A3,				// PA3/D3
    A4,				// PA4/D4
    A5, 			// PA5/D5
    A6,				// PA6/D6
    A7,				// PA7/D7
    A8,				// PB5/D13
    A9,				// PB6/D14
    A10				// PB7/D15
  } __attribute__((packed));

  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1 and 2V56.
   */
  enum Reference {
    AVCC_REFERENCE = 0,
    APIN_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = _BV(REFS1),
    A2V56_REFERENCE = (_BV(REFS2) | _BV(REFS1))
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile
   * time checking
   */
  enum PWMPin {
    PWM0 = D9,			// PB1 => OCR1A
    PWM1 = D11,			// PB3 => OCR1B
    PWM2 = D13			// PB5 => OCR1C
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D14,			// PB6
    EXT1 = D2			// PA2
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,			// PA0
    PCI1 = D1,			// PA1
    PCI2 = D2,			// PA2
    PCI3 = D3,			// PA3
    PCI4 = D4,			// PA4
    PCI5 = D5,			// PA5
    PCI6 = D6,			// PA6
    PCI7 = D7,			// PA7
    PCI8 = D8,			// PB0
    PCI9 = D9,			// PB1
    PCI10 = D10,		// PB2
    PCI11 = D11,		// PB3
    PCI12 = D12,		// PB4
    PCI13 = D13,		// PB5
    PCI14 = D14,		// PB6
    PCI15 = D15			// PB7
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 11,
    DIGITAL_PIN_MAX = 16,
    EXT_PIN_MAX = 2,
    PCI_PIN_MAX = 16,
    PWM_PIN_MAX = 3
  };

  /**
   * Pins used for TWI interface (Port B, bit 0/2, D8/D10).
   */
  enum TWIPin {
    SDA = 0,			// PB2/D8
    SCL = 2			// PB2/D10
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (Port A, bit 0-3, D0-D3)
   */
  enum SPIPin {
    MISO = 0,			// PA0/D0
    MOSI = 1,			// PA1/D1
    SCK = 2,			// PA2/D2
    SS = 3			// PA3/D3
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    EXT_MAX = 2,
    PCMSK_MAX = 2,
    PCINT_MAX = 16
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
 */
#define ANALOG_COMP_vect ANA_COMP_vect
#define PCINT0_vect PCINT_vect
#define ACSR ACSRB
#define WGM01 WGM00
#define TIMSK0 TIMSK
#define TIMSK1 TIMSK
#define TCNT0 TCNT0L
#define TIFR0 TIFR
#define WDTCSR WDTCR

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void INT1_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void TIMER0_CAPT_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USI_START_vect(void) __attribute__ ((signal));
  void USI_OVF_vect(void) __attribute__ ((signal));
}
#endif
