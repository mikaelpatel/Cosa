/**
 * @file Cosa/IOStream/Driver/PCD8544.hh
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
 * @section Description
 * PCD8544 48x84 pixels matrix LCD controller/driver, device driver 
 * for IOStream access. Binding to trace, etc. For furter details
 * see Product Specification, Philips Semiconductors, 1999 Apr 12.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_DRIVER_PCD8544_HH__
#define __COSA_IOSTREAM_DRIVER_PCD8544_HH__

#include "Cosa/Pins.hh"
#include "Cosa/Board.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/Font/System5x7.hh"

class PCD8544 : public IOStream::Device {
protected:
  /**
   * Instruction set (table 1, pp. 14)
   */
  enum {
    NOP = 0x00,			// No operation
    SET_FUNC = 0x20,		// Set function
      BASIC_INST = 0x00,	// Basic instruction set
      EXTENDED_INST = 0x01,	// Extended instruction set control
      HORIZONTAL_ADDR = 0x00,	// Horizontal addressing
      VERTICAL_ADDR = 0x02,	// Vertical addressing
      POWER_UP_MODE = 0x00,	// Power up mode
      POWER_DOWN_MODE = 0x04,	// Power down mode
    DISPLAY_CNTL = 0x08,	// Display control
      DISPLAY_OFF = 0x00,	// All display segments off
      DISPLAY_ON = 0x01,	// All display segments on
      NORMAL_MODE = 0x04,	// Normal mode
      INVERSE_MODE = 0x05,	// Inverse video mode
    SET_Y_ADDR = 0x40,		// Sets Y-address of RAM (0..5)
      Y_ADDR_MASK = 0x07,	// Mask Y-address
    SET_X_ADDR = 0x80,		// Sets X-address of RAM (0..83)
      X_ADDR_MASK = 0x7f,	// Mask X-addres
    SET_TEMP_COEFF = 0x04,	// Set temperature coefficient (0..3)
    SET_BIAS_SYS = 0x10,	// Set Bias System (0..7)
    SET_VOP = 0x80		// Write Vop to register
  };

  // Initialization script to reduce memory footprint
  static const uint8_t script[] PROGMEM;

  // Display pins and state
  OutputPin m_sdin;		// Serial data input
  OutputPin m_sclk;		// Serial clock input
  OutputPin m_dc;		// Data/command
  OutputPin m_sce;		// Chip enable
  Font* m_font;			// Font
  uint8_t m_x;			// Cursor x (0..83)
  uint8_t m_y;			// Cursor y (0..5)
  uint8_t m_mode;		// Text mode (inverted)

public:
  static const uint8_t WIDTH = 84;
  static const uint8_t HEIGHT = 48;
  static const uint8_t LINES = HEIGHT / CHARBITS;

  /**
   * Construct display device driver with given pins and font.
   * @param[in] sdin screen data pin (default D6).
   * @param[in] sclk screen clock pin (default D7). 
   * @param[in] dc data/command control pin (default D8).
   * @param[in] sce screen chip enable pin (default D9).
   */
  PCD8544(Board::DigitalPin sdin = Board::D6, 
	  Board::DigitalPin sclk = Board::D7, 
	  Board::DigitalPin dc = Board::D8, 
	  Board::DigitalPin sce = Board::D9,
	  Font* font = &system5x7) :
    IOStream::Device(),
    m_sdin(sdin, 0),
    m_sclk(sclk, 0),
    m_dc(dc, 1),
    m_sce(sce, 1),
    m_font(font),
    m_x(0),
    m_y(0),
    m_mode(0)
  {}

  /**
   * Start interaction with display.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin();

  /**
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Set cursor to given position.
   * @param[in] x pixel position (0..83).
   * @param[in] y line position (0..5).
   */
  void set_cursor(uint8_t x, uint8_t y);

  /**
   * Set text mode, non-zero givens normal mode, otherwise text inverted
   * mode.
   * @param[in] mode
   */
  void set_mode(uint8_t mode)
  {
    m_mode = (mode ? 0x00 : 0xff);
  }

  /**
   * @override
   * Write character to display. Handles carriage-return-line-feed, and
   * form-feed. Returns character or EOF on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);
};

/**
 * Some syntactic sugar to capture the device selection block
 */
#define PCD8544_transaction(sce)				\
  for (uint8_t i = (sce.clear(), 1); i != 0; i--, sce.set())

#endif
