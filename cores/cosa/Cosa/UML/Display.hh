/**
 * @file Cosa/UML/Display.hh
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

#ifndef COSA_UML_LCD_HH
#define COSA_UML_LCD_HH

#include "Cosa/LCD.hh"
#include "Cosa/UML/Capsule.hh"

namespace UML {

/**
 * Abstract Display Capsule for given Connector type. Will display
 * connector value with given prefix and suffix string on LCD at the
 * given position (X,Y).
 * @param[in] T connector type to display.
 * @param[in] X position on display.
 * @param[in] Y position on display.
 *
 * @section Diagram
 * @code
 *                 Display<T,X,Y>
 *           +----------+-----------------+
 *           | display  | name            |
 * ---[T]--->|          | x,y             |
 *           |          | width,precision |
 *           +----------+-----------------+
 *
 * @endcode
 */
template<typename T, uint8_t X, uint8_t Y>
class Display : public Capsule {
public:
  /**
   * Construct Display for given connector.
   * @param[in] connector.
   * @param[in] dev LCD device.
   * @param[in] prefix string in program memory.
   * @param[in] suffix string in program memory.
   */
  Display(T& connector, LCD::Device* dev, str_P prefix, str_P suffix) :
    Capsule(),
    m_dev(dev),
    m_prefix(prefix),
    m_suffix(suffix),
    m_width(7),
    m_prec(2),
    m_connector(connector)
  {}

  /**
   * Set minimum width for double numbers. The width is signed value,
   * negative for left adjustment.
   */
  void width(int8_t value)
  {
    m_width = value;
  }

  /**
   * Set number of digits after decimal point for double numbers.
   * @param[in] value precision.
   */
  void precision(uint8_t value)
  {
    m_prec = value;
  }

  /**
   * @override UML::Capsule
   * Print connector value at display position with prefix and suffix
   * string. Use width and precision if floating-point.
   */
  virtual void behavior()
  {
    IOStream ios(m_dev);
    ios.width(m_width);
    ios.precision(m_prec);
    m_dev->set_cursor(X, Y);
    ios << m_prefix << m_connector << m_suffix;
  }

protected:
  LCD::Device* m_dev;		//!< Device to display on.
  str_P m_prefix;		//!< Value prefix string (program memory).
  str_P m_suffix;		//!< Value suffix string (program memory).
  uint8_t m_width;		//!< Float width.
  uint8_t m_prec;		//!< Float precision.
  T& m_connector;		//!< Connector to display value of.
};

};
#endif
