/**
 * @file ILI9341.cpp
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

#include "ILI9341.hh"

const uint8_t ILI9341::s_script[] __PROGMEM = {
  // Software Reset
  SWRESET, 0,
  // Software Delay
  SWDELAY, 250,
  // Power Control A
  // Vcore = 1.6 V, DDVDH = 5.6 V
  PWCTRLA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  // Power Control B
  // PCEQ enable
  PWCTRLB, 3, 0x00, 0xC1, 0x30,
  // Driver Timing Control A
  // Internal clock -1, EQ -1, CR -1, Pre-Charge -2
  DTCTRLA, 3, 0x85, 0x00, 0x78,
  // Driver Timing Control B
  // External clock 0, EQE -1, CRE -1, Pre-Charge -2
  DTCTRLB, 2, 0x00, 0x00,
  // Power On Sequence Control
  // Soft start keep 1 frame
  // 1st frame enable
  // DDVHDH enhanced mode enable
  PWONCTRL, 4, 0x64, 0x03, 0X12, 0X81,
  // Pump Ratio Control
  // DDVDH = 2xVCI
  PRCTRL, 1, 0x20,
  // Power Control 1
  // GVDD = 4.60 V
  PWCTRL1, 1, 0x23,
  // Power Control 2
  PWCTRL2, 1, 0x10,
  // VCOM Control 1
  // VCOMH = 4.250 V
  // VCOML = -1.5 V
  VMCTRL1, 2, 0x3e, 0x28,
  // VCOM Control 2
  // VCOMH = VMH - 58
  // VCOML = VMH - 58
  VMCTRL2, 1, 0x86,
  // Memory Data Access Control
  // Column Address Order (MX), BGR Order
  MADCTL, 1, 0x48,
  // Pixel Format Set
  // RGB 16-bits, MCU 16-bits
  PIXSET, 1, 0x55,
  // Frame Rate Control
  // Division Ratio = fosc / 1
  // Frame Rate = 79 Hz
  FRMCTR1, 2, 0x00, 0x18,
  // Display Function Control
  // Interval scan, V63, V0, VCOML, VCOMH
  DISCTRL, 3, 0x08, 0x82, 0x27,
  // Disable 3-Gamma
  EN3GAM, 1, 0x00,
  // Gamma Set
  // Gamma curve 1
  GAMSET, 1, 0x01,
  // Positive Gamma Correction
  PGAMCTRL, 15,
  0x0F, 0x31, 0x2B, 0x0C, 0x0E,
  0x08, 0x4E, 0xF1, 0x37, 0x07,
  0x10, 0x03, 0x0E, 0x09, 0x00,
  // Negative Gamma Correction
  NGAMCTRL, 15,
  0x00, 0x0E, 0x14, 0x03, 0x11,
  0x07, 0x31, 0xC1, 0x48, 0x08,
  0x0F, 0x0C, 0x31, 0x36, 0x0F,
  // Exit Sleep Mode
  SLPOUT, 0,
  // Software Delay
  SWDELAY, 120,
  // Display On
  DISPON, 0,
  // END OF SCRIPT
  SCRIPTEND
};

ILI9341::ILI9341(Board::DigitalPin cs, Board::DigitalPin dc) :
  GDDRAM(SCREEN_WIDTH, SCREEN_HEIGHT, cs, dc)
{
}
