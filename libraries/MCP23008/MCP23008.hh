/**
 * @file MCP23008.hh
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

#ifndef COSA_MCP23008_HH
#define COSA_MCP23008_HH

#include "Cosa/TWI.hh"

/**
 * Driver for the MCP23008 8-bit I/O Expander with I2C Interface
 * and Interrupt.
 *
 * @section Circuit
 * @code
 *                          MCP23008
 *                       +------------+
 * (SCL)---------------1-|SCL      VCC|-18--------------(VCC)
 * (SDA)---------------2-|SDA      GP7|-17---------------(P7)
 * (A2)----[ ]---------3-|A2       GP6|-16---------------(P6)
 * (A1)----[ ]---------4-|A1       GP5|-15---------------(P5)
 * (A0)----[ ]---------5-|A0       GP4|-14---------------(P4)
 * (RST)---------------6-|RESET/   GP3|-13---------------(P3)
 *                     7-|NC       GP2|-12---------------(P2)
 * (EXT)---------------8-|INT      GP1|-11---------------(P1)
 * (GND)---------------9-|GND      GP0|-10---------------(P0)
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Microchip Technology Inc., Device Documentation, DS21919E, 2007.
 * http://ww1.microchip.com/downloads/en/DeviceDoc/21919e.pdf
 */
class MCP23008 : private TWI::Driver {
public:
  /** Interrupt Pin modes */
  enum InterruptMode {
    DISABLE = 0,		//!< Interrupt disabled.
    ON_CHANGE = 1,		//!< Interrupt on change (toggle).
    ON_FALLING = 2,		//!< Interrupt on falling (high to low).
    ON_RISING = 3		//!< Interrupt on rising (low to high).
  } __attribute__((packed));

  /**
   * Construct connection to MCP23008 8-bit I/O Expander with
   * given sub-address. All pins are input on reset. Interrupt disabled.
   * @param[in] subaddr sub-address (0..7, default 0).
   */
  MCP23008(uint8_t subaddr = 0) :
    TWI::Driver(0x20 | (subaddr & SUBADDR_MASK)),
    m_iodir(0xff),
    m_gpinten(0),
    m_defval(0),
    m_intcon(0),
    m_gppu(0),
    m_olat(0)
  {}

  /**
   * Initiate device driver. Return true if initiated otherwise false.
   * @return bool.
   */
  bool begin();

  /**
   * Set data direction for port pin P0..P7; 0 for output, 1 for input.
   * Return true if set otherwise false.
   * @param[in] iodir data direction mask.
   * @return bool.
   */
  bool data_direction(uint8_t iodir);

  /**
   * Set pullup mode for port pin P0..P7; 0 for normal, 1 for pullup
   * resistor. Return true if set otherwise false.
   * @param[in] gppu pullup resistor mask.
   * @return bool.
   */
  bool pullup(uint8_t gppu);

  /**
   * Set given pin as input. Return true if set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool input_pin(uint8_t pin)
    __attribute__((always_inline))
  {
    return (data_direction(m_iodir | _BV(pin & PIN_MASK)));
  }

  /**
   * Set given pin pullup. Return true if set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool pullup_pin(uint8_t pin)
    __attribute__((always_inline))
  {
    return (pullup(m_gppu | _BV(pin & PIN_MASK)));
  }

  /**
   * Set interrupt mode for given pin. Return true if set otherwise
   * false.
   * @param[in] pin number (0..7).
   * @param[in] mode interrupt.
   * @return bool.
   */
  bool interrupt_pin(uint8_t pin, InterruptMode mode);

  /**
   * Set given pin as output. Return true if set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool output_pin(uint8_t pin)
    __attribute__((always_inline))
  {
    return (data_direction(m_iodir & ~_BV(pin & PIN_MASK)));
  }

  /**
   * Read given pin and return true is set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool read_pin(uint8_t pin)
    __attribute__((always_inline))
  {
    return ((read() & _BV(pin & PIN_MASK)) != 0);
  }

  /**
   * Write set given output pin if value is non-zero, otherwise clear.
   * Return true if successful otherwise false.
   * @param[in] pin number (0..7).
   * @param[in] value.
   * @return bool.
   */
  bool write_pin(uint8_t pin, uint8_t value)
  __attribute__((always_inline))
  {
    uint8_t mask = _BV(pin & PIN_MASK);
    if (value)
      m_olat |= mask;
    else
      m_olat &= ~mask;
    return (write(m_olat));
  }

  /**
   * Read pins and return current values.
   * @return input pin values.
   */
  uint8_t read();

  /**
   * Read pins value into given buffer with given size. Return true if
   * successful otherwise false.
   * @param[in,out] buf pointer to buffer for data.
   * @param[in] size number of bytes to read.
   * @return bool.
   */
  bool read(void* buf, size_t size);

  /**
   * Write given value to the output pins. Return true if successful
   * otherwise false.
   * @param[in] value.
   * @return bool.
   */
  bool write(uint8_t value);

  /**
   * Write given values to the output pins. Allow sequence of updates
   * to output pins in a single write. Return true if successful
   * otherwise false.
   * @param[in] buf pointer to data to write to port.
   * @param[in] size of buffer.
   * @return bool.
   */
  bool write(void* buf, size_t size);

protected:
  /** Sub-address mask. */
  static const uint8_t SUBADDR_MASK = 0x07;

  /** Pin number mask. */
  static const uint8_t PIN_MASK = 0x07;

  /** Register Addresses, table 1-2, pp. 6. */
  enum {
    IODIR = 0x00,		//!< I/O Direction Register.
    IPOL = 0x01,		//!< Input Polarity Register.
    GPINTEN = 0x02,		//!< Interrupt on Change Control Register.
    DEFVAL = 0x03,		//!< Default Compare Register.
    INTCON = 0x04,		//!< Interrupt Control Register.
    IOCON = 0x05,		//!< Configuration Register.
    GPPU = 0x06,		//!< Pull-up Resistor Configuration.
    INTF = 0x07,		//!< Interrupt Flag Register.
    INTCAP = 0x08,		//!< Interrupt Capture Register.
    GPIO = 0x09,		//!< Port Register.
    OLAT = 0x0a,		//!< Output Latch Register.
    REG_MAX = 0x0b		//!< Number of Registers.
  } __attribute__((packed));

  /** Configuration Register Bits, pp. 15. */
  enum {
    INTPOL = 1,			//!< Polarity of Interrupt Pin.
    ODR = 2,			//!< Open-drain Output enable.
    HAEN = 3,			//!< Hardware Address Enable (SPI).
    DISSLW = 4,			//!< Slew Rate disable.
    SEQOP = 5			//!< Sequence Operation mode disable.
  } __attribute__((packed));

  /** Data Direction Register, 0 = output, 1 = input, default all input. */
  uint8_t m_iodir;

  /** Interrupt on Change Control Register, 0 = disable, 1 = enable. */
  uint8_t m_gpinten;

  /** Default Compare Register,  0 = rising, 1 = falling. */
  uint8_t m_defval;

  /** Interrupt Control Register,  0 = change/toggle, 1 = compare. */
  uint8_t m_intcon;

  /** Pullup Register, 0 = disable, 1 = pullup enable, default disable. */
  uint8_t m_gppu;

  /** Output Register values. */
  uint8_t m_olat;

  /** Current Register. */
  uint8_t m_reg;
};
#endif
