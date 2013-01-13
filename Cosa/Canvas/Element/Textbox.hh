/**
 * @file Cosa/Canvas/Element/Textbox.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Canvas Textbox element. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_ELEMENT_TEXTBOX_HH__
#define __COSA_CANVAS_ELEMENT_TEXTBOX_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Canvas.hh"
#include "Cosa/Canvas/Font.hh"

class Textbox : private Canvas::Element, public IOStream::Device {
protected:
  /**
   * Text handling; font, color, scale, caret and port.
   */
  Font* m_font;
  uint16_t m_text_color;
  uint8_t m_text_scale;
  uint16_t m_canvas_color;
  struct {
    uint8_t x;
    uint8_t y;
  } m_caret;
  struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
  } m_text_port;

public:
  /**
   * Character/line spacing
   */
  uint8_t CHAR_SPACING;
  uint8_t LINE_SPACING;

  /**
   * Construct canvas object and initiate.
   * @param[in] x.
   * @param[in] y.
   * @param[in] width.
   * @param[in] height.
   * @param[in] font.
   */
  Textbox(Canvas* canvas, Font* font = (Font*) &system5x7) :
    Canvas::Element(canvas),
    IOStream::Device(),
    m_font(font),
    m_text_color(Canvas::BLACK),
    m_text_scale(1),
    m_canvas_color(Canvas::WHITE),
    CHAR_SPACING(1),
    LINE_SPACING(2)
  {
    set_text_port(0, 0, canvas->WIDTH, canvas->HEIGHT);
  }

  /**
   * Get current canvas color.
   * @return color.
   */
  uint16_t get_canvas_color()
  {
    return (m_canvas_color);
  }

  /**
   * Set current canvas color.
   * @param[in] color
   */
  void set_canvas_color(uint16_t color)
  {
    m_canvas_color = color;
  }

  /**
   * Get current text color.
   * @return color.
   */
  uint16_t get_text_color()
  {
    return (m_text_color);
  }

  /**
   * Set current text color.
   * @param[in] color
   */
  void set_text_color(uint16_t color)
  {
    m_text_color = color;
  }

  /**
   * Get current text font.
   */
  Font* get_text_font()
  {
    return (m_font);
  }

  /**
   * Set current text font.
   * @param[in] font
   */
  void set_text_font(Font* font)
  {
    m_font = font;
  }

  /**
   * Get current text scale.
   * @return text scale.
   */
  uint8_t get_text_scale()
  {
    return (m_text_scale);
  }

  /**
   * Set current text scale (1..n).
   * @param[in] scale.
   */
  void set_text_scale(uint8_t scale)
  {
    m_text_scale = (scale > 0 ? scale : 1);
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
    set_caret(x, y);
  }

  /**
   * Get current caret position.
   * @param[out] x
   * @param[out] y
   */
  void get_caret(uint8_t& x, uint8_t& y)
  {
    x = m_caret.x;
    y = m_caret.y;
  }

  /**
   * Set current caret position.
   * @param[in] x
   * @param[in] y
   */
  void set_caret(uint8_t x, uint8_t y)
  {
    m_caret.x = x;
    m_caret.y = y;
  }

  /**
   * @override
   * Write character at current cursor position, with current text
   * color, scale and font.  
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);
};

#endif
