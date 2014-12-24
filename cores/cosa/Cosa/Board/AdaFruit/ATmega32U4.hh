/**
 * @file Cosa/Board/AdaFruit/ATmega32U4.hh
 * @version 1.0
 *
 * @author Contributed by jediunix
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

#ifndef COSA_BOARD_ADAFRUIT_ATMEGA32U4_HH
#define COSA_BOARD_ADAFRUIT_ATMEGA32U4_HH


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
 * AdaFruit ATmega32U4 board. Cosa does not use pin numbers as
 * Arduino/Wiring, instead strong data type is used (enum types) for
 * the specific pin classes; DigitalPin, AnalogPin, PWMPin, etc. 
 *
 * The pin numbers for ATmega32u4 are mapped as in Arduino Leonardo.
 * The static inline functions, SFR, BIT and UART, rely on compiler
 * optimizations to be reduced.  
 *
 * @section Board
 * @code
 *                      AdaFruit ATmega32u4
 *
 *        Chip           Leonardo-Equiv     Board    -----
 * vvvvvvvvvvvvvvvvvvvv  vvvvvvvvvvvvvvvvvv vvv +----|USB|
 *                                              |    |   |
 *                                          3V3 |[]  |---|
 * PE6/INT.6/AIN0....................LED/D7 *E6 |[]
 * PB0/PCINT0/SS................SS/PCI0/D20 *B0 |[]
 * PB1/PCINT1/SCLK.............SCK/PCI1/D21 *B1 |[]
 * PB2/PDI/PCINT2/MOSI........MOSI/PCI2/D22 *B2 |[]
 * PB3/PDO/PCINT3/MISO........MISO/PCI3/D23 *B3 |[]
 * PD0/OC0B/SCL/INT0.......SCL/EXT0/PWM1/D3 *D0 |[]
 * PD1/SDA/INT1.................SDA/EXT1/D2 *D1 |[]
 * PD2/RX1D/INT2....................EXT2/D0 *D2 |[]
 * PD3/TX1D/INT3....................EXT3/D1 *D3 |[]
 * PD4/ICP1/ADC8......................A6/D4 *D4 |[]
 * PD5/XCK1/!CTS...........................  D5 |[]
 * PD6/T1/!OC4D/ADC9.................A7/D12 *D6 |[]
 * PD7/T0/OC4D/ADC10.............PWM6/A8/D6 *D7 |[]
 *                                          GND |[]
 *                                        RESET |[]   ICSP
 *                                              |    o-o-o*
 *                                              |    o-o-o
 *                                              +---------
 *
 *
 * -----    Board Leonardo-Equiv             Chip
 * |USB|----+ vvv vvvvvvvvvvvvvvvvvv  vvvvvvvvvvvvvvvvvvvv
 * |   |    |
 * |---|  []| 5V
 *        []| AREF
 *        []| *F0 D19/A5..............PF0/ADC0
 *        []| *F1 D18/A4..............PF1/ADC1
 *        []| *F4 D17/A3..............PF4/ADC4/TCK
 *        []| *F5 D16/A2..............PF5/ADC5/TMS
 *        []| *F6 D15/A1..............PF6/ADC6/TDO
 *        []| *F7 D14/A0..............PF7/ADC7/TDI
 *        []| *C7 D13/PWM5............PC7/IPC3/CLK0/OC4A
 *        []| *C6 D5/PWM4.............PC6/OC3A/!OC4A
 *        []| *B7 D11/PWM0/PCI7.......PB7/PCINT7/OC0A/OC1C/!RTS
 *        []| *B6 D10/A11/PWM3/PCI6...PB6/PCINT6/OC1B/OC4B/ADC13
 *        []| *B5 D9/A10/PWM2/PCI5....PB5/PCINT5/OC1A/!OC4B/ADC12
 *        []| *B4 D8/A9/PCI4..........PB4/PCINT4/ADC11
 *        []| GND
 *  ICSP  []| Vcc
 * o-o-o*   |
 * o-o-o    |
 * ---------+
 * @endcode
 *
 * The board labels are E6, B0-B3, D0-D7, B4-B7, C6-C7, F7-F0. The
 * traditional naming convention is preserved for compatibility with
 * Leonardo; that is, D3 refers to Leonardo D3, not the board label
 * D3.
 *
 * Additional aliases of the board label are available for various uses.
 * The symbol format is symbol usage followed by 'L' for Label followed
 * by the board label.  Symbol usage is one of
 *
 * 'D'   - Digital
 * 'A'   - Analog
 * 'PWM' - PWM
 * 'EI'  - ExternalInterrupt
 * 'I'   - Interrupt
 *
 * For example, board pin labeled B5 would have aliases
 * DLB5   - Digital
 * ALB5   - Analog
 * PWMLB5 - PWM
 * ILB5   - Interrupt
 * 
 * Note: The SPI pins (on ICSP) are also numbered as digital pins.
 */
class Board {
  friend class Pin;
  friend class UART;
private:
  /**
   * Do not allow instances. This is a static singleton; name space.
   */
  Board() {}

  enum PortMap {
    PORTB_MAP = 0,
    PORTC_MAP = PORTB_MAP+8,
    PORTD_MAP = PORTC_MAP+8,
    PORTE_MAP = PORTD_MAP+8,
    PORTF_MAP = PORTE_MAP+8
  } __attribute__((packed));

  /**
   * Return Special Function Register for given Arduino pin number.
   * @param[in] pin number.
   * @return special register pointer.
   */
  static volatile uint8_t* SFR(uint8_t pin)
    __attribute__((always_inline))
  { 
    return (pin < PORTC_MAP ? &PINB : 
            pin < PORTD_MAP ? &PINC : 
            pin < PORTE_MAP ? &PIND : 
            pin < PORTF_MAP ? &PINE : 
                 &PINF);
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
    return (pin & 0x07);
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
    return (&PCMSK0);
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
    return (&UCSR1A);
  }

  /**
   * Processor pin symbols
   */
  enum ProcessorPin {
    PinE6 = PORTE_MAP + 6,
    PinB0 = PORTB_MAP + 0,
    PinB1 = PORTB_MAP + 1,
    PinB2 = PORTB_MAP + 2,
    PinB3 = PORTB_MAP + 3,
    PinD0 = PORTD_MAP + 0,
    PinD1 = PORTD_MAP + 1,
    PinD2 = PORTD_MAP + 2,
    PinD3 = PORTD_MAP + 3,
    PinD4 = PORTD_MAP + 4,
    PinD5 = PORTD_MAP + 5,
    PinD6 = PORTD_MAP + 6,
    PinD7 = PORTD_MAP + 7,
    PinB4 = PORTB_MAP + 4,
    PinB5 = PORTB_MAP + 5,
    PinB6 = PORTB_MAP + 6,
    PinB7 = PORTB_MAP + 7,
    PinC6 = PORTC_MAP + 6,
    PinC7 = PORTC_MAP + 7,
    PinF7 = PORTF_MAP + 7,
    PinF6 = PORTF_MAP + 6,
    PinF5 = PORTF_MAP + 5,
    PinF4 = PORTF_MAP + 4,
    PinF1 = PORTF_MAP + 1,
    PinF0 = PORTF_MAP + 0
  } __attribute__((packed));
    
public:
  /**
   * Digital pin symbols
   *
   * D0-D23 are connected exactly as they are in Leonardo.
   * Board labels can be addressed with the board symbol prefixed by 'L' (for Label).
   */
  enum DigitalPin {
    D0 = PinD2,
    D1 = PinD3,
    D2 = PinD1,
    D3 = PinD0,
    D4 = PinD4,
    D5 = PinC6,
    D6 = PinD7,
    D7 = PinE6,
    D8 = PinB4,
    D9 = PinB5,
    D10 = PinB6,
    D11 = PinB7,
    D12 = PinD6,
    D13 = PinC7,
    D14 = PinF7,
    D15 = PinF6,
    D16 = PinF5,
    D17 = PinF4,
    D18 = PinF1,
    D19 = PinF0,
    D20 = PinB0,
    D21 = PinB1,
    D22 = PinB2,
    D23 = PinB3,

    // Aliases
    DLE6 = PinE6,
    DLB0 = PinB0,
    DLB1 = PinB1,
    DLB2 = PinB2,
    DLB3 = PinB3,
    DLD0 = PinD0,
    DLD1 = PinD1,
    DLD2 = PinD2,
    DLD3 = PinD3,
    DLD4 = PinD4,
    DLD6 = PinD6,
    DLD7 = PinD7,
    DLB4 = PinB4,
    DLB5 = PinB5,
    DLB6 = PinB6,
    DLB7 = PinB7,
    DLC6 = PinC6,
    DLC7 = PinC7,
    DLF7 = PinF7,
    DLF6 = PinF6,
    DLF5 = PinF5,
    DLF4 = PinF4,
    DLF1 = PinF1,
    DLF0 = PinF0,
    LED = PinE6,
    TXD1 = PinD3,
    RXD1 = PinD2
  } __attribute__((packed));

  /**
   * Analog pin symbols (ADC channel numbers)
   */
  enum AnalogPin {
    A0 = PinF7,
    A1 = PinF6,
    A2 = PinF5,
    A3 = PinF4,
    A4 = PinF1,
    A5 = PinF0,
    A6 = PinD4,
    A7 = PinD6,
    A8 = PinD7,
    A9 = PinB4,
    A10 = PinB5,
    A11 = PinB6,

    // Aliases
    ALF7 = PinF7,
    ALF6 = PinF6,
    ALF5 = PinF5,
    ALF4 = PinF4,
    ALF1 = PinF1,
    ALF0 = PinF0,
    ALD4 = PinD4,
    ALD6 = PinD6,
    ALD7 = PinD7,
    ALB4 = PinB4,
    ALB5 = PinB5,
    ALB6 = PinB6    
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
    PWM0 = PinB7,  // => OCR0A
    PWM1 = PinD0,  // => OCR0B
    PWM2 = PinB5,  // => OCR1A
    PWM3 = PinB6,  // => OCR1B
    PWM4 = PinC6,  // => OCR3A
    PWM5 = PinC7,  // => OCR4A
    PWM6 = PinD7,  // => OCR4D

    // Aliases
    PWMLB7 = PinB7,
    PWMLD0 = PinD0,
    PWMLB5 = PinB5,
    PWMLB6 = PinB6,
    PWMLC6 = PinC6,
    PWMLC7 = PinC7,
    PWMLD7 = PinD7
  } __attribute__((packed));

  /**
   * External interrupt pin symbols; sub-set of digital pins 
   * to allow compile time checking.
   */
  enum ExternalInterruptPin {
    EXT0 = PinD0,
    EXT1 = PinD1,
    EXT2 = PinD2,
    EXT3 = PinD3,

    // Aliases
    EILD0 = PinD0,
    EILD1 = PinD1,
    EILD2 = PinD2,
    EILD3 = PinD3
  } __attribute__((packed));

  /**
   * Pin change interrupt (PCI) pins. Number of port registers.
   */
  enum InterruptPin {
    PCI0 = PinB0,
    PCI1 = PinB1,
    PCI2 = PinB2,
    PCI3 = PinB3,
    PCI4 = PinB4,
    PCI5 = PinB5,
    PCI6 = PinB6,
    PCI7 = PinB7,

    // Aliases
    ILB0 = PinB0,
    ILB1 = PinB1,
    ILB2 = PinB2,
    ILB3 = PinB3,
    ILB4 = PinB4,
    ILB5 = PinB5,
    ILB6 = PinB6,
    ILB7 = PinB7
  } __attribute__((packed));

  /** 
   * Size of pin maps.
   */
  enum {
    ANALOG_PIN_MAX = 12,
    DIGITAL_PIN_MAX = 24,
    EXT_PIN_MAX = 4,
    PCI_PIN_MAX = 8,
    PWM_PIN_MAX = 7
  };

  /**
   * Pins used for TWI interface (port D, bit 0-1, D0-D1)
   */
  enum TWIPin {
    SDA = PinD1,
    SCL = PinD0
  } __attribute__((packed));
  
  /**
   * Pins used for SPI interface (port B, bit 0-3)
   */
  enum SPIPin {
    SS = PinB0,
    SCK = PinB1,
    MOSI = PinB2,
    MISO = PinB3
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
