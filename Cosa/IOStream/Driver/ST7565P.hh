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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_IOSTREAM_DRIVER_ST7565P_HH__
#define __COSA_IOSTREAM_DRIVER_ST7565P_HH__

#include "Cosa/Pins.hh"
#include "Cosa/Board.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/Font/System5x7.hh"

/**
 * ST7565P 64x128 pixels matrix LCD controller/driver, device driver 
 * for IOStream access. Binding to trace, etc. 
 *
 * @section See Also
 * For further details see Sitronix 65x132 Dot Matrix LCD Controller/
 * Driver, Ver 1.3, 2004 May 18.
 */
class ST7565P : public IOStream::Device {
protected:
  /**
   * Instruction set (table 16, pp. 52)
   */
  enum {
    DISPLAY_OFF = 0xAE,
    DISPLAY_ON = 0xAF,
    SET_DISPLAY_START = 0x40,
    DISPLAY_START_MASK = 0x3f,
    SET_Y_ADDR = 0xB0,
    Y_ADDR_MASK = 0x0f,
    SET_X_ADDR = 0x10,
    X_ADDR_MASK = 0x0f,
    ADC_NORMAL = 0xA0,
    ADC_REVERSE = 0xA1,
    DISPLAY_NORMAL = 0xA6,
    DISPLAY_REVERSE = 0xA7,
    DISPLAY_NORMAL_POINTS = 0xA4,
    DISPLAY_ALL_POINTS = 0xA5,
    LCD_BIAS_9 = 0xA2,
    LCD_BIAS_7 = 0xA3,
    X_ADDR_INC = 0xE0,
    X_ADDR_CLEAR = 0xEE,
    INTERNAL_RESET = 0xE2,
    COM_OUTPUT_NORMAL = 0xC0,
    COM_OUTPUT_REVERSE = 0xC8,
    SET_POWER_CONTROL = 0x28,
    POWER_MASK = 0x07,
    SET_RESISTOR_RATIO = 0x20,
    RESISTOR_MASK = 0x07,
    SET_CONSTRAST = 0x81,
    CONSTRAST_MASK = 0x3f,
    INDICATOR_OFF = 0xAC,
    INDICATOR_ON = 0xAD,
    FLASHING_OFF = 0x00,
    FLASHING_ON = 0x01,
    SET_BOOSTER_RATIO = 0xF8,
    BOOSTER_RATIO_234X = 0,
    BOOSTER_RATIO_5X = 1,
    BOOSTER_RATIO_6X = 3,
    NOP = 0xE3,
    SCRIPT_PAUSE = 0xF0,
    SCRIPT_END = 0xFF
  } __attribute__((packed));

  // Initialization script to reduce memory footprint
  static const uint8_t script[] PROGMEM;

  // Display pins and state
  OutputPin m_si;		// Serial input
  OutputPin m_scl;		// Serial clock input
  OutputPin m_dc;		// Data(1) or command(0)
  OutputPin m_cs;		// Chip select
  Font* m_font;			// Font
  uint8_t m_x;			// Cursor x (0..WIDTH-1)
  uint8_t m_y;			// Cursor y (0..LINES-1)
  uint8_t m_mode;		// Text mode (inverted)

  /**
   * Set display address for next data block.
   * @param[in] x position (0..WIDTH-1).
   * @param[in] y position (0..LINES-1).
   */
  void set(uint8_t x, uint8_t y);
  
  /**
   * Write given data to display according to mode.
   * Chip select and/or Command/Data pin asserted.
   * @param[in] data to fill write to device.
   */
  void write(uint8_t data)
  {
    m_si.write(data, m_scl);
  }

  /**
   * Fill display with given data.
   * @param[in] data to fill with.
   * @param[in] count number of bytes to fill.
   */
  void fill(uint8_t data, uint16_t count) 
  {
    m_cs.clear();
    for (uint16_t i = 0; i < count; i++) 
      write(data);
    m_cs.set();
  }

public:
  enum DisplayMode {
    NORMAL_DISPLAY_MODE,
    REVERSE_DISPLAY_MODE
  } __attribute__((packed));
  enum TextMode {
    NORMAL_TEXT_MODE = 0x00,
    INVERTED_TEXT_MODE = 0xff
  } __attribute__((packed));
  static const uint8_t WIDTH = 128;
  static const uint8_t HEIGHT = 64;
  static const uint8_t LINES = 8;

  /**
   * Construct display device driver with given pins and font.
   * Defaults are digital pin for Arduino/Tiny.
   * @param[in] si screen data pin (default D0/D6).
   * @param[in] scl screen clock pin (default D1/D7). 
   * @param[in] dc data/command control pin (default D2/D8).
   * @param[in] cs screen chip enable pin (default D3/D9).
   */
#if defined(__ARDUINO_TINY__)
  ST7565P(Board::DigitalPin si = Board::D0, 
	  Board::DigitalPin scl = Board::D1, 
	  Board::DigitalPin dc = Board::D2, 
	  Board::DigitalPin cs = Board::D3,
	  Font* font = &system5x7) :
#else
  ST7565P(Board::DigitalPin si = Board::D6, 
	  Board::DigitalPin scl = Board::D7, 
	  Board::DigitalPin dc = Board::D8, 
	  Board::DigitalPin cs = Board::D9,
	  Font* font = &system5x7) :
#endif
    IOStream::Device(),
    m_si(si, 0),
    m_scl(scl, 0),
    m_dc(dc, 1),
    m_cs(cs, 1),
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
  bool begin(uint8_t level = 0x10);

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
   * Set display contrast (0..63).
   * @param[in] contrast level.
   */
  void set_display_contrast(uint8_t level);

  /**
   * Get current cursor position.
   * @param[out] x pixel position (0..WIDTH-1).
   * @param[out] y line position (0..LINES-1).
   */
  void get_cursor(uint8_t& x, uint8_t& y)
  {
    x = m_x;
    y = m_y;
  }

  /**
   * Set cursor to given position.
   * @param[in] x pixel position (0..WIDTH-1).
   * @param[in] y line position (0..LINES-1).
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
