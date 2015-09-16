/**
 * @file ERM1602_5.hh
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

#ifndef COSA_ERM1602_5_HH
#define COSA_ERM1602_5_HH

#include <HD44780.h>
#include "Cosa/OutputPin.hh"

/**
 * ERM1602-5 Series, Character Display Module, adapter with digital
 * output pins.
 *
 * @section Circuit
 * The serial 3-line SPI circuit for ERM1602-5.
 * @code
 *                          ERM1602-5
 *                       +------------+
 * (GND)---------------1-|VSS         |
 * (VCC)---------------2-|VDD         |
 *                     3-|VO          |
 * (D5)----------------4-|CS          |
 * (D6)----------------5-|SCL         |
 * (D7)----------------6-|DI          |
 *                     7-|PS          |
 *                     8-|RST         |
 * (VCC)---------------9-|LED+        |
 * (GND)--------------10-|LED-        |
 *                       +------------+
 * @endcode
 */
class ERM1602_5 : public HD44780::IO {
public:
  /**
   * Construct ERM1602-5 3-wire serial port connected to given serial
   * data, clock, enable pulse and backlight control pin.
   * @param[in] sda serial data pin (Default D7)
   * @param[in] scl serial clock pin (Default D6)
   * @param[in] en enable pulse (Default D5)
   * @param[in] bt backlight control (Default D4)
   */
#if !defined(BOARD_ATTINY)
  ERM1602_5(Board::DigitalPin sda = Board::D7,
	    Board::DigitalPin scl = Board::D6,
	    Board::DigitalPin en = Board::D5,
	    Board::DigitalPin bt = Board::D4);
#else
  ERM1602_5(Board::DigitalPin sda = Board::D1,
	    Board::DigitalPin scl = Board::D2,
	    Board::DigitalPin en = Board::D3,
	    Board::DigitalPin bt = Board::D4);
#endif

  /**
   * @override{HD44780::IO}
   * Initiate serial port. Returns true.
   * @return bool.
   */
  virtual bool setup();

  /**
   * @override{HD44780::IO}
   * Write LSB nibble to display using serial port.
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
   * Set instruction/data mode using given rs pin; zero for
   * instruction, non-zero for data mode.
   * @param[in] flag.
   */
  virtual void set_mode(uint8_t flag);

  /**
   * @override{HD44780::IO}
   * Set backlight on/off using bt pin.
   * @param[in] flag.
   */
  virtual void set_backlight(uint8_t flag);

protected:
  /** Execution time delay (us). */
  static const uint16_t SHORT_EXEC_TIME = 20;

  OutputPin m_sda;	 //!< Serial data output.
  OutputPin m_scl;	 //!< Serial clock.
  OutputPin m_en;	 //!< Starts data read/write.
  OutputPin m_bt;	 //!< Back-light control (0/on, 1/off).
  uint8_t m_rs;		 //!< Register select (0/instruction, 1/data).
  uint8_t m_dirty;	 //!< Mark register select change required.

  /**
   * Flush register setting if dirty.
   */
  void flush();
};

#endif
