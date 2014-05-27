/**
 * @file Cosa/LCD/Driver/PCD8544.hh
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

#ifndef COSA_LCD_DRIVER_PCD8544_HH
#define COSA_LCD_DRIVER_PCD8544_HH

#include "Cosa/Board.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/LCD.hh"
#include "Cosa/SPI.hh"
#include "Cosa/Canvas/Font.hh"
#include "Cosa/Canvas/Font/System5x7.hh"

/**
 * PCD8544 48x84 pixels matrix LCD controller/driver, device driver 
 * for IOStream access. Binding to trace, etc. Supports simple text  
 * scroll, cursor, and handling of special characters such as
 * form-feed, back-space and new-line. Graphics may be performed
 * with OffScreen Canvas and copied to the display with draw_bitmap().
 *
 * @section References
 * 1. Product Specification, Philips Semiconductors, 1999 Apr 12.  
 * https://www.sparkfun.com/datasheets/LCD/Monochrome/Nokia5110.pdf 
 */
class PCD8544 : public LCD::Device {
public:
  /**
   * Abstract PCD8544 LCD IO adapter to isolate communication specific
   * functions and allow access over software serial or hardware SPI.
   */
  class IO {
  public:
    /**
     * @override PCD8544::IO
     * Initiate IO port. Called by PCD8544::begin(). 
     */
    virtual void setup() {}

    /**
     * @override PCD8544::IO
     * Start of data/command transfer block.
     */
    virtual void begin() = 0;

    /**
     * @override PCD8544::IO
     * End of data/command transfer block.
     */
    virtual void end() = 0;

    /**
     * @override PCD8544::IO
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data) = 0;

    /**
     * @override PCD8544::IO
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(void* buf, size_t size) = 0;
  };

  /**
   * PCD8544 IO adapter for serial 3 wire, output pins.
   *
   * @section Circuit
   * PCD8544 is a low voltage device (3V3) and signals require level
   * shifter (74HC4050 or 10K resistor). 
   * @code
   *                          PCD8544
   *                       +------------+
   * (RST)---| > |-------1-|RST         |
   * (D9/D3)-| > |-------2-|CE          |
   * (D8/D2)-| > |-------3-|DC          |
   * (D6/D0)-| > |-------4-|DIN         |
   * (D7/D1)-| > |-------5-|CLK         |
   * (3V3)---------------6-|VCC         |
   * (GND)---|220|-------7-|LED         |
   * (GND)---------------8-|GND         |
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
     * @override PCD8544::IO
     * Start of data/command transfer block.
     */
    virtual void begin()
    { 
      m_sce.clear();
    }

    /**
     * @override PCD8544::IO
     * End of data/command transfer block.
     */
    virtual void end()
    { 
      m_sce.set();
    }

    /**
     * @override PCD8544::IO
     * Write byte (8bit) to display. Must be in data/command transfer
     * block.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data)
    { 
      m_sdin.write(data, m_sclk); 
    }

    /**
     * @override PCD8544::IO
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
   * PCD8544 IO adapter for 3 wire SPI; MOSI, SCK and SCE.
   *
   * @section Circuit
   * PCD8544 is a low voltage device (3V3) and signals require level
   * shifter (74HC4050 or 10K resistor). 
   * @code
   *                           PCD8544
   *                        +------------+
   * (RST)---------| > |--1-|RST         |
   * (D9/D3)-------| > |--2-|CE          |
   * (D8/D2)-------| > |--3-|DC          |
   * (MOSI/D11/D5)-| > |--4-|DIN         |
   * (SCK/D13/D4)--| > |--5-|CLK         |
   * (3V3)----------------6-|VCC         |
   * (GND)---|220|--------7-|LED         |
   * (GND)----------------8-|GND         |
   *                        +------------+
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
     * @override PCD8544::IO
     * Start of data/command transfer block.
     */
    virtual void begin()
    { 
      spi.begin(this);
    }

    /**
     * @override PCD8544::IO
     * End of data/command transfer block.
     */
    virtual void end()
    { 
      spi.end();
    }

    /**
     * @override PCD8544::IO
     * Write byte (8bit) to display. Must be in data/command transfer
     * block.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data)
    { 
      spi.transfer(data);
    }

    /**
     * @override PCD8544::IO
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

  /** Display size */
  static const uint8_t WIDTH = 84;
  static const uint8_t HEIGHT = 48;
  static const uint8_t LINES = HEIGHT / CHARBITS;

  /**
   * Construct display device driver with given io adapter, chip
   * select pin and font.
   * @param[in] io adapter;
   * @param[in] dc data/command control pin (default D8/D2).
   * @param[in] font bitmap (default System 5X7).
   */
#if !defined(BOARD_ATTINY)
  PCD8544(IO* io, Board::DigitalPin dc = Board::D8,
	  Font* font = &system5x7);
#else
  PCD8544(IO* io, Board::DigitalPin dc = Board::D2,
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
  Font* get_text_font() const
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
   * given precent (0..100).
   * @param[in] precent filled from left to right.
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
  IO* m_io;			//<! Display adapter
  OutputPin m_dc;		//<! Data/command output pin
  Font* m_font;			//<! Font

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
};

#endif
