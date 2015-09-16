/**
 * @file Adafruit_I2C_LCD_Backpack.hh
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

#ifndef COSA_ADAFRUIT_I2C_LCD_BACKPACK_HH
#define COSA_ADAFRUIT_I2C_LCD_BACKPACK_HH

#include <HD44780.h>
#include <MCP23008.h>

/**
 * IO handler for HD44780 (LCD-II) Dot Matix Liquid Crystal Display
 * Controller/Driver when using the Adafruit I2C LCD Backpack with
 * MCP23008 I2C IO expander device driver.
 *
 * @section Circuit
 * @code
 *              Adafruit I2C LCD Backpack/MCP23008
 *                       +------------+
 * (SCL)---------------1-|SCL      VCC|-18--------------(VCC)
 * (SDA)---------------2-|SDA      GP7|-17--------------(LCD/BT)
 * (VCC)----[ ]--------3-|A2       GP6|-16--------------(LCD/D7)
 * (VCC)----[ ]--------4-|A1       GP5|-15--------------(LCD/D6)
 * (VCC)----[ ]--------5-|A0       GP4|-14--------------(LCD/D5)
 * (VCC)---------------6-|RESET/   GP3|-13--------------(LCD/D4)
 *                     7-|NC       GP2|-12--------------(LCD/EN)
 * (EXT)---------------8-|INT      GP1|-11--------------(LCD/RS)
 * (GND)---------------9-|GND      GP0|-10--------------(NC)
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. https://learn.adafruit.com/i2c-spi-lcd-backpack/overview
 */
class Adafruit_I2C_LCD_Backpack : public HD44780::IO, private MCP23008 {
public:
  /**
   * Construct HD44780 IO port handler using the Adafruit I2C LCD
   * Backpack with given sub-address (A0..A2).
   * @param[in] subaddr sub-address (0..7, default 0).
   */
  Adafruit_I2C_LCD_Backpack(uint8_t subaddr = 0) :
    MCP23008(subaddr),
    m_port()
  {}

  /**
   * @override{HD44780::IO}
   * Initiate TWI interface. Returns false.
   * @return bool.
   */
  virtual bool setup();

  /**
   * @override{HD44780::IO}
   * Write nibble to display using TWI interface.
   * @param[in] data (4b) to write.
   */
  virtual void write4b(uint8_t data);

  /**
   * @override{HD44780::IO}
   * Write byte (8bit) to display.
   * @param[in] data (8b) to write.
   */
  virtual void write8b(uint8_t data);

  /**
   * @override{HD44780::IO}
   * Write character buffer to display.
   * @param[in] buf pointer to buffer.
   * @param[in] size number of bytes in buffer.
   */
  virtual void write8n(const void* buf, size_t size);

  /**
   * @override{HD44780::IO}
   * Set instruction/data mode; zero for instruction,
   * non-zero for data mode.
   * @param[in] flag.
   */
  virtual void set_mode(uint8_t flag);

  /**
   * @override{HD44780::IO}
   * Set backlight on/off.
   * @param[in] flag.
   */
  virtual void set_backlight(uint8_t flag);

protected:
  /** Max size of temporary buffer for TWI message. */
  static const uint8_t TMP_MAX = 32;

  /** Expander port bit fields; little endian. */
  union port_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {
      uint8_t nc:1;		//!< Not connected (GP0).
      uint8_t rs:1;		//!< Command/Data select (GP1).
      uint8_t en:1;		//!< Enable pulse (GP2).
      uint8_t data:4;		//!< Data port (GP3..GP6).
      uint8_t bt:1;		//!< Back-light (GP7).
    };
    operator uint8_t()
    {
      return (as_uint8);
    }
    port_t()
    {
      as_uint8 = 0;
    }
  };
  port_t m_port;		//!< Port setting.
};

#endif
