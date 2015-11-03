/**
 * @file PCF8574.hh
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

#ifndef COSA_PCF8574_HH
#define COSA_PCF8574_HH

#include "Cosa/TWI.hh"

/**
 * Driver for the PCF8574/PCF8574A Remote 8-bit I/O expander for
 * I2C-bus with interrupt.
 *
 * @section Circuit
 * The I/0 expander used for LCD 1602 I2C communication.
 * @code
 *                          PCF8574A
 *                       +------------+
 * (GND)---[ ]---------1-|A0       VCC|-16--------------(VCC)
 * (GND)---[ ]---------2-|A1       SDA|-15-----------(SDA/A4)
 * (GND)---[ ]---------3-|A2       SCL|-14-----------(SCL/A5)
 * (P0)----------------4-|P0       INT|-13
 * (P1)----------------5-|P1        P7|-12---------------(P7)
 * (P2)----------------6-|P2        P6|-11---------------(P6)
 * (P3)----------------7-|P3        P5|-10---------------(P5)
 * (GND)---------------8-|GND       P4|-9----------------(P4)
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. NXP Semiconductors Product data sheet, Rev. 5, 27 May 2013.
 */
class PCF8574 : private TWI::Driver {
public:
  /**
   * Construct connection to PCF8574 Remote 8-bit I/O expander with
   * given sub-address.
   * @param[in] subaddr sub-address (0..7, default 7).
   */
  PCF8574(uint8_t subaddr = 7) :
    TWI::Driver(0x20 | (subaddr & 0x7)),
    m_ddr(0xff),
    m_port(0)
  {}

  /**
   * Set data direction for port pin P0..P7; 0 for output, 1 for input.
   * Return true if set otherwise false.
   * @param[in] ddr data direction mask.
   * @return bool.
   */
  bool data_direction(uint8_t ddr);

  /**
   * Set given pin as input. Return true if set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool input_pin(uint8_t pin)
    __attribute__((always_inline))
  {
    return (data_direction(m_ddr | _BV(pin & PIN_MASK)));
  }

  /**
   * Set given pin as output. Return true if set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool output_pin(uint8_t pin)
    __attribute__((always_inline))
  {
    return (data_direction(m_ddr & ~_BV(pin & PIN_MASK)));
  }

  /**
   * Read given pin and return true is set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool read(uint8_t pin)
    __attribute__((always_inline))
  {
    return ((read() & _BV(pin & PIN_MASK)) != 0);
  }

  /**
   * Read pins and return current values.
   * @return input pin values.
   */
  uint8_t read();

  /**
   * Write set given output pin if value is non-zero, otherwise clear.
   * Return true if successful otherwise false.
   * @param[in] pin number (0..7).
   * @param[in] value.
   * @return bool.
   */
  bool write(uint8_t pin, uint8_t value);

  /**
   * Write given value to the output pins. Return true if successful
   * otherwise false. Return true if successful otherwise false.
   * @param[in] value.
   * @return bool.
   */
  bool write(uint8_t value);

  /**
   * Write given values to the output pins. Return true if successful
   * otherwise false. Return true if successful otherwise false.
   * @param[in] buf pointer to data to write to device.
   * @param[in] size of buffer.
   * @return bool.
   */
  bool write(void* buf, size_t size);

protected:
  /** Pin number mask. */
  static const uint8_t PIN_MASK = 0x07;

  /** Data Direction Register, 0 = output, 1 = input, default all input. */
  uint8_t m_ddr;

  /** Port Register to mask and maintain output pin values. */
  uint8_t m_port;

  /**
   * Construct connection to PCF8574 Remote 8-bit I/O expander with
   * given address.
   * @param[in] addr bus address.
   * @param[in] subaddr device sub address.
   */
  PCF8574(uint8_t addr, uint8_t subaddr) :
    TWI::Driver(addr | (subaddr & 0x7)),
    m_ddr(0xff),
    m_port(0)
  {}
};

class PCF8574A : protected PCF8574 {
public:
  /**
   * Construct connection to PCF8574A Remote 8-bit I/O expander with
   * given sub-address.
   * @param[in] subaddr sub-address (0..7, default 7).
   */
  PCF8574A(uint8_t subaddr = 7) : PCF8574(0x38, subaddr) {}
};
#endif
