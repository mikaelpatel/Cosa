/**
 * @file Cosa/Board/PJRC/Teensypp_2_0.hh
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

#ifndef COSA_BOARD_PJRC_TEENSYPP_2_0_HH
#define COSA_BOARD_PJRC_TEENSYPP_2_0_HH

/* This board is based on AT90USB1286 */
#define BOARD_AT90USB1286

/**
 * Compiler warning on unused varable.
 */
#if !defined(UNUSED)
#define UNUSED(x) (void) (x)
#endif

/**
 * Cosa pin symbol definitions for the PJRC Teensy++ 2.0, AT90USB1286
 * based board. Cosa does not use pin numbers as Arduino/Wiring,
 * instead strong data type is used (enum types) for the specific pin
 * classes; DigitalPin, AnalogPin, PWMPin, etc.
 *
 * The pin numbers for Teensy++ 2.0 are only symbolically mapped,
 * i.e. a pin number/digit will not work, symbols must be used,
 * e.g., Board::D12. Avoid iterations assuming that the symbols
 * are in order.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced.
 *
 * @section Board
 * @code
 *                  Teensy++ 2.0
 *                     -----
 *                +----| V |----+
 *            GND |o   |   |   o| VCC
 *            D27 |o   -----   o| D26
 *             D0 |o           o| D25
 *             D1 |o           o| D24
 *             D2 |o           o| D23
 *             D3 |o 36 o-o 37 o| D22
 *             D4 |o           o| D21
 *             D5 |o           o| D20
 *         LED/D6 |o           o| D19
 *             D7 |o           o| D18
 *             D8 |o           o| GND
 *             D9 |o           o| AREF
 *            D10 |o           o| D38/A0
 *            D11 |o 32 o-o 28 o| D39/A1
 *            D12 |o 33 o-o 29 o| D40/A2
 *            D13 |o 34 o-o 30 o| D41/A3
 *            D14 |o 35 o-o 31 o| D42/A4
 *            D15 |o           o| D43/A5
 *            D16 |o    ( )    o| D44/A6
 *            D17 |o   o o o   o| D45/A7
 *                +-------------+
 *                    /  |  \
 *                 RST  GND  VCC
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
    return (pin < 8  ? &PIND :
	    pin < 16 ? &PINC :
	    pin < 24 ? &PINB :
	    pin < 32 ? &PINA :
	    pin < 40 ? &PINE :
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
    D0 = 0,			// PD0
    D1 = 1,			// PD1
    D2 = 2,			// PD2
    D3 = 3,			// PD3
    D4 = 4,			// PD4
    D5 = 5,			// PD5
    D6 = 6,			// PD6
    D7 = 7,			// PD7
    D8 = 32,			// PE0
    D9 = 33,			// PE1
    D10 = 8,			// PC0
    D11 = 9,			// PC1
    D12 = 10,			// PC2
    D13 = 11,			// PC3
    D14 = 12,			// PC4
    D15 = 13,			// PC5
    D16 = 14,			// PC6
    D17 = 15,			// PC7
    D18 = 39,			// PE7
    D19 = 38,			// PE6
    D20 = 16,			// PB0
    D21 = 17,			// PB1
    D22 = 18,			// PB2
    D23 = 19,			// PB3
    D24 = 20,			// PB4
    D25 = 21,			// PB5
    D26 = 22,			// PB6
    D27 = 23,			// PB7
    D28 = 24,			// PA0
    D29 = 25,			// PA1
    D30 = 26,			// PA2
    D31 = 27,			// PA3
    D32 = 28,			// PA4
    D33 = 29,			// PA5
    D34 = 30,			// PA6
    D35 = 31,			// PA7
    D36 = 36,			// PE4
    D37 = 37,			// PE5
    D38 = 40,			// PF0
    D39 = 41,			// PF1
    D40 = 42,			// PF2
    D41 = 43,			// PF3
    D42 = 44,			// PF4
    D43 = 45,			// PF5
    D44 = 46,			// PF6
    D45 = 47,			// PF7
    LED = D6
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = 0,			// PF0
    A1 = 1,			// PF1
    A2 = 2,			// PF2
    A3 = 3,			// PF3
    A4 = 5,			// PF4
    A5 = 5,			// PF5
    A6 = 6,			// PF6
    A7 = 7			// PF7
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
    PWM0 = D27,			// PB7 => OCR0A
    PWM1 = D0,			// PD0 => OCR0B
    PWM2 = D25,			// PB5 => OCR1A
    PWM3 = D26,			// PB6 => OCR1B
    PWM4 = D16,			// PC6 => OCR3A
    PWM5 = D15,			// PC5 => OCR3B
    PWM6 = D14			// PD4 => OCR3C
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D0,			// PD0
    EXT1 = D1,			// PD1
    EXT2 = D2,			// PD2
    EXT3 = D3,			// PD3
    EXT4 = D36,			// PE4
    EXT5 = D37,			// PE5
    EXT6 = D18,			// PE6
    EXT7 = D19			// PE7
  } __attribute__((packed));

  /**
   * Pin change interrupt (PCI) pins. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = D20,			// PB0
    PCI1 = D21,			// PB1
    PCI2 = D22,			// PB2
    PCI3 = D23,			// PB3
    PCI4 = D24,			// PB4
    PCI5 = D25,			// PB5
    PCI6 = D26,			// PB6
    PCI7 = D27			// PB7
  } __attribute__((packed));

  /**
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 8,
    DIGITAL_PIN_MAX = 46,
    EXT_PIN_MAX = 8,
    PCI_PIN_MAX = 8,
    PWM_PIN_MAX = 7
  };

  /**
   * Pins used for TWI interface (in port D, bit 0-1, D0-D1)
   */
  enum TWIPin {
    SDA = 1,			// PD1/D1
    SCL = 0			// PD0/D0
  } __attribute__((packed));

  /**
   * Pins used for SPI interface (in port B, bit 0-3, D20-D23)
   */
  enum SPIPin {
    SS = 0,			// PB0/D20
    MOSI = 2,			// PB2/D22
    MISO = 3,			// PB3/D23
    SCK = 1			// PB1/D21
  } __attribute__((packed));

  /**
   * Auxiliary
   */
  enum {
    VBG = (_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)),
    UART_MAX = 2,
    EXT_MAX = 8,
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
#undef ID

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
  void SPI_STC_vect(void) __attribute__ ((signal));
  void TIMER0_COMPA_vect(void) __attribute__ ((signal));
  void TIMER0_COMPB_vect(void) __attribute__ ((signal));
  void TIMER0_OVF_vect(void) __attribute__ ((signal));
  void TIMER1_CAPT_vect(void)  __attribute__ ((signal));
  void TIMER1_COMPA_vect(void) __attribute__ ((signal));
  void TIMER1_COMPB_vect(void) __attribute__ ((signal));
  void TIMER1_COMPC_vect(void) __attribute__ ((signal));
  void TIMER1_OVF_vect(void) __attribute__ ((signal));
  void TIMER2_COMPA_vect(void) __attribute__ ((signal));
  void TIMER2_COMPB_vect(void) __attribute__ ((signal));
  void TIMER2_OVF_vect(void) __attribute__ ((signal));
  void TIMER3_CAPT_vect(void)  __attribute__ ((signal));
  void TIMER3_COMPA_vect(void) __attribute__ ((signal));
  void TIMER3_COMPB_vect(void) __attribute__ ((signal));
  void TIMER3_COMPC_vect(void) __attribute__ ((signal));
  void TIMER3_OVF_vect(void) __attribute__ ((signal));
  void TWI_vect(void) __attribute__ ((signal));
  void WDT_vect(void) __attribute__ ((signal));
  void USART1_RX_vect(void) __attribute__ ((signal));
  void USART1_TX_vect(void) __attribute__ ((signal));
  void USART1_UDRE_vect(void) __attribute__ ((signal));
  void USB_COM_vect(void) __attribute__ ((signal));
  void USB_GEN_vect(void) __attribute__ ((signal));
}
#endif
