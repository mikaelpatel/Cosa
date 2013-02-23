/**
 * @file Cosa/Board.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * @section Description
 * Cosa Board pin symbol definitions for the ATmega8, ATmega168
 * ATmega328P, ATmega1280 and ATmega2560 based Arduino boards;
 * Arduino Uno, Mini Pro, Nano, LilyPad and Mega 2560.
 * Cosa does not use pin numbers are Arduino. Instead strong
 * data type is used (enum types) for the specific pin classes;
 * e.g. InterruptPin, AnalogPin, PWMPin.
 *
 * @section Limitations
 * The pin numbers for ATmega8, ATmega168 and ATmega328P are mapped
 * as in Arduino but ATmega1280 and ATmega2560 are only symbolically
 * mapped, i.e. a pin number/digit will not work, symbols must be 
 * used, e.g., Board::D42.
 *
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_BOARD_HH__
#define __COSA_BOARD_HH__

#if defined(__AVR_ATmega8__)			\
 || defined(__AVR_ATmega168__)			\
 || defined(__AVR_ATmega328P__)

class Board {
  friend class Pin;
  friend class UART;
private:
  /**
   * Do not allow instances. This is a static singleton.
   */
  Board() {}

  /**
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin) 
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
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin < 8  ? pin : 
	    pin < 14 ? pin - 8 : 
	    pin - 14);
  }
  
  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    return (&UCSR0A);
  }

public:
  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,
    D10,
    D11,
    D12,
    D13,
    LED = D13
  };

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 14,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8
  };

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
#if !defined(__AVR_ATmega8__)
    PWM0 = D3,
    PWM1 = D5,
    PWM2 = D6,
#endif
    PWM3 = D9,
    PWM4 = D10,
    PWM5 = D11
  };

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum InterruptPin {
    EXT0 = D2,
    EXT1 = D3,
    EXT_MAX = 2
  };

  /**
   * Pins used for TWI interface (in port C, analog pins 18-19).
   */
  enum TWIPin {
    SDA = 4,
    SCL = 5
  };

 /**
   * Pins used for SPI interface (in port B, digital pins 10-13).
   */
  enum SPIPin {
    SS = 2,
    MOSI = 3,
    MISO = 4,
    SCK = 5
  };
};

#elif defined(__AVR_ATmega1284P__)

#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect 

class Board {
  friend class Pin;
  friend class UART;
private:
  /**
   * Do not allow instances. This is a static singleton.
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
	    pin < 14 ? &PIND : 
	    pin < 24 ? &PINC :
	    &PINA);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin & 0x7);
  }
  
  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    return (port == 1 ? &UCSR1A : &UCSR0A);
  }

public:
  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,
    D10,
    D11,
    D12,
    D13,
    D14,
    D15,
    D16,
    D17,
    D18,
    D19,
    D20,
    D21,
    D22,
    D23,
    LED = D13
  };

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 24,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8
  };

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
    PWM0 = D3,
    PWM1 = D4,
    PWM2 = D6,
    PWM3 = D7,
    PWM4 = D12,
    PWM5 = D13,
    PWM6 = D14,
    PWM7 = D15
  };

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum InterruptPin {
    EXT0 = D10,
    EXT1 = D11,
    EXT2 = D2,
    EXT_MAX = 3
  };

  /**
   * Pins used for TWI interface (in port C, digital pin 16, 17).
   */
  enum TWIPin {
    SDA = 1,
    SCL = 0
  };

 /**
   * Pins used for SPI interface (in port B, digital pins 4-7).
   */
  enum SPIPin {
    SS = 4,
    MOSI = 5,
    MISO = 6,
    SCK = 7
  };
};

#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect 

class Board {
  friend class Pin;
  friend class UART;
private:
  /**
   * Do not allow instances. This is a static singleton.
   */
  Board() {}

  /**
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin) 
  { 
    return (pin < 8  ? &PINE : 
	    pin < 16 ? &PINH : 
	    pin < 24 ? &PINB : 
	    pin < 32 ? &PINA : 
	    pin < 40 ? &PINC : 
	    pin < 48 ? &PIND : 
	    pin < 56 ? &PINL : 
	    pin < 64 ? &PINF : 
	    pin < 72 ? &PINK : 
	    pin < 80 ? &PINJ : 
	    &PING);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register.
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin & 0x7);
  }
  
  /**
   * Return UART Register for given Arduino serial port.
   * @param[in] port number.
   * @return UART register pointer.
   */
  static volatile uint8_t* UART(uint8_t port) 
  { 
    return (port == 1 ? &UCSR1A :
	    port == 2 ? &UCSR2A :
	    port == 3 ? &UCSR3A :
	    &UCSR0A);
  }

public:
  /**
   * Digital pin symbols; mapping from name to port<5>:bit<3>.
   */
  enum DigitalPin {
    D0 = 0,
    D1 = 1,
    D2 = 4,
    D3 = 5,
    D4 = 93,
    D5 = 3,
    D6 = 11,
    D7 = 12,
    D8 = 13,
    D9 = 14,
    D10 = 20,
    D11 = 21,
    D12 = 22,
    D13 = 23,
    D14 = 81,
    D15 = 80,
    D16 = 9,
    D17 = 8,
    D18 = 51,
    D19 = 50,
    D20 = 49,
    D21 = 48,
    D22 = 24,
    D23 = 25,
    D24 = 26,
    D25 = 27,
    D26 = 28,
    D27 = 29,
    D28 = 30,
    D29 = 31,
    D30 = 39,
    D31 = 38,
    D32 = 37,
    D33 = 36,
    D34 = 35,
    D35 = 34,
    D36 = 33,
    D37 = 32,
    D38 = 47,
    D39 = 82,
    D40 = 81,
    D41 = 80,
    D42 = 55,
    D43 = 54,
    D44 = 53,
    D45 = 52,
    D46 = 51,
    D47 = 50,
    D48 = 49,
    D49 = 48,
    D50 = 19,
    D51 = 18,
    D52 = 17,
    D53 = 16,
    LED = D13
  };

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = 56,
    A1 = 57,
    A2 = 58,
    A3 = 59,
    A4 = 60,
    A5 = 61,
    A6 = 62,
    A7 = 63,
    A8 = 64,
    A9 = 65,
    A10 = 66,
    A11 = 67,
    A12 = 68,
    A13 = 69,
    A14 = 70,
    A15 = 71
  };

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
    PWM0 = D2,
    PWM1 = D3,
    PWM2 = D4,
    PWM3 = D5,
    PWM4 = D6,
    PWM5 = D7,
    PWM6 = D8,
    PWM7 = D9,
    PWM8 = D10,
    PWM9 = D11,
    PWM10 = D12,
    PWM11 = D13
  };

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum InterruptPin {
    EXT0 = D21,
    EXT1 = D20,
    EXT2 = D19,
    EXT3 = D18,
    EXT4 = D2,
    EXT5 = D3,
    EXT_MAX = 6
  };

  /**
   * Pins used for TWI interface (in port D, digital pins 20-21).
   */
  enum TWIPin {
    SCL = 0,
    SDA = 1
  };

  /**
   * Pins used for SPI interface (in port B, digital pins 50-53).
   */
  enum SPIPin {
    SS = 0,
    SCK = 1,
    MOSI = 2,
    MISO = 3
  };
};

#elif defined(__AVR_ATtiny25__)		\
   || defined(__AVR_ATtiny45__)		\
   || defined(__AVR_ATtiny85__)

#define ANALOG_COMP_vect ANA_COMP_vect
#define TIMSK0 TIMSK
#define TIFR0 TIFR
#define WDTCSR WDTCR

class Board {
  friend class Pin;
  friend class UART;
private:
  /**
   * Do not allow instances. This is a static singleton.
   */
  Board() {}

  /**
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin) 
  { 
    return (&PINB);
  }

  /**
   * Return bit position for given Arduino pin number in Special
   * Function Register. 
   * @param[in] pin number.
   * @return pin bit position.
   */
  static const uint8_t BIT(uint8_t pin)
  {
    return (pin);
  }
  
public:
  /**
   * Digital pin symbols
   */
  enum DigitalPin {
    D0 = 0,
    D1,
    D2,
    D3,
    D4,
    D5,
    LED = D4
  };

  /**
   * Analog pin symbols
   */
  enum AnalogPin {
    A0 = D5,
    A1 = D4,
    A2 = D3,
    A3 = D2,
    A4 = D1,
    A5 = D0
  };

  /**
   * PWM pin symbols; sub-set of digital pins to allow compile 
   * time checking
   */
  enum PWMPin {
    PWM0 = D0,
    PWM1 = D1
  };

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum InterruptPin {
    EXT0 = D2,
    EXT_MAX = 1
  };

  /**
   * Pins used for TWI interface.
   */
  enum TWIPin {
    SDA = 0,
    SCL = 2
  };

 /**
   * Pins used for SPI interface.
   */
  enum SPIPin {
    SS = 4,
    MOSI = 0,
    MISO = 1,
    SCK = 2
  };
};

#endif
#endif

