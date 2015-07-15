/**
 * @file Cosa/Board/Arduino/ATtinyX5.hh
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

#ifndef COSA_BOARD_ARDUINO_ATTINYX5_HH
#define COSA_BOARD_ARDUINO_ATTINYX5_HH

/* This board is based on ATtinyX5/ATtiny */
#define BOARD_ATTINYX5
#define BOARD_ATTINY

/**
 * Compiler warning on unused varable.
 */
#if !defined(UNUSED)
#define UNUSED(x) (void) (x)
#endif

/**
 * Cosa ATTINYX5 Board pin symbol definitions for the ATtinyX5
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
 *                  ATinyX5
 *                +----U----+
 * (/RESET)-----1-|PB5   VCC|-8-----------------(VCC)
 * (D3/A3)------2-|PB3   PB2|-7--(D2/A1/EXT0/SCL/SCK)
 * (LED/D4/A2)--3-|PB4   PB1|-6-------------(D1/MOSI)
 * (GND)--------4-|GND   PB0|-5---------(D0/SDA/MISO)
 *                +---------+
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
    UNUSED(pin);
    return (&PINB);
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
    UNUSED(pin);
    return (pin);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin)
    __attribute__((always_inline))
  {
    UNUSED(pin);
    return (&PCMSK);
  }

  /**
   * Return Universal Serial Interface SFR register.
   * @return special register pointer.
   */
  static volatile uint8_t* USI()
    __attribute__((always_inline))
  {
    return (&PINB);
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
    D1,				// PB1
    D2,				// PB2
    D3,				// PB3
    D4,				// PB4
    D5,				// PB5
    LED = D4
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PB5/D5
    A1,				// PB2/D2
    A2,				// PB4/D4
    A3				// PB3/D3
  } __attribute__((packed));

  /**
   * Reference voltage; ARef pin, Vcc or internal 2V56.
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
    PWM0 = D0,			// PB0 => OCR0A
    PWM1 = D1			// PB1 => OCR0B
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D2			// PB2
  } __attribute__((packed));

  /**
   * Pin change interrupt. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D0,			// PB0
    PCI1 = D1,			// PB1
    PCI2 = D2,			// PB2
    PCI3 = D3,			// PB3
    PCI4 = D4,			// PB4
    PCI5 = D5			// PB5
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 4,
    DIGITAL_PIN_MAX = 6,
    EXT_PIN_MAX = 1,
    PCI_PIN_MAX = 6,
    PWM_PIN_MAX = 2
  };

  /**
   * Pins used for TWI interface (in Port B, bit 0/2, D0/D2)
   */
  enum TWIPin {
    SDA = D0,			// PB0
    SCL = D2			// PB2
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (in Port B, bit 0-3, D0-D3)
   */
  enum SPIPin {
    MISO = 0,			// PB0/D0
    MOSI = 1,			// PB1/D1
    SCK = 2,			// PB2/D2
    SS = 3			// PB3/D3
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX3) | _BV(MUX2)),
    EXT_MAX = 1,
    PCMSK_MAX = 1,
    PCINT_MAX = 6
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
 */
#define ANALOG_COMP_vect ANA_COMP_vect
#define PCMSK0 PCMSK
#define TIMSK0 TIMSK
#define TIMSK1 TIMSK
#define TIFR0 TIFR
#define WDTCSR WDTCR

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
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
