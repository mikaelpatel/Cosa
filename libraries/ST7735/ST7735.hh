/**
 * @file ST7735.hh
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

#ifndef COSA_ST7735_HH
#define COSA_ST7735_HH

#include <GDDRAM.h>

/**
 * Device driver for ST7735, 262K Color Single-Chip TFT Controller.
 *
 * @section Circuit
 * @code
 *                           ST7735
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                      -|            |
 * (RST*)--------------6-|RESET       |
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
 *
 * @section Acknowledgements
 * Inspired by graphics library by ladyada/adafruit.
 */
class ST7735 : public GDDRAM {
public:
  /**
   * Construct canvas object with given control pins.
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
   * @override{Canvas}
   * Set screen orientation.
   * @param[in] direction.
   */
  virtual uint8_t set_orientation(uint8_t direction);

  /**
   * Screen size (width and height).
   */
  static const uint16_t SCREEN_WIDTH = 128;
  static const uint16_t SCREEN_HEIGHT = 160;

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

