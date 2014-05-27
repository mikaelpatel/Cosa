/**
 * @file Cosa/LCD/Driver/ST7565.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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

#ifndef COSA_LCD_DRIVER_ST7565_HH
#define COSA_LCD_DRIVER_ST7565_HH

#include "Cosa/Board.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/LCD.hh"
#include "Cosa/SPI.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/Font/System5x7.hh"

/**
 * ST7565 64x128 pixels matrix LCD controller/driver, device driver
 * for IOStream/LCD access. Binding to trace, etc. Support natural
 * text scroll, cursor, and handling of special characters such as
 * carriage-return, form-feed, back-space, horizontal tab and
 * new-line. Graphics should be performed with OffScreen Canvas and
 * copied to the display with draw_bitmap(). 
 *
 * @section References
 * 1. Sitronix 65x132 Dot Matrix LCD Controller/Driver, Ver 1.3, 2004 May 18.
 */
class ST7565 : public LCD::Device {
public:
  /**
   * Abstract ST7565 LCD IO adapter to isolate communication specific
   * functions and allow access over software serial or hardware SPI.
   */
  class IO {
  public:
    /**
     * @override ST7565::IO
     * Initiate IO port. Called by ST7565::begin(). 
     */
    virtual void setup() {}

    /**
     * @override ST7565::IO
     * Start of data/command transfer block.
     */
    virtual void begin() = 0;

    /**
     * @override ST7565::IO
     * End of data/command transfer block.
     */
    virtual void end() = 0;

    /**
     * @override ST7565::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data) = 0;

    /**
     * @override ST7565::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(void* buf, size_t size) = 0;
  };

  /**
   * ST7565 IO adapter for serial 3 wire, output pins.
   *
   * @section Circuit
   * @code
   *                       ST7565/Serial3W
   *                       +------------+
   *                     1-|DB0         |
   *                     2-|DB1         |
   *                     3-|DB2         |
   *                     4-|DB3         |
   *                     5-|DB4         |
   *                     6-|DB5         |
   * (D7/D1)-------------7-|DB6(SCL)    |
   * (D6/D0)-------------8-|DB7(SI)     |
   * (VCC)---------------9-|VDD         |
   * (GND)--------------10-|VSS         |
   * (VCC)---|220|------11-|A           |
   * (D9/D3)------------12-|CS          |
   * (RST)--------------13-|RST         |
   * (D8/D2)------------14-|DC          |
   *                    15-|WR(R/W)     |
   *                    16-|RD(E)       |
   *                       +------------+
   * @endcode
   */
  class Serial3W : public IO {
  public:
    /**
     * Construct display device driver adapter with given pins.
     * @param[in] sdin screen data pin (default D6/D0).
     * @param[in] sclk screen clock pin (default D7/D1). 
     * @param[in] sce screen chip enable pin (default D9/D3).
     */
#if defined(BOARD_ATTINY)
    Serial3W(Board::DigitalPin sdin = Board::D0, 
	     Board::DigitalPin sclk = Board::D1, 
	     Board::DigitalPin sce = Board::D3) :
      m_sdin(sdin, 0),
      m_sclk(sclk, 0),
      m_sce(sce, 1)
    {
    }
#else
    Serial3W(Board::DigitalPin sdin = Board::D6, 
	     Board::DigitalPin sclk = Board::D7, 
	     Board::DigitalPin sce = Board::D9) :
      m_sdin(sdin, 0),
      m_sclk(sclk, 0),
      m_sce(sce, 1)
    {
    }
#endif

    /**
     * @override ST7565::IO
     * Start of data/command transfer block.
     */
    virtual void begin()
    { 
      m_sce.clear();
    }

    /**
     * @override ST7565::IO
     * End of data/command transfer block.
     */
    virtual void end()
    { 
      m_sce.set();
    }

    /**
     * @override ST7565::IO
     * Write byte (8bit) to display. Must be in data/command transfer
     * block.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data)
    { 
      m_sdin.write(data, m_sclk); 
    }

    /**
     * @override ST7565::IO
     * Write character buffer to display. Must be in data/command transfer
     * block.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(void* buf, size_t size)
    {
      uint8_t* dp = (uint8_t*) buf;
      while (size--) m_sdin.write(*dp++, m_sclk); 
    }
    
  protected:
    // Display pins and state
    OutputPin m_sdin;		//<! Serial data input
    OutputPin m_sclk;		//<! Serial clock input
    OutputPin m_sce;		//<! Chip enable
  };

  /**
   * ST7565 IO adapter for 3 wire SPI; MOSI, SCK and SCE.
   *
   * @section Circuit
   * @code
   *                        ST7565/SPI3W
   *                       +------------+
   *                     1-|DB0         |
   *                     2-|DB1         |
   *                     3-|DB2         |
   *                     4-|DB3         |
   *                     5-|DB4         |
   *                     6-|DB5         |
   * (SCK/D13/D4)--------7-|DB6(SCL)    |
   * (MOSI/D11/D5)-------8-|DB7(SI)     |
   * (VCC)---------------9-|VDD         |
   * (GND)--------------10-|VSS         |
   * (VCC)---|220|------11-|A           |
   * (D9/D3)------------12-|CS          |
   * (RST)--------------13-|RST         |
   * (D8/D2)------------14-|DC          |
   *                    15-|WR(R/W)     |
   *                    16-|RD(E)       |
   *                       +------------+
   * @endcode
   */
  class SPI3W : public IO, public SPI::Driver {
  public:
    /**
     * Construct display device driver adapter with given pins.
     * Implicit usage of SPI SCK(D13/D4) and MOSI(D11/D5).
     * @param[in] sce screen chip enable pin (default D9/D3).
     */
#if defined(BOARD_ATTINY)
    SPI3W(Board::DigitalPin sce = Board::D3) : IO(), SPI::Driver(sce) {}
#else
    SPI3W(Board::DigitalPin sce = Board::D9) : IO(), SPI::Driver(sce) {}
#endif

    /**
     * @override ST7565::IO
     * Start of data/command transfer block.
     */
    virtual void begin()
    { 
      spi.begin(this);
    }

    /**
     * @override ST7565::IO
     * End of data/command transfer block.
     */
    virtual void end()
    { 
      spi.end();
    }

    /**
     * @override ST7565::IO
     * Write byte (8bit) to display. Must be in data/command transfer
     * block.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data)
    { 
      spi.transfer(data);
    }

    /**
     * @override ST7565::IO
     * Write character buffer to display. Must be in data/command transfer
     * block.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(void* buf, size_t size)
    {
      spi.write(buf, size);
    }
  };

  // Display width and height (in pixels)
  static const uint8_t WIDTH = 128;
  static const uint8_t HEIGHT = 64;
  static const uint8_t LINES = 8;

  /**
   * Construct display device driver with given io adapter, chip
   * select pin and font.
   * @param[in] io adapter;
   * @param[in] dc data/command control pin (default D8/D2).
   * @param[in] font bitmap (default System 5X7).
   */
#if !defined(BOARD_ATTINY)
  ST7565(IO* io, Board::DigitalPin dc = Board::D8,
	 Font* font = &system5x7);
#else
  ST7565(IO* io, Board::DigitalPin dc = Board::D2,
	 Font* font = &system5x7);
#endif

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
   * Set display contrast (0..63).
   * @param[in] level.
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
  Font* get_text_font() { return (m_font); }

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
   * given percent (0..100).
   * @param[in] percent filled from left to right.
   * @param[in] width of bar.
   * @param[in] pattern of filled section of bar.
   */
  void draw_bar(uint8_t percent, uint8_t width, uint8_t pattern = 0x55);

  /**
   * @override IOStream::Device
   * Write character to display. Handles carriage-return-line-feed, back-
   * space, alert, horizontal tab and form-feed. Returns character or EOF 
   * on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

protected:
  /**
   * Instruction set (table 16, pp. 52)
   */
  enum {
    DISPLAY_OFF = 0xAE,		  // Turn display off
    DISPLAY_ON = 0xAF,		  // Turn display on
    SET_DISPLAY_START = 0x40,	  // Set start line address
    DISPLAY_START_MASK = 0x3f,	  // - line address mask
    SET_Y_ADDR = 0xB0,		  // Set page address
    Y_ADDR_MASK = 0x0f,		  // - page address mask
    SET_X_ADDR = 0x10,		  // Set column address (2x4 bits) 
    X_ADDR_MASK = 0x0f,		  // - colum address mask
    ADC_NORMAL = 0xA0,		  // Set normal address correspondence
    ADC_REVERSE = 0xA1,		  // Set reverse address correspondence
    DISPLAY_NORMAL = 0xA6,	  // Normal display mode
    DISPLAY_REVERSE = 0xA7,	  // Reverse display mode
    DISPLAY_64X128_POINTS = 0xA4, // Display normal
    DISPLAY_65X132_POINTS = 0xA5, // Display all points
    LCD_BIAS_9 = 0xA2,		  // Voltage ratio 1/9 bias
    LCD_BIAS_7 = 0xA3,		  // Voltage ratio 1/7 bias
    X_ADDR_INC = 0xE0,		  // Column address increment
    X_ADDR_CLEAR = 0xEE,	  // Clear read/modify/write
    INTERNAL_RESET = 0xE2,	  // Internal reset
    COM_OUTPUT_NORMAL = 0xC0,	  // Normal output scan direction
    COM_OUTPUT_REVERSE = 0xC8,	  // - reverse direction
    SET_POWER_CONTROL = 0x28,	  // Select internal power supply mode
    POWER_MASK = 0x07,		  // - operation mode mask
    SET_RESISTOR_RATIO = 0x20,	  // Select internal resistor ratio
    RESISTOR_MASK = 0x07,	  // - resistor ratio mask
    SET_CONSTRAST = 0x81,	  // Set output voltage volume register
    CONSTRAST_MASK = 0x3f,	  // - electronic volume mask
    INDICATOR_OFF = 0xAC,	  // Static indicator off
    INDICATOR_ON = 0xAD,	  // - on
    FLASHING_OFF = 0x00,	  // Set indicator flashing mode off
    FLASHING_ON = 0x01,		  // - on
    SET_BOOSTER_RATIO = 0xF8,	  // Set booster ratio
    BOOSTER_RATIO_234X = 0,	  // - 2x, 3x, 4x
    BOOSTER_RATIO_5X = 1,	  // - 5x
    BOOSTER_RATIO_6X = 3,	  // - 6x
    NOP = 0xE3,			  // Non-operation
    SCRIPT_PAUSE = 0xF0,	  // Init script pause (ms)
    SCRIPT_END = 0xFF		  // Init script end
  } __attribute__((packed));

  // Initialization script to reduce memory footprint
  static const uint8_t script[] PROGMEM;

  // Display pins and state
  IO* m_io;			  //<! Display adapter
  OutputPin m_dc;		  //<! Data(1) or command(0)
  uint8_t m_line;		  //<! Display start line
  Font* m_font;			  //<! Font

  /**
   * Set the given command code.
   * @param[in] cmd command code.
   */
  void set(uint8_t cmd);
  
  /**
   * Set display address for next data block.
   * @param[in] x position (0..WIDTH-1).
   * @param[in] y position (0..LINES-1).
   */
  void set(uint8_t x, uint8_t y);
  
  /**
   * Fill display with given data.
   * @param[in] data to fill with.
   * @param[in] count number of bytes to fill.
   */
  void fill(uint8_t data, uint16_t count);
};

#endif
