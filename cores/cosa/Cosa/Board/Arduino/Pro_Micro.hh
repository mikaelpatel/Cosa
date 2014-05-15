/**
 * @file Cosa/Board/Arduino/Pro_Micro.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef COSA_BOARD_ARDUINO_PRO_MICRO_HH
#define COSA_BOARD_ARDUINO_PRO_MICRO_HH

/* This board is based on ATmega32U4 */
#define BOARD_ATMEGA32U4

/**
 * Compiler warning on unused varable.
 */
#if !defined(UNUSED)
#define UNUSED(x) (void) (x)
#endif

/**
 * Cosa pin symbol and hardware definitions for the ATmega32U4 based
 * SparkFun Pro Micro board. Cosa does not use pin numbers as
 * Arduino/Wiring, instead strong data type is used (enum types) for
 * the specific pin classes; DigitalPin, AnalogPin, PWMPin, etc. 
 *
 * The pin numbers for ATmega32u4 are mapped as in Arduino. The static
 * inline functions, SFR, BIT and UART, rely on compiler optimizations
 * to be reduced.  
 *
 * @section Board
 * @code
 *               Arduino Pro Micro
 *                     -----
 *                +----| V |----+
 *     EXT3/TX/D1 |o<  |   |   o| RAW
 *     EXT2/RX/D0 |o>  -----   o| GND
 *            GND |o           o| RESET
 *            GND |o           o| VCC
 *        EXT1/D2 |o           o| D17/A3
 *   PWM1/EXT0/D3 |o           o| D16/A2
 *             D4 |o           o| D15/A1
 *        PWM4/D5 |o           o| D14/A0
 *        PWM6/D6 |o           o| D21/SCK
 *             D7 |o           o| D23/MISO
 *             D8 |o           o| D22/MOSI
 *        PWM2/D9 |o           o| D10/PWM3
 *                +-------------+
 * @endcode
 *
 * Note: SPI pins are not numbered as on board as the numbers are 
 * reserved in Cosa for analog pins (which also may act as digital 
 * pins). 
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
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin) 
  { 
    return (pin < 8  ? &PINB : 
	    pin < 16 ? &PINC : 
	    pin < 24 ? &PIND : 
	    pin < 32 ? &PINE : 
	               &PINF);
  }

  /**
   * Return Pin Change Mask Register for given Arduino pin number.
   * @param[in] pin number.
   * @return pin change mask register pointer.
   */
  static volatile uint8_t* PCIMR(uint8_t pin) 
  { 
    UNUSED(pin);
    return (&PCMSK0);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static uint8_t BIT(uint8_t pin)
  {
    return (pin & 0x07);
  }
  
  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    UNUSED(port);
    return (&UCSR1A);
  }

public:
  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 18,
    D1 = 19,
    D2 = 17,
    D3 = 16,
    D4 = 20,
    D5 = 14,
    D6 = 23,
    D7 = 30,
    D8 = 4,
    D9 = 5,
    D10 = 6,
    D14 = 39,
    D15 = 38,
    D16 = 37,
    D17 = 36,
    D20 = 0,			// SS
    D21 = 1,			// SCK
    D22 = 2,			// MOSI
    D23 = 3,			// MISO
    LED = 15,			// Not used
    RXLED = 0,			// Green
    TXLED = 21			// Yellow
  } __attribute__((packed));

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 7,
    A1 = 6,
    A2 = 5,
    A3 = 4,
    A4 = 1,
    A5 = 0,			// Extended Analog Pins
    A6 = 32,			// D4
    A7 = 33,			// D12
    A8 = 34,			// D6
    A9 = 35,			// D8
    A10 = 36,			// D9
    A11 = 37			// D10
    
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
    PWM0 = D11,
    PWM1 = D3,
    PWM2 = D9,
    PWM3 = D10,
    PWM4 = D5,
    PWM5 = D13,
    PWM6 = D6
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = D3,
    EXT1 = D2,
    EXT2 = D0,
    EXT3 = D1
  } __attribute__((packed));

  /**
   * Pin change interrupt (PCI) pins. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = 0,			// RXLED
    PCI1 = 1,			// SCK
    PCI2 = 2,			// MOSI
    PCI3 = 3,			// MISO
    PCI4 = D8,
    PCI5 = D9,
    PCI6 = D10,
    PCI7 = D11
  } __attribute__((packed));

  /**
   * Pins used for TWI interface (in port D, digital pin D0-D1, TWI pins)
   */
  enum TWIPin {
    SDA = 1,
    SCL = 0
  } __attribute__((packed));
  
  /**
   * Pins used for SPI interface (in port B, bit 0-3, SPI pins)
   */
  enum SPIPin {
    SS = 0,
    MOSI = 2,
    MISO = 3,
    SCK = 1
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
#define RXCIE0 RXCIE1
#define RXEN0 RXEN1
#define TXEN0 TXEN1
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




#endif

