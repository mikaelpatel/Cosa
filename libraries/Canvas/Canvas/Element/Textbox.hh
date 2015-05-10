/**
 * @file Textbox.hh
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

#ifndef COSA_TEXTBOX_HH
#define COSA_TEXTBOX_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

#include <Canvas.h>
#include "Font.hh"

extern System5x7 system5x7;

/**
 * Canvas Textbox element. Acts as an IOStream/console output to a
 * canvas. As an element it holds its own canvas state; context. The
 * textbox is defined by a port (x, y, width, height) on the canvas.
 * Basic special character handling of carriage-return, line- and
 * form-feed.
 */
class Textbox : public Canvas::Element, public IOStream::Device {
public:
  /**
   * Construct text box on given canvas. Set textbox port to canvas size.
   * @param[in] canvas.
   */
  Textbox(Canvas* canvas, Font* font = (Font*) &system5x7) :
    Canvas::Element(canvas, font),
    IOStream::Device()
  {
    set_text_port(0, 0, canvas->WIDTH, canvas->HEIGHT);
  }

  /**
   * Get current text port.
   * @param[out] x.
   * @param[out] y.
   * @param[out] width.
   * @param[out] height.
   */
  void get_text_port(uint16_t& x, uint16_t& y, uint16_t& width, uint16_t& height)
  {
    x = m_text_port.x;
    y = m_text_port.y;
    width = m_text_port.width;
    height = m_text_port.height;
  }

  /**
   * Set current text position.
   * @param[in] x.
   * @param[in] y.
   * @param[in] width.
   * @param[in] height.
   */
  void set_text_port(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
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
    return (m_canvas->get_context()->get_text_font()->LINE_SPACING);
  }

  /**
   * Set current line spacing.
   * @param[in] spacing.
   */
  void set_line_spacing(uint8_t spacing)
  {
    m_canvas->get_context()->get_text_font()->LINE_SPACING = spacing;
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
  /** Textbox port rectangle. */
  Canvas::rect16_t m_text_port;
};

#endif
