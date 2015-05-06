/**
 * @file ST7920.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef ST7920_HH
#define ST7920_HH

#include "Cosa/LCD.hh"
#include <HD44780.h>

/**
 * ST7920 LCD controller/driver. Binding to trace, etc. Supports simple text
 * scroll, cursor, and handling of special characters such as carriage-
 * return, form-feed, back-space, horizontal tab and new-line.
 *
 * @section Circuit
 * Circuit when using HD44780::Port4b and ignoring back-light control (BT).
 *
 * @code
 *                           ST7920
 *                       +------------+
 * (GND)---------------1-|VSS         |
 * (VCC)---------------2-|VDD         |
 *                     3-|VO          |
 * (D8)----------------4-|RS          |
 * (GND)---------------5-|RW          |
 * (D9)----------------6-|EN          |
 *                     7-|D0          |
 *                     8-|D1          |
 *                     9-|D2          |
 *                    10-|D3          |
 * (D4/D0)------------11-|D4          |
 * (D5/D1)------------12-|D5          |
 * (D6/D2)------------13-|D6          |
 * (D7/D3)------------14-|D7          |
 * (VCC)--------------15-|PSB         |
 *                    16-|NC          |
 *                    17-|RST         |
 *                    18-|VOUT        |
 * (VCC)--------------19-|A           |
 * (GND)---[330]------20-|K           |
 *                       +------------+
 *
 * @section References
 * 1. Product Specification, Sintronix, ST7920, Chinese Fonts built in
 * LCD controller/driver, V4.0, 2008/08/18.
 */
class ST7920 : public HD44780 {
public:
  /**
   * Construct ST7920 LCD connected to given io port handler. The
   * display is initiated when calling begin().
   * @param[in] io handler.
   */
  ST7920(IO* io) : HD44780(io, 16, 4)
  {
    m_offset = offset2;
  }

private:
  /** Row offset tables for display dimensions (16X4). */
  static const uint8_t offset2[] PROGMEM;
};
#endif
