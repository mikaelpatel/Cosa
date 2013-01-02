/**
 * @file Cosa/SPI/ST7735R.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * Device driver for ST7735R, 262K Color Single-Chip TFT Controller.
 * 
 * @section See Also
 * Sitronix Technology Corp. ST7735R documentation, V2.1, 2010-02-01.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_SPI_ST7735R_HH__
#define __COSA_SPI_ST7735R_HH__

#include "Cosa/SPI.hh"
#include "Cosa/Pins.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"

class ST7735R : private SPI::Driver {

protected:
  /**
   * Slave select pin (default is pin 10)
   */
  OutputPin m_cs;

  /**
   * Data/Command select pin (default is pin 9)
   */
  OutputPin m_dc;

  /**
   * Current drawing color; RGB<5, 6, 5>
   */
  uint16_t m_color;

  /**
   * Current font size (scale 1..n) and text position.
   */
  uint8_t m_size;
  uint8_t m_x;
  uint8_t m_y;

  /**
   * Initialization script and font (in program memory)
   */
  static uint8_t script[] PROGMEM;
  static uint8_t font[] PROGMEM;

public:
  /**
   * SPI commands (ch. 10 Command, pp. 77-78, pp. 119-120)
   */
  enum Command {
    NOP = 0x0,			// No Operation
    SWRESET = 0x01,		// Software Reset
    SWDELAY = 0x02,		// Software Delay
    SCRIPTEND = 0x03,		// Script End
    RDDID = 0x04,		// Read Display ID
    RDDST = 0x09,		// Read Display Status
    RDDPM = 0x0A,		// Read Display Power Mode
    RDDMADCTL = 0x0B,		// Read Display MADCTL
    RDDCOLMOD = 0x0C,		// Read Display Pixel Format
    RDDIM = 0x0D,		// Read Display Image Mode
    RDDSM = 0x0E,		// Read Display Signal Mode
    SLPIN = 0x10,		// Sleep in
    SLPOUT = 0x11,		// Sleep out
    PTLON = 0x12,		// Partial Display Mode On
    NORON = 0x13,		// Normal Display Mode On
    INVOFF = 0x20,		// Display Inversion Off
    INVON = 0x21,		// Display Inversion On
    GAMSET = 0x26,		// Gamma Set
    DISPOFF = 0x28,		// Display Off
    DISPON = 0x29,		// Display On
    CASET = 0x2A,		// Column Address Set
    RASET = 0x2B,		// Row Address Set
    RAMWR = 0x2C,		// Memory Write
    RGBSET = 0x2D,		// Color Setting for 4K, 65K and 262K
    RAMRD = 0x2E,		// Memory Read
    PTLAR = 0x30,		// Partial Area
    TEOFF = 0x34,		// Tearing Effect Line Off
    TEON = 0x35,		// Tearing Effect Line On
    MADCTL = 0x36,		// Memory Data Access Control
    IMDOFF = 0x38,		// Idle Mode Off
    IMDON = 0x39,		// Idle Mode On
    COLMOD = 0x3A,		// Interface Pixel Format
    RDID1 = 0xDA,		// Read ID1 Value
    RDID2 = 0xDB,		// Read ID2 Value
    RDID3 = 0xDC,		// Read ID3 Value
    FRMCTR1 = 0xB1,		// Frame Rate Control, normal mode
    FRMCTR2 = 0xB2,		// Frame Rate Control, idle mode
    FRMCTR3 = 0xB3,		// Frame Rate Control, partial mode
    INVCTR = 0xB4,		// Display Inversion Control
    DISSET5 = 0xB6,		// Diaplay Function set 5
    PWCTR1 = 0xC0,		// Power Control 1
    PWCTR2 = 0xC1,		// Power Control 2
    PWCTR3 = 0xC2,		// Power Control 3, normal mode
    PWCTR4 = 0xC3,		// Power Control 4, idle mode
    PWCTR5 = 0xC4,		// Power Control 5, partial mode
    PWCTR6 = 0xFC,		// Power Control 6, partial mode
    VMCTR1 = 0xC5,		// VCOM Control 1
    VMOFCTR = 0xC7,		// VCOM Offset Control
    WRID2 = 0xD1,		// Write ID2 Value
    WRID3 = 0xD2,		// Write ID3 Value
    NVFCTR1 = 0xD9,		// EEPROM Control Status
    NVRCTR2 = 0xDE,		// EEPROM Read Command
    NVFCTR3 = 0xDF,		// EEPROM Write Command
    GMCTRP1 = 0xE0,		// Positive Gamma Correction
    GMCTRN1 = 0xE1,		// Negative Gamma Correction
    EXTCTRL = 0xF0,		// Extension Command Control
    VCOM4L = 0xFF		// VCOM 4 Level Control
  };

  /**
   * Write command to device.
   * @param[in] cmd command to write.
   */
  void write(Command cmd);

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint8_t data);

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint16_t data);

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] x data to write.
   * @param[in] y data to write.
   */
  void write(Command cmd, uint16_t x, uint16_t y);

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] buffer data buffer to write.
   * @param[in] count number of bytes to write.
   */
  void write_P(Command cmd, const void* buffer, uint8_t count);

  /**
   * Screen size; width/height
   */
  static const uint8_t SCREEN_WIDTH = 128;
  static const uint8_t SCREEN_HEIGHT = 160;

  /**
   * Font size; width/height 
   */
  static const uint8_t FONT_HEIGHT = 8;
  static const uint8_t FONT_WIDTH = 5;
  static const uint8_t CHAR_SPACING = 1;
  static const uint8_t LINE_SPACING = 2;

  /**
   * Construct display object with given control pins.
   * @param[in] cs slave selection pin (default pin 10).
   * @param[in] dc data/command selection pin (default pin 9).
   */
  ST7735R(uint8_t cs = 10, uint8_t dc = 9);

  /**
   * Start interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin()
  {
    return (spi.begin(SPI::DEFAULT_CLOCK, 3, SPI::MSB_FIRST));
  }

  /**
   * Initiate device with command and data sequence.
   * @param[in] bp boot sequence (in program memory).
   */
  void initiate(const uint8_t* bp = script);

  /**
   * Set the current display port.
   * @param[in] x0 
   * @param[in] y0
   * @param[in] x1
   * @param[in] y1
   */
  void set_port(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
  {
    write(CASET, x0, x1); 
    write(RASET, y0, y1);
    write(RAMWR);
  }

  /**
   * Basic color set.
   */
  enum {
    WHITE = 0xFFFFU,
    BLACK = 0x0000U,
    RED = 0xF800U,
    GREEN = 0x07E0U,
    BLUE = 0x001FU,
    CYAN = GREEN + BLUE,
    MAGENTA = RED + BLUE,
    YELLOW = RED + GREEN
  };
  
  /**
   * Get current drawing color.
   * @return color.
   */
  uint16_t get_color()
  {
    return (m_color);
  }

  /**
   * Set current drawing color.
   * @param[in] color
   */
  void set_color(uint16_t color)
  {
    m_color = color;
  }

  /**
   * Set current drawing color from primary colors (RGB).
   * @param[in] red
   * @param[in] green
   * @param[in] blue
   */
  void set_color(uint8_t red, uint8_t green, uint8_t blue)
  {
    m_color = (((red & 0x1f) << 11) | ((green & 0x3f) << 5) | (blue & 0x1f));
  }

  /**
   * Set current drawing color to gray scale shade (0..100%)
   * @param[in] scale
   */
  void set_gray(uint8_t scale)
  {
    if (scale > 100) scale = 100;
    uint8_t level = (scale * 0x1fU) / 100;
    set_color(level, level << 1, level);
  }

  /**
   * Get current font size.
   * @return size.
   */
  uint8_t get_size(uint8_t size)
  {
    return (m_size);
  }

  /**
   * Set current font size.
   * @param[in] size
   */
  void set_size(uint8_t size)
  {
    m_size = (size > 0 ? size : 1);
  }

  /**
   * Set current text position.
   * @param[out] x
   * @param[out] y
   */
  void get_cursor(uint8_t& x, uint8_t& y)
  {
    x = m_x;
    y = m_y;
  }

  /**
   * Set current text position.
   * @param[in] x
   * @param[in] y
   */
  void set_cursor(uint8_t x, uint8_t y)
  {
    m_x = x;
    m_y = y;
  }

  /**
   * Set pixel with current color.
   * @param[in] x
   * @param[in] y
   */
  void draw_pixel(uint8_t x, uint8_t y)
  {
    set_port(x, y, x + 1, y + 1);
    SPI_transaction(m_cs) {
      spi.exchange(m_color >> 8);
      spi.exchange(m_color);
    }
  }

  /**
   * Draw line with current color.
   * @param[in] x0 
   * @param[in] y0
   * @param[in] x1
   * @param[in] y1
   */
  void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

  /**
   * Draw vertical line with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  void draw_vertical_line(uint8_t x, uint8_t y, uint8_t length);

  /**
   * Draw horizontal line with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  void draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length);

  /**
   * Draw rectangle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  void draw_rect(uint8_t x, uint8_t y, uint8_t width = 1, uint8_t height = 1);

  /**
   * Fill rectangle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  void fill_rect(uint8_t x, uint8_t y, uint8_t width = 1, uint8_t height = 1);
  
  /**
   * Fill screen with current color.
   */
  void fill_screen()
  {
    fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  /**
   * Draw circle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  void draw_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Fill circle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  void fill_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Draw character with current color and font.
   * @param[in] c
   */
  void draw_char(char c);

  /**
   * Draw string in current color and font.
   * @param[in] s
   */
  void draw_string(char* s);

  /**
   * Draw string from program memory with current color and font.
   * @param[in] s
   */
  void draw_string_P(const char* s);

  /**
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end()
  {
    return (spi.end());
  }
};

#endif
