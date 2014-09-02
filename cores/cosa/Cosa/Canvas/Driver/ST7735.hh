/**
 * @file Cosa/Canvas/Driver/ST7735.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2014, Mikael Patel
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

#ifndef COSA_CANVAS_DRIVER_ST7735_HH
#define COSA_CANVAS_DRIVER_ST7735_HH

#include "Cosa/Canvas.hh"
#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"

/**
 * Device driver for ST7735, 262K Color Single-Chip TFT Controller.
 * 
 * @section Acknowledgements
 * Inspired by graphics library by ladyada/adafruit.
 *
 * @section Circuit
 * @code
 *                           ST7735
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                      -|            |
 * (RST)---------------6-|RESET       |
 * (D9)----------------7-|A0          |
 * (MOSI/D11)----------8-|SDA         |
 * (SCK/D13)-----------9-|SCK         |
 * (SS/D10)-----------10-|CS          |
 *                      -|            |
 * (VCC)---[330]------15-|LED+        |
 * (GND)--------------16-|LED-        |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Sitronix Technology Corp. ST7735 documentation, V2.1, 2010-02-01.
 */
class ST7735 : public Canvas, SPI::Driver {
public:
  /**
   * Construct display object with given control pins.
   * @param[in] cs slave selection pin (default pin 10).
   * @param[in] dc data/command selection pin (default pin 9).
   */
#if defined(BOARD_ATTINYX4)
  ST7735(Board::DigitalPin cs = Board::D3, 
	 Board::DigitalPin dc = Board::D7);
#elif defined(BOARD_ATTINYX5)
  ST7735(Board::DigitalPin cs = Board::D3, 
	 Board::DigitalPin dc = Board::D4);
#else
  ST7735(Board::DigitalPin cs = Board::D10, 
	 Board::DigitalPin dc = Board::D9);
#endif

  /**
   * @override Canvas
   * Start interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin();

  /**
   * @override Canvas
   * Set screen orientation
   * @param[in] direction.
   */
  virtual uint8_t set_orientation(uint8_t direction);

  /**
   * @override Canvas
   * Set pixel with current color.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void draw_pixel(uint8_t x, uint8_t y);

  /**
   * @override Canvas
   * Draw vertical line with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  virtual void draw_vertical_line(uint8_t x, uint8_t y, uint8_t length);

  /**
   * @override Canvas
   * Draw horizontal line with current color.
   * @param[in] x.
   * @param[in] y.
   * @param[in] length.
   */
  virtual void draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length);

  /**
   * @override Canvas
   * Fill rectangle with current color.
   * @param[in] x.
   * @param[in] y.
   * @param[in] width.
   * @param[in] height.
   */
  virtual void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
  
  /**
   * @override Canvas
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end();

protected:
  /**
   * Data/Command select pin (default is pin 9)
   */
  OutputPin m_dc;

  /**
   * Initialization script (in program memory).
   */
  static const uint8_t script[] PROGMEM;
  uint8_t m_initiated;

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
  } __attribute__((packed));

  /**
   *  Memory Data Access Control (bits)
   */
  enum {
    MADCTL_MY = 0x80,
    MADCTL_MX = 0x40,
    MADCTL_MV = 0x20,
    MADCTL_ML = 0x10,
    MADCTL_BGR = 0x08,
    MADCTL_MH = 0x04
  } __attribute__((packed));

  /**
   * Screen size
   */
  static const uint8_t SCREEN_WIDTH = 128;
  static const uint8_t SCREEN_HEIGHT = 160;

  /**
   * Write 16-bit data to device, MSB first.
   * @param[in] data to write.
   */
  void write(uint16_t data)
    __attribute__((always_inline))
  {
    spi.transfer(data >> 8);
    spi.transfer(data);
  }

  /**
   * Write command to device.
   * @param[in] cmd command to write.
   */
  void write(Command cmd)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
  }

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint8_t data)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer(data);
  }

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint16_t data)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer(data >> 8);
    spi.transfer(data);
  }

  /**
   * Write command and data to device.
   * @param[in] cmd command to write.
   * @param[in] x data to write.
   * @param[in] y data to write.
   */
  void write(Command cmd, uint8_t x, uint8_t y)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer(0);
    spi.transfer(x);
    spi.transfer(0);
    spi.transfer(y);
  }
};

#endif

