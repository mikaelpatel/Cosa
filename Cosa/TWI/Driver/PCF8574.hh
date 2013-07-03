/**
 * @file Cosa/TWI/Driver/PCF8574.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TWI_DRIVER_PCF8574_HH__
#define __COSA_TWI_DRIVER_PCF8574_HH__

#include "Cosa/TWI.hh"

/**
 * Driver for the PCF8574/PCF8574A Remote 8-bit I/O expander for
 * I2C-bus with interrupt. 
 *
 * @section See Also
 * NXP Semiconductors Product data sheet, Rev. 5, 27 May 2013.
 */
class PCF8574 : private TWI::Driver {
protected:
  // Sub-address mask
  static const uint8_t SUBADDR_MASK = 0x07;
  
  // Device Address 
  uint8_t m_addr;
  
  // Pin number mask
  static const uint8_t PIN_MASK = 0x07;
  
  // Data Direction Register, 0 = output, 1 = input, default all input
  uint8_t m_ddr;
  
  // Port Register to mask and maintain output pin values
  uint8_t m_port;

  /**
   * Construct connection to PCF8574 Remote 8-bit I/O expander with
   * given address.
   * @param[in] addr bus address.
   * @param[in] subaddr device sub address (0..7, default 7).
   */
  PCF8574(uint8_t addr, uint8_t subaddr = 7) :
    TWI::Driver(),
    m_addr(addr | ((subaddr & SUBADDR_MASK) << 1)),
    m_ddr(0xff),
    m_port(0)
  {
  }

public:
  // Two-wire address for PCF8574 
  static const uint8_t ADDR = 0x40;

  /**
   * Construct connection to PCF8574 Remote 8-bit I/O expander with
   * given sub-address.
   * @param[in] subaddr sub-address (0..7, default 7).
   */
  PCF8574(uint8_t subaddr = 7) :
    TWI::Driver(),
    m_addr(ADDR | ((subaddr & SUBADDR_MASK) << 1)),
    m_ddr(0xff),
    m_port(0)
  {
  }

  /**
   * Set data direction for port pin P0..P7; 0 for output, 1 for input.
   * @param[in] ddr data direction mask.
   */
  void set_data_direction(uint8_t ddr)
  {
    m_ddr = ddr;
  }

  /**
   * Set given pin as input.
   * @param[in] pin number (0..7).
   */
  void set_input_pin(uint8_t pin)
  {
    m_ddr |= _BV(pin & PIN_MASK);
  }

  /**
   * Set given pin as output.
   * @param[in] pin number (0..7).
   */
  void set_output_pin(uint8_t pin)
  {
    m_ddr &= ~_BV(pin & PIN_MASK);
  }

  /**
   * Read given pin and return true is set otherwise false.
   * @param[in] pin number (0..7).
   * @return bool.
   */
  bool read(uint8_t pin)
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
};

class PCF8574A : protected PCF8574 {
public:
  // Two-wire address for PCF8574A
  static const uint8_t ADDR = 0x70;

  /**
   * Construct connection to PCF8574A Remote 8-bit I/O expander with
   * given sub-address.
   * @param[in] subaddr sub-address (0..7, default 7).
   */
  PCF8574A(uint8_t subaddr = 7) : PCF8574(ADDR, subaddr) {}
};
#endif
