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
    SET_Y_ADDR = 0x40,		// Sets Y-address of RAM (0..5)
      Y_ADDR_MASK = 0x07,	// Mask Y-address
    SET_X_ADDR = 0x80,		// Sets X-address of RAM (0..83)
      X_ADDR_MASK = 0x7f,	// Mask X-addres
    SET_TEMP_COEFF = 0x04,	// Set temperature coefficient (0..3)
    SET_BIAS_SYS = 0x10,	// Set Bias System (0..7)
    SET_VOP = 0x80,		// Write Vop to register
      VOP_MASK = 0x7f		// Mask Vop
  } __attribute__((packed));

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

  /**
   * Set display address for next data block.
   * @param[in] x position 0..WIDTH-1.
   * @param[in] y position 0..HEIGHT-1.
   */
  void set_address(uint8_t x, uint8_t y);
  
  /**
   * Fill display with given data.
   * @param[in] data to fill with.
   * @param[in] count number of bytes to fill.
   */
  void fill(uint8_t data, uint16_t count);

public:
  enum DisplayMode {
    DISPLAY_OFF = 0x00,
    DISPLAY_ON = 0x01,
    NORMAL_MODE = 0x04,	
    INVERSE_MODE = 0x05
  } __attribute__((packed));
  enum TextMode {
    NORMAL_TEXT_MODE = 0x00,
    INVERTED_TEXT_MODE = 0xff
  } __attribute__((packed));
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
   * @param[in] level contrast.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint8_t level = 0x31);

  /**
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Set display mode. 
   * @param[in] mode new display mode.
   */
  void set_display_mode(DisplayMode mode);

  /**
   * Set display contrast (0..127).
   * @param[in] contrast level.
   */
  void set_display_contrast(uint8_t level);

  /**
   * Set cursor to given position.
   * @param[in] x pixel position (0..83).
   * @param[in] y line position (0..5).
   */
  void set_cursor(uint8_t x, uint8_t y);

  /**
   * Set text mode. Return previous text mode.
   * @param[in] mode new text mode.
   * @return previous text mode.
   */
  TextMode set_text_mode(TextMode mode)
  {
    TextMode previous = (TextMode) m_mode;
    m_mode = mode;
    return (previous);
  }

  /**
   * Set text font. Returns previous setting.
   * @param[in] font.
   * @return previous font setting.
   */
  Font* set_text_font(Font* font)
  {
    Font* previous = m_font;
    m_font = font;
    return (previous);
  }

  /**
   * Draw icon in the current mode. The icon must be stored in program
   * memory with width, height and data.
   * @param[in] bp
   */
  void draw_icon(const uint8_t* bp);

  /**
   * Draw bitmap in the current mode. 
   * @param[in] bp
   */
  void draw_bitmap(uint8_t* bp, uint8_t width, uint8_t height);

  /**
   * Draw a bar at the current position with the given width.
   * The bar is filled from left to right proportional to the
   * given procent (0..100).
   * @param[in] procent filled from left to right.
   * @param[in] width of bar.
   * @param[in] pattern of filled section of bar.
   */
  void draw_bar(uint8_t procent, uint8_t width, uint8_t pattern = 0x55);

  /**
   * @override
   * Write character to display. Handles carriage-return-line-feed, and
   * form-feed. Returns character or EOF on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);
};

#endif
