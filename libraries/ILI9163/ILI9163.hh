/**
 * @file ILI9163.hh
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

#ifndef COSA_ILI9163_HH
#define COSA_ILI9163_HH

#include <GDDRAM.h>

/**
 * Device driver for ILI9163, TFT LCD Single Chip Driver,
 * 128x128 Resolution and max 262K color. The device driver uses
 * 16-bit color. See Canvas and GDDRAM abstract driver.
 *
 * @section Circuit
 * Please note that 3V3 level signals are required. The reset signal
 * is optional.
 *
 * @code
 *                           ILI9163
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (SS/D10)------------3-|CS          |
 * (RST*)--------------4-|RST         |
 * (D9)----------------5-|DC          |
 * (MOSI/D11)----------6-|SDI         |
 * (SCK/D13)-----------7-|SCK         |
 * (VCC)------[330]----8-|LED         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. ILITEK. ILI9163 specification, V0.18.
 *
 * @section Acknowledgements
 * Inspired by graphics library by ladyada/adafruit.
 *
 */
class ILI9163 : public GDDRAM {
public:
  /**
   * Construct ILI9163 canvas object with given control pins.
   * @param[in] cs slave selection pin (default pin 10).
   * @param[in] dc data/command selection pin (default pin 9).
   */
#if defined(BOARD_ATTINYX4)
  ILI9163(Board::DigitalPin cs = Board::D3,
	  Board::DigitalPin dc = Board::D7);
#elif defined(BOARD_ATTINYX5)
  ILI9163(Board::DigitalPin cs = Board::D3,
	  Board::DigitalPin dc = Board::D4);
#else
  ILI9163(Board::DigitalPin cs = Board::D10,
	  Board::DigitalPin dc = Board::D9);
#endif

  /**
   * Screen size (width and height).
   */
  static const uint16_t SCREEN_WIDTH = 128;
  static const uint16_t SCREEN_HEIGHT = 128;

  /**
   * @override{Canvas}
   * Set screen orientation.
   * @param[in] direction.
   */
  virtual uint8_t set_orientation(uint8_t direction);

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
