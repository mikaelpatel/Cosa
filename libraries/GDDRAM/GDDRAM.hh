/**
 * @file GDDRAM.hh
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

#ifndef COSA_GDDRAM_HH
#define COSA_GDDRAM_HH

#include <Canvas.h>
#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"

/**
 * Abstract device driver for Graphical Display Data RAM Devices.
 */
class GDDRAM : public Canvas, protected SPI::Driver {
public:
  /**
   * Construct GDDRAM canvas object with given control pins.
   * @param[in] cs slave selection pin.
   * @param[in] dc data/command selection pin.
   * @param[in] width screen.
   * @param[in] height screen.
   */
  GDDRAM(uint16_t width,
	 uint16_t height,
	 Board::DigitalPin cs,
	 Board::DigitalPin dc);

  /**
   * @override{Canvas}
   * Start interaction with device.
   * @return true(1) if successful otherwise false(0).
   */
  virtual bool begin();

  /**
   * @override{Canvas}
   * Set screen orientation.
   * @param[in] direction.
   */
  virtual uint8_t set_orientation(uint8_t direction);

  /**
   * @override{Canvas}
   * Set pixel with current color.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void draw_pixel(uint16_t x, uint16_t y);

  /**
   * @override{Canvas}
   * Draw image on canvas at given position.
   * @param[in] x.
   * @param[in] y.
   * @param[in] image.
   */
  virtual void draw_image(uint16_t x, uint16_t y, Image* image);

  /**
   * @override{Canvas}
   * Draw vertical line with current color.
   * @param[in] x
   * @param[in] y
   * @param[in] length.
   */
  virtual void draw_vertical_line(uint16_t x, uint16_t y, uint16_t length);

  /**
   * @override{Canvas}
   * Draw horizontal line with current color.
   * @param[in] x.
   * @param[in] y.
   * @param[in] length.
   */
  virtual void draw_horizontal_line(uint16_t x, uint16_t y, uint16_t length);

  /**
   * @override{Canvas}
   * Fill rectangle with current color.
   * @param[in] x.
   * @param[in] y.
   * @param[in] width.
   * @param[in] height.
   */
  virtual void fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

  /**
   * @override{Canvas}
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0).
   */
  virtual bool end();

protected:
  OutputPin m_dc;		//!< Data/Command select pin.
  bool m_initiated;		//!< Initialization state.

  /**
   * @override{GDDRAM}
   * Get initialization script (in program memory).
   * @return pointer to script.
   */
  virtual const uint8_t* script() = 0;

  /**
   * GDDRAM SPI commands.
   */
  enum Command {
    NOP = 0x0,			// No Operation
    SWRESET = 0x01,		// Software Reset
    SWDELAY = 0x02,		// Software Delay
    SCRIPTEND = 0x03,		// Script End
    RDDIDIF = 0x04,		// Read Display Identification Information
    RDDST = 0x09,		// Read Display Status
    RDDPM = 0x0A,		// Read Display Power Mode
    RDDMADCTL = 0x0B,		// Read Display MADCTL
    RDDCOLMOD = 0x0C,		// Read Display Pixel Format
    RDDIM = 0x0D,		// Read Display Image Mode
    RDDSM = 0x0E,		// Read Display Signal Mode
    RDDSDR = 0x0F,		// Read Display Self-Diagnostic Result
    SLPIN = 0x10,		// Enter Sleep Mode
    SLPOUT = 0x11,		// Exit Sleep Mode
    PTLON = 0x12,		// Partial Mode On
    NORON = 0x13,		// Normal Display Mode On
    DINVOFF = 0x20,		// Display Inversion Off
    DINVON = 0x21,		// Display Inversion On
    GAMSET = 0x26,		// Gamma Set
    DISPOFF = 0x28,		// Display Off
    DISPON = 0x29,		// Display On
    CASET = 0x2A,		// Column Address Set
    PASET = 0x2B,		// Page Address Set
    RASET = 0x2B,		// Page Address Set
    RAMWR = 0x2C,		// Memory Write
    RGBSET = 0x2D,		// Color Setting for 4K, 65K and 262K
    RAMRD = 0x2E,		// Memory Read
    PTLAR = 0x30,		// Partial Area
    VSCRDEF = 0x33,		// Vertical Scrolling Definition
    TEOFF = 0x34,		// Tearing Effect Line Off
    TEON = 0x35,		// Tearing Effect Line On
    MADCTL = 0x36,		// Memory Data Access Control
    VSCRSADD = 0x37,		// Vertical Scrolling Start Address
    IDMOFF = 0x38,		// Idle Mode Off
    IDMON = 0x39,		// Idle Mode On
    PIXSET = 0x3A,		// Pixel Format Set
    COLMOD = 0x3A,		// Set Color Mode
    RAMWRC = 0x3C,		// Write Memory Continue
    RAMRDC = 0x3E,		// Read Memory Continue
    SETSCANLINE = 0x44,		// Set Tear Scanline
    GETSCANLINE = 0x45,		// Get Scanline
    WRDISBV = 0x51,		// Write Display Brightness Value
    RDDISBV = 0x52,		// Read Display Brightness Value
    WRCTRLD = 0x53,		// Write CTRL Display
    RDCTRLD = 0x54,		// Read CTRL Display
    WRCABC = 0x55,		// Write Content Adaptive Brightness Control
    RDCABC = 0x56,		// Read Content Adaptive Brightness Control
    WRCABCMIN = 0x5E,		// Write CABC Minimum Brightness
    RDCABCMIN = 0x5F,		// Read CABC Minimum Brightness
    RDID1 = 0xDA,		// Read ID1 Value
    RDID2 = 0xDB,		// Read ID2 Value
    RDID3 = 0xDC,		// Read ID3 Value
    IFMODE = 0xB0,		// Interface Mode Control
    FRMCTR1 = 0xB1,		// Frame Rate Control, normal mode
    FRMCTR2 = 0xB2,		// Frame Rate Control, idle mode
    FRMCTR3 = 0xB3,		// Frame Rate Control, partial mode
    INVCTR = 0xB4,		// Display Inversion Control
    PRCTR = 0xB5,		// Blanking Porch Control
    DISCTRL = 0xB6,		// Display Function Control
    ETMOD = 0xB7,		// Entry Mode Set
    BLCTRL1 = 0xB8,		// Backlight Control 1
    BLCTRL2 = 0xB9,		// Backlight Control 2
    BLCTRL3 = 0xBA,		// Backlight Control 3
    BLCTRL4 = 0xBB,		// Backlight Control 4
    BLCTRL5 = 0xBC,		// Backlight Control 5
    BLCTRL6 = 0xBD,		// Backlight Control 6
    BLCTRL7 = 0xBE,		// Backlight Control 7
    BLCTRL8 = 0xBF,		// Backlight Control 8
    PWCTRL1 = 0xC0,		// Power Control 1
    PWCTRL2 = 0xC1,		// Power Control 2
    PWCTRL3 = 0xC2,		// Power Control 3, normal mode
    PWCTRL4 = 0xC3,		// Power Control 4, idle mode
    PWCTRL5 = 0xC4,		// Power Control 5, partial mode
    PWCTRL6 = 0xFC,		// Power Control 6, partial mode
    VMCTRL1 = 0xC5,		// VCOM Control 1
    VMCTRL2 = 0xC7,		// VCOM Control 2
    NVMWR = 0xD0,		// NV Memory Write
    NVMPKEY = 0xD1,		// NV Memory Protection Key
    RDNVM = 0xD2,		// NV Memory Status Read
    RDID4 = 0xD3,		// Read ID4
    PGAMCTRL = 0xE0,		// Positive Gamma Correction
    GMCTRP1 = 0xE0,		// Positive Gamma Correction
    NGAMCTRL = 0xE1,		// Negative Gamma Correction
    GMCTRN1 = 0xE1,		// Negative Gamma Correction
    DGAMCTRL1 = 0xE2,		// Digital Gamma Control 1
    DGAMCTRL2 = 0xE3,		// Digital Gamma Control 2
    IFCTRL = 0xF6,		// Interface Control
    PWCTRLA = 0xCB,		// Power Control A
    PWCTRLB = 0xCF,		// Power Control B
    DTCTRLA = 0xE8,		// Driver Timing Control A
    DTCTRLB = 0xEA,		// Driver Timing Control B
    PWONCTRL = 0xED,		// Power On Sequence Control
    EXTCTRL = 0xF0,		// Extension Command Control
    EN3GAM = 0xF2,		// Enable/Disable 3-Gamma
    PRCTRL = 0xF7,		// Pump Ratio Control
    VCOM4L = 0xFF		// VCOM 4 Level Control
  } __attribute__((packed));

  /**
   *  Memory Data Access Control (bits).
   */
  enum {
    MADCTL_MH = 0x04,		// Horizontal Refresh order
    MADCTL_ML = 0x10,		// Vertical Refresh order
    MADCTL_MV = 0x20,		// Row / Column Exchange
    MADCTL_MX = 0x40,		// Column Address order
    MADCTL_MY = 0x80,		// Row Address order
    MADCTL_BGR = 0x08,		// BGR order
    MADCTL_RGB = 0x00		// RGB order
  } __attribute__((packed));

  /**
   * Write 16-bit data to device, MSB first.
   * @param[in] data to write.
   */
  void write(uint16_t data)
    __attribute__((always_inline))
  {
    spi.transfer_start(data >> 8);
    spi.transfer_next(data);
    spi.transfer_await();
  }

  /**
   * Write given number of 16-bit data to device, MSB first.
   * @param[in] data to write.
   * @param[in] count number of data elements to write.
   * @pre (count > 0)
   */
  void write(uint16_t data, uint16_t count)
    __attribute__((always_inline))
  {
    uint8_t high = data >> 8;
    uint8_t low = data;
    spi.transfer_start(high);
    while (--count) {
      spi.transfer_next(low);
      spi.transfer_next(high);
    };
    spi.transfer_next(low);
    spi.transfer_await();
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
   * Write command and 8-bit data to device.
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
   * Write command and 16-bit data to device.
   * @param[in] cmd command to write.
   * @param[in] data to write.
   */
  void write(Command cmd, uint16_t data)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer_start(data >> 8);
    spi.transfer_next(data);
    spi.transfer_await();
  }

  /**
   * Write command and 2x16-bit data to device.
   * @param[in] cmd command to write.
   * @param[in] x data to write.
   * @param[in] y data to write.
   */
  void write(Command cmd, uint16_t x, uint16_t y)
    __attribute__((always_inline))
  {
    asserted(m_dc) {
      spi.transfer(cmd);
    }
    spi.transfer_start(x >> 8);
    spi.transfer_next(x);
    spi.transfer_next(y >> 8);
    spi.transfer_next(y);
    spi.transfer_await();
  }
};

#endif

