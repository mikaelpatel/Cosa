/**
 * @file Cosa/SPI/ST7735R.cpp
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

#include "Cosa/SPI/ST7735R.hh"

const uint8_t ST7735R::script[] PROGMEM = {
  // Software Reset
  SWRESET, 0, 
  // Software Delay 
  SWDELAY, 150,
  // Out of Sleep Mode
  SLPOUT, 0, 
  // Software Delay 
  SWDELAY, 250,
  // Software Delay 
  SWDELAY, 250,
  // Frame Rate Control 1, normal mode
  FRMCTR1, 3, 0x01, 0x2C, 0x2D, 
  // Frame Rate Control 2, idle mode
  FRMCTR2, 3, 0x01, 0x2C, 0x2D, 
  // Frame Rate Control 2, idle mode
  FRMCTR3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
  // Display Inversion Control  
  INVCTR, 1, 0x07,
  // Power Control 1, -4.6V, auto mode
  PWCTR1, 3, 0xA2, 0x02, 0x84,
  // Power Control 2, VGH25 = 2.4C, VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
  PWCTR2, 1, 0xC5,
  // Power Control 3, Opamp current small, Boost frequency
  PWCTR3, 2, 0x0A, 0x00,
  // Power Control 4, BCLK/2, Opamp current small & medium low
  PWCTR4, 2, 0x8A, 0x2A,
  // Power Control 5
  PWCTR5, 2, 0x8A, 0xEE,
  // EEPROM Control 1
  VMCTR1, 1, 0x0E,
  // Invert Display Off
  INVOFF, 0,
  // Memory Access Control, row address/col address, bottom to top refresh
  MADCTL, 1, 0xC0,
  // Set Color Mode, 16-bit color
  COLMOD, 1, 0x05,
  // Set Column Address, XSTART = 0, XEND = SCREEN_WIDTH - 1
  CASET, 4, 0x00, 0x00, 0x00, 127,
  // Set Row Address, YSTART = 0, XEND = SCREEN_HEIGHT - 1
  RASET, 4, 0x00, 0x00, 0x00, 159,
  // Positive Gamma Correction
  GMCTRP1, 16, 
  0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
  0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
  // Negative Gamma Correction
  GMCTRN1, 16, 
  0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
  0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
  // Normal Display On
  NORON, 0,
  // Software Delay 
  SWDELAY, 10,
  // Display On
  DISPON, 0,
  // Software Delay 
  SWDELAY, 120,
  // END OF SCRIPT
  SCRIPTEND
};

bool 
ST7735R::begin()
{
  bool res = spi.begin(SPI::DEFAULT_CLOCK, 3, SPI::MSB_FIRST);
  if (!res || m_initiated) return (res);
  const uint8_t* bp = script;
  uint8_t count;
  uint8_t cmd;
  SPI_transaction(m_cs) {
    while ((cmd = pgm_read_byte(bp++)) != SCRIPTEND) {
      count = pgm_read_byte(bp++);
      if (cmd == SWDELAY) {
	DELAY(count);
      } 
      else {
	m_dc.clear();
	spi.exchange(cmd);
	m_dc.set();
	while (count--) spi.exchange(pgm_read_byte(bp++));
      }
    }
  }
  m_initiated = 1;
  return (res);
}

ST7735R::ST7735R(uint8_t cs, uint8_t dc) :
  Canvas(128, 160),
  m_cs(cs, 1),
  m_dc(dc, 1),
  m_initiated(0)
{
}

void 
ST7735R::write(Command cmd)
{
  SPI_transaction(m_cs) {
    m_dc.clear();
    spi.exchange(cmd);
    m_dc.set();
  }
}

void 
ST7735R::write(Command cmd, uint8_t data)
{
  SPI_transaction(m_cs) {
    m_dc.clear();
    spi.exchange(cmd);
    m_dc.set();
    spi.exchange(data);
  }
}

void 
ST7735R::write(Command cmd, uint16_t data)
{
  SPI_transaction(m_cs) {
    m_dc.clear();
    spi.exchange(cmd);
    m_dc.set();
    spi.exchange(data >> 8);
    spi.exchange(data);
  }
}

void 
ST7735R::write(Command cmd, uint16_t x, uint16_t y)
{
  SPI_transaction(m_cs) {
    m_dc.clear();
    spi.exchange(cmd);
    m_dc.set();
    spi.exchange(x >> 8);
    spi.exchange(x);
    spi.exchange(y >> 8);
    spi.exchange(y);
  }
}

void 
ST7735R::fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  set_port(x, y, x + width - 1, y + height - 1);
  SPI_transaction(m_cs) {
    for (x = 0; x < width; x++)
      for (y = 0; y < height; y++) {
	spi.exchange(m_pen_color >> 8);
	spi.exchange(m_pen_color);
      }
  }
}

void 
ST7735R::draw_vertical_line(uint8_t x, uint8_t y, uint8_t length)
{
  if (x >= SCREEN_WIDTH) return;
  if (y + length >= SCREEN_HEIGHT) length = SCREEN_HEIGHT - y - 1;
  set_port(x, y, x, y + length);
  SPI_transaction(m_cs) {
    while (length--) {
      spi.exchange(m_pen_color >> 8);
      spi.exchange(m_pen_color);
    }
  }
}

void 
ST7735R::draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length)
{
  if (y >= SCREEN_HEIGHT) return;
  if (x + length >= SCREEN_WIDTH) length = SCREEN_WIDTH - x - 1;
  set_port(x, y, x + length, y + 1);
  SPI_transaction(m_cs) {
    while (length--) {
      spi.exchange(m_pen_color >> 8);
      spi.exchange(m_pen_color);
    }
  }
}

