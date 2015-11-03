/**
 * @file ILI9163.cpp
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

#include "ILI9163.hh"

const uint8_t ILI9163::s_script[] __PROGMEM = {
  // Software Reset
  SWRESET, 0,
  // Out of Sleep Mode
  SLPOUT, 0,
  SWDELAY, 120,
  // Set Color Mode, 16-bit color
  COLMOD, 1, 0x05,
  // Gamma set
  GAMSET, 1, 0x04,
  // Enable Gamma adjustment
  EN3GAM, 1, 0x01,
  // Display Normal Mode
  NORON, 0,
  // Display functions
  DISCTRL, 2, 0xff, 0x06,
  // Positive Gamma Correction
  GMCTRP1, 15,
  0x36, 0x29, 0x12, 0x22, 0x1C,
  0x15, 0x42, 0xB7, 0x2F, 0x13,
  0x12, 0x0A, 0x11, 0x0B, 0x06,
  // Negative Gamma Correction
  GMCTRN1, 15,
  0x09, 0x16, 0x2D, 0x0D, 0x13,
  0x15, 0x40, 0x48, 0x53, 0x0C,
  0x1D, 0x25, 0x2E, 0x34, 0x39,
  // Frame Rate Control 1, normal mode
  FRMCTR1, 2, 0x08, 0x02,
  // Display Inversion Control
  INVCTR, 1, 0x07,
  // Power Control 1
  PWCTRL1, 2, 0x0A, 0x02,
  // Power Control 2
  PWCTRL2, 1, 0x02,
  // VCOM Control 1
  VMCTRL1, 2, 0x50, 0x63,
  // VCOM Control 2
  VMCTRL2, 1, 0,
  // Set Column Address
  // XSTART = 0, XEND = WIDTH - 1
  CASET, 4, 0x00, 0x00, 0x00, SCREEN_WIDTH - 1,
  // Set Row Address
  // YSTART = 0, XEND = HEIGHT - 1
  RASET, 4, 0x00, 0x00, 0x00, SCREEN_HEIGHT - 1,
  // Memory Access Control
  // row address/col address, bottom to top refresh
  MADCTL, 1, MADCTL_MX | MADCTL_MY | MADCTL_BGR,
  // Display On
  DISPON, 0,
  // Software Delay
  SWDELAY, 1,
  // END OF SCRIPT
  SCRIPTEND
};

ILI9163::ILI9163(Board::DigitalPin cs, Board::DigitalPin dc) :
  GDDRAM(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc)
{
}

uint8_t
ILI9163::set_orientation(uint8_t direction)
{
  uint8_t previous = m_direction;
  uint8_t setting = 0;
  m_direction = direction;
  uint16_t width = WIDTH;
  WIDTH  = HEIGHT;
  HEIGHT = width;
  if (direction == LANDSCAPE) {
    setting = MADCTL_MY | MADCTL_MV | MADCTL_BGR;
  }
  else {
    setting = MADCTL_MX | MADCTL_MY | MADCTL_BGR;
  }
  spi.acquire(this);
    spi.begin();
      write(MADCTL, setting);
    spi.end();
  spi.release();
  return (previous);
}
