/**
 * @file Cosa/SPI/Driver/ST7735.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/SPI/Driver/ST7735.hh"

const uint8_t ST7735::script[] __PROGMEM = {
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
  // Set Column Address, XSTART = 0, XEND = WIDTH - 1
  CASET, 4, 0x00, 0x00, 0x00, 127,
  // Set Row Address, YSTART = 0, XEND = HEIGHT - 1
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
ST7735::begin()
{
  if (m_initiated) return (false);
  const uint8_t* bp = script;
  uint8_t count;
  uint8_t cmd;
  spi.begin(this);
  while ((cmd = pgm_read_byte(bp++)) != SCRIPTEND) {
    count = pgm_read_byte(bp++);
    if (cmd == SWDELAY) {
      DELAY(count);
    } 
    else {
      asserted(m_dc) {
	spi.transfer(cmd);
      }
      while (count--) spi.transfer(pgm_read_byte(bp++));
    }
  }
  spi.end();
  m_initiated = 1;
  return (true);
}

ST7735::ST7735(Board::DigitalPin cs, Board::DigitalPin dc) :
  Canvas(SCREEN_WIDTH, SCREEN_HEIGHT),
  SPI::Driver(cs, 0, SPI::DEFAULT_CLOCK, 3, SPI::MSB_ORDER, 0),
  m_dc(dc, 1),
  m_initiated(0)
{
}

void 
ST7735::fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  if (x + width >= WIDTH) width = WIDTH - x;
  if (y + height >= HEIGHT) height = HEIGHT - y;
  set_port(x, y, x + width - 1, y + height - 1);
  color16_t color = get_pen_color();
  spi.begin(this);
  for (x = 0; x < width; x++) {
    for (y = 0; y < height; y++) {
      spi.transfer(color.rgb >> 8);
      spi.transfer(color.rgb);
    }
  }
  spi.end();
}

void 
ST7735::draw_vertical_line(uint8_t x, uint8_t y, uint8_t length)
{
  if (x >= WIDTH || length == 0) return;
  if (y >= HEIGHT) {
    int8_t z = y + length;
    if (z >= HEIGHT) return;
    length = z;
    y = 0;
  }
  if (y + length >= HEIGHT) length = HEIGHT - y;
  set_port(x, y, x, y + length);
  color16_t color = get_pen_color();
  spi.begin(this);
  while (length--) {
    spi.transfer(color.rgb >> 8);
    spi.transfer(color.rgb);
  }
  spi.end();
}

void 
ST7735::draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length)
{
  if (y >= HEIGHT || length == 0) return;
  if (x >= WIDTH) {
    int8_t z = x + length;
    if (z >= WIDTH) return;
    length = z;
    x = 0;
  }
  if (x + length >= WIDTH) length = WIDTH - x;
  set_port(x, y, x + length, y);
  color16_t color = get_pen_color();
  spi.begin(this);
  while (length--) {
    spi.transfer(color.rgb >> 8);
    spi.transfer(color.rgb);
  }
  spi.end();
}

uint8_t
ST7735::set_orientation(uint8_t direction) 
{
  uint8_t previous = m_direction;
  uint8_t setting = 0;
  if (direction == LANDSCAPE) {
    m_direction = LANDSCAPE;
    setting = (MADCTL_MX | MADCTL_MV);
    WIDTH  = SCREEN_HEIGHT;
    HEIGHT = SCREEN_WIDTH;
  } 
  else {
    m_direction = PORTRAIT;
    setting = (MADCTL_MX | MADCTL_MY);
    WIDTH  = SCREEN_WIDTH;
    HEIGHT = SCREEN_HEIGHT;
  }
  spi.begin(this);
  write(MADCTL, setting);
  spi.end();
  return (previous);
}
