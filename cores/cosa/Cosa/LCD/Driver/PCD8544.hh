/**
 * @file Cosa/LCD/Driver/PCD8544.hh
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

#ifndef __COSA_LCD_DRIVER_PCD8544_HH__
#define __COSA_LCD_DRIVER_PCD8544_HH__

#include "Cosa/Board.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/LCD.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/Font/System5x7.hh"

/**
 * PCD8544 48x84 pixels matrix LCD controller/driver, device driver 
 * for IOStream access. Binding to trace, etc. Supports simple text 
 * scroll, cursor, and handling of special characters such as form-feed, 
 * back-space and new-line. Graphics should be performed with OffScreen
 * Canvas and copied to the display with draw_bitmap().
 *
 * @section See Also
 * For furter details see Product Specification, Philips
 * Semiconductors, 1999 Apr 12. 
 * https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf 
 */
class PCD8544 : public LCD::Device {
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
      DISPLAY_OFF = 0x00,	// Turn display off
      DISPLAY_ON = 0x01,	// Turn display on
      NORMAL_MODE = 0x04,	// Normal display mode
      INVERSE_MODE = 0x05,	// Inverse display mode
    SET_Y_ADDR = 0x40,		// Sets Y-address of RAM (0..5)
      Y_ADDR_MASK = 0x07,	// Mask Y-address
    SET_X_ADDR = 0x80,		// Sets X-address of RAM (0..83)
      X_ADDR_MASK = 0x7f,	// Mask X-addres
    SET_TEMP_COEFF = 0x04,	// Set temperature coefficient (0..3)
    SET_BIAS_SYS = 0x10,	// Set Bias System (0..7)
    SET_VOP = 0x80,		// Write Vop to register
      VOP_MASK = 0x7f,		// Mask Vop
    SCRIPT_END = 0xff		// Init script end
  } __attribute__((packed));

  // Initialization script to reduce memory footprint
  static const uint8_t script[] PROGMEM;

  // Display pins and state
  OutputPin m_sdin;		/**< Serial data input */
  OutputPin m_sclk;		/**< Serial clock input */
  OutputPin m_dc;		/**< Data/command */
  OutputPin m_sce;		/**< Chip enable */
  Font* m_font;			/**< Font */

  /**
   * Write given data to display according to mode.
   * @param[in] data to fill write to device.
   */
  void write(uint8_t data)
  {
    m_sdin.write(data, m_sclk);
  }

  /**
   * Set the given command code.
   * @param[in] cmd command code.
   */
  void set(uint8_t cmd);
  
  /**
   * Set display address for next data block.
   * @param[in] x position 0..WIDTH-1.
   * @param[in] y position 0..HEIGHT-1.
   */
  void set(uint8_t x, uint8_t y);
  
  /**
   * Fill display with given data.
   * @param[in] data to fill with.
   * @param[in] count number of bytes to fill.
   */
  void fill(uint8_t data, uint16_t count);

public:
  /** Display size */
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
#if defined(__ARDUINO_TINY__)
  PCD8544(Board::DigitalPin sdin = Board::D0, 
	  Board::DigitalPin sclk = Board::D1, 
	  Board::DigitalPin dc = Board::D2, 
	  Board::DigitalPin sce = Board::D3,
	  Font* font = &system5x7) :
#else
  PCD8544(Board::DigitalPin sdin = Board::D6, 
	  Board::DigitalPin sclk = Board::D7, 
	  Board::DigitalPin dc = Board::D8, 
	  Board::DigitalPin sce = Board::D9,
	  Font* font = &system5x7) :
#endif
    LCD::Device(),
    m_sdin(sdin, 0),
    m_sclk(sclk, 0),
    m_dc(dc, 1),
    m_sce(sce, 1),
    m_font(font)
  {}

  /**
   * @override LCD::Device
   * Start interaction with display.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin();

  /**
   * @override LCD::Device
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end();

  /**
   * @override LCD::Device
   * Set display contrast (0..127).
   * @param[in] contrast level.
   */
  virtual void display_contrast(uint8_t level);

  /**
   * @override LCD::Device
   * Turn display on. 
   */
  virtual void display_on();

  /**
   * @override LCD::Device
   * Turn display off. 
   */
  virtual void display_off();

  /**
   * @override LCD::Device
   * Display normal mode.
   */
  virtual void display_normal();

  /**
   * @override LCD::Device
   * Display inverse mode. 
   */
  virtual void display_inverse();

  /**
   * @override LCD::Device
   * Clear display and move cursor to home.
   */
  virtual void display_clear();

  /**
   * @override LCD::Device
   * Set cursor to given position.
   * @param[in] x pixel position (0..WIDTH-1).
   * @param[in] y line position (0..LINES-1).
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

  /**
   * Get current text font. 
   * @return font setting.
   */
  Font* get_text_font()
  {
    return (m_font);
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
   * @param[in] bp.
   * @param[in] width.
   * @param[in] height.
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
   * @override IOStream::Device
   * Write character to display. Handles carriage-return-line-feed, back-
   * space, alert, horizontal tab and form-feed. Returns character or EOF 
   * on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);
};

#endif
