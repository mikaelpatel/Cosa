/**
 * @file ILI9341.hh
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

#ifndef COSA_ILI9341_HH
#define COSA_ILI9341_HH

#include <GDDRAM.h>

/**
 * Device driver for ILI9341, TFT LCD Single Chip Driver,
 * 240x320 Resolution and max 262K color. The device driver uses
 * 16-bit color. See Canvas and GDDRAM abstract driver.
 *
 * @section Circuit
 * Please note that 3V3 level signals are required. The reset signal
 * is optional.
 *
 * @code
 *                           ILI9341
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST*)--------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 * (MISO/D12)----------9-|SDO         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. ILITEK. ILI9341 specification, V1.13, 2011-07-20.
 *
 * @section Acknowledgements
 * Inspired by graphics library by ladyada/adafruit.
 *
 */
class ILI9341 : public GDDRAM {
public:
  /**
   * Construct ILI9341 canvas object with given control pins.
   * @param[in] cs slave selection pin (default pin 10).
   * @param[in] dc data/command selection pin (default pin 9).
   */
#if defined(BOARD_ATTINYX4)
  ILI9341(Board::DigitalPin cs = Board::D3,
	  Board::DigitalPin dc = Board::D7);
#elif defined(BOARD_ATTINYX5)
  ILI9341(Board::DigitalPin cs = Board::D3,
	  Board::DigitalPin dc = Board::D4);
#else
  ILI9341(Board::DigitalPin cs = Board::D10,
	  Board::DigitalPin dc = Board::D9);
#endif

  /**
   * Screen size (width and height).
   */
  static const uint16_t SCREEN_WIDTH = 240;
  static const uint16_t SCREEN_HEIGHT = 320;

protected:
  /**
   * @override{GDDRAM}
   * Get initialization script (in program memory).
   * @return pointer to script.
   */
  virtual const uint8_t* script()
  {
    return (s_script);
  }

  /**
   * Initialization script (in program memory).
   */
  static const uint8_t s_script[] PROGMEM;
};
#endif
