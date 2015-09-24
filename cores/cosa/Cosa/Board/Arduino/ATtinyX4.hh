/**
 * @file Cosa/Board/Arduino/ATtinyX4.hh
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

#ifndef COSA_BOARD_ARDUINO_ATTINYX4_HH
#define COSA_BOARD_ARDUINO_ATTINYX4_HH

/* This board is based on ATtinyX4/ATtiny */
#define BOARD_ATTINYX4
#define BOARD_ATTINY

/**
 * Cosa ATTINYX4 Board pin symbol definitions for the ATtinyX4
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
 *                       ATinyX4
 *                     +----U----+
 * (VCC)-------------1-|VCC   GND|-14------------(GND)
 * (D8)--------------2-|PB0   PA0|-13----------(D0/A0)
 * (D9)--------------3-|PB1   PA1|-12----------(D1/A1)
 * (/RESET)----------4-|PB3   PA2|-11----------(D2/A2)
 * (EXT0/D10)--------5-|PB2   PA3|-10-------(D3/A3/SS)
 * (LED/D7/A7)-------6-|PA7   PA4|-9---(D4/A4/SCL/SCK)
 * (MISO/SDA/D6/A6)--7-|PA6   PA5|-8------(D5/A5/MOSI)
 *                     +---------+
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
    LED = D7
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel number)
   */
  enum AnalogPin {
    A0 = 0,			// PA0/D0
    A1,				// PA1/D1
    A2,				// PA2/D2
    A3,				// PA3/D3
    A4,				// PA4/D4
    A5, 			// PA5/D5
    A6,				// PA6/D6
    A7				// PA7/D7
  } __attribute__((packed));

  /**
   * Reference voltage; ARef pin, Vcc or internal 1V1.
   */
  enum Reference {
    AVCC_REFERENCE = 0,
    APIN_REFERENCE = _BV(REFS0),
    A1V1_REFERENCE = _BV(REFS1)
  } __attribute__((packed));

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile
   * time checking
   */
  enum PWMPin {
    PWM0 = D10,			// PB2 => OCR0A
    PWM1 = D7,			// PA7 => OCR0B
    PWM2 = D6,			// PA6 => OCR1A
    PWM3 = D5			// PA5 => OCR1B
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D10			// PB2
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
    PCI10 = D10			// PB2
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 8,
    DIGITAL_PIN_MAX = 11,
    EXT_PIN_MAX = 1,
    PCI_PIN_MAX = 11,
    PWM_PIN_MAX = 4
  };

  /**
   * Pins used for TWI interface (in Port A, bit 4/6, D4/D6)
   */
  enum TWIPin {
    SDA = 6,			// PA6/D6
    SCL = 4			// PA4/D4
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (in Port A, bit 3-6, D3-D5)
   */
  enum SPIPin {
    SS = 3,			// PA3/D3
    SCK = 4,			// PA4/D4
    MOSI = 5,			// PA5/D5
    MISO = 6			// PA6/D6
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX5) | _BV(MUX0)),
    EXT_MAX = 1,
    PCMSK_MAX = 2,
    PCINT_MAX = 11
  } __attribute__((packed));
};

/**
 * Redefinition of symbols to allow generic code.
 */
#define ANALOG_COMP_vect ANA_COMP_vect
#define TIMER0_OVF_vect TIM0_OVF_vect
#define TIMER0_COMPA_vect TIM0_COMPA_vect
#define TIMER0_COMPB_vect TIM0_COMPB_vect
#define TIMER1_OVF_vect TIM1_OVF_vect
#define TIMER1_COMPA_vect TIM1_COMPA_vect
#define TIMER1_COMPB_vect TIM1_COMPB_vect

/**
 * Forward declare interrupt service routines to allow them as friends.
 */
extern "C" {
  void ADC_vect(void) __attribute__ ((signal));
  void ANALOG_COMP_vect(void) __attribute__ ((signal));
  void INT0_vect(void) __attribute__ ((signal));
  void PCINT0_vect(void) __attribute__ ((signal));
  void PCINT1_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_CAPT_vect(void)  __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USI_START_vect(void) __attribute__ ((signal));
  void USI_OVF_vect(void) __attribute__ ((signal));
}
#endif
