/**
 * @file Cosa/Canvas/Element/Textbox.hh
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

#ifndef __COSA_CANVAS_ELEMENT_TEXTBOX_HH__
#define __COSA_CANVAS_ELEMENT_TEXTBOX_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Font.hh"

/**
 * Canvas Textbox element. Acts as an IOStream/console output to
 * Canvas. As an element it holds its own canvas state; context. The
 * textbox is defined by a port (x, y, width, height) on the
 * canvas. Basic special character handling of carriage-return, line-
 * and form-feed. 
 */
class Textbox : public Canvas::Element, public IOStream::Device {
public:
  /**
   * Construct text box on given canvas. Set textbox port to canvas size.
   * @param[in] canvas.
   */
  Textbox(Canvas* canvas) :
    Canvas::Element(canvas),
    IOStream::Device(),
    m_line_spacing(2)
  {
    set_text_port(0, 0, canvas->WIDTH, canvas->HEIGHT);
  }

  /**
   * Get current text port.
   * @param[out] x
   * @param[out] y
   * @param[out] width
   * @param[out] height
   */
  void get_text_port(uint8_t& x, uint8_t& y, uint8_t& width, uint8_t& height)
  {
    x = m_text_port.x;
    y = m_text_port.y;
    width = m_text_port.width;
    height = m_text_port.height;
  }

  /**
   * Set current text position.
   * @param[in] x
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  void set_text_port(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
  {
    m_text_port.x = x;
    m_text_port.y = y;
    m_text_port.width = width;
    m_text_port.height = height;
    set_cursor(x, y);
  }

  /**
   * Get current line spacing.
   * @return line spacing.
   */
  uint8_t get_line_spacing()
  {
    return (m_line_spacing);
  }

  /**
   * Set current line spacing.
   * @param[in] spacing.
   */
  void set_line_spacing(uint8_t spacing)
  {
    m_line_spacing = spacing;
  }

  /**
   * @override IOStream::Device
   * Write character at current cursor position, with current text
   * color, scale and font. The textbox will handle carriage-return,
   * line-feed and form-feed. Scrolling is handled as a wrap-around.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

protected:
  /**
   * Textbox port and line spacing. Character spacing is defined
   * by the font setting.
   */
  Canvas::rect8_t m_text_port;
  uint8_t m_line_spacing;
};

#endif
