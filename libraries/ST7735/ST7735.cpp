/**
 * @file ST7735.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#include "ST7735.hh"

const uint8_t ST7735::s_script[] __PROGMEM = {
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
  // Power Control 1
  // -4.6V, auto mode
  PWCTRL1, 3, 0xA2, 0x02, 0x84,
  // Power Control 2
  // VGH25 = 2.4C, VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
  PWCTRL2, 1, 0xC5,
  // Power Control 3
  // Opamp current small, Boost frequency
  PWCTRL3, 2, 0x0A, 0x00,
  // Power Control 4
  // BCLK/2, Opamp current small & medium low
  PWCTRL4, 2, 0x8A, 0x2A,
  // Power Control 5
  PWCTRL5, 2, 0x8A, 0xEE,
  // EEPROM Control 1
  VMCTRL1, 1, 0x0E,
  // Invert Display Off
  DINVOFF, 0,
  // Memory Access Control
  // row address/col address, bottom to top refresh
  MADCTL, 1, 0xC0,
  // Set Color Mode, 16-bit color
  COLMOD, 1, 0x05,
  // Set Column Address
  // XSTART = 0, XEND = WIDTH - 1
  CASET, 4, 0x00, 0x00, 0x00, SCREEN_WIDTH - 1,
  // Set Row Address
  // YSTART = 0, XEND = HEIGHT - 1
  RASET, 4, 0x00, 0x00, 0x00, SCREEN_HEIGHT - 1,
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

ST7735::ST7735(Board::DigitalPin cs, Board::DigitalPin dc) :
  GDDRAM(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc)
{
}

uint8_t
ST7735::set_orientation(uint8_t direction)
{
  uint8_t previous = m_direction;
  uint8_t setting = 0;
  m_direction = direction;
  uint16_t width = WIDTH;
  WIDTH  = HEIGHT;
  HEIGHT = width;
  if (direction == LANDSCAPE) {
    setting = (MADCTL_MX | MADCTL_MV);
  }
  else {
    setting = (MADCTL_MX | MADCTL_MY);
  }
  spi.acquire(this);
    spi.begin();
      write(MADCTL, setting);
    spi.end();
  spi.release();
  return (previous);
}
