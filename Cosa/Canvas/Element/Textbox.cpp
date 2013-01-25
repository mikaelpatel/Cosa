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
 * Canvas Textbox element. Acts as an IOStream/console output to
 * Canvas. As an element it holds its own canvas state; context.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas/Element/Textbox.hh"

int 
Textbox::putchar(char c)
{ 
  // Save the current context and setup our context
  Canvas::Context* saved = m_canvas->set_context(this);

  // Handle some special characters, new-line
  uint8_t x, y;
  get_cursor(x, y);
  uint8_t scale = m_text_scale;
  uint8_t width = (x + (scale * m_font->get_width(c)) - m_text_port.x);
  if ((c == '\n') || (width > m_text_port.width)) {
    uint8_t font_height = scale * (m_font->HEIGHT);
    uint8_t line_height = scale * (m_font->HEIGHT + m_line_spacing);
    uint8_t y = m_cursor.y + line_height;
    if (y + font_height > m_text_port.y + m_text_port.height) {
      y = m_text_port.y;
    }
    x = m_text_port.x;
    Canvas::color16_t saved = set_pen_color(get_canvas_color());
    set_cursor(x, y);
    m_canvas->fill_rect(m_text_port.width, line_height);
    set_pen_color(saved);
  } 

  // Form-feed
  else if (c == '\f') {
    Canvas::color16_t saved = set_pen_color(get_canvas_color());
    set_cursor(m_text_port.x, m_text_port.y);
    m_canvas->fill_rect(m_text_port.width, m_text_port.height);
    set_pen_color(saved);
  }

  // Draw only normal characters
  else if (c >= ' ') m_canvas->draw_char(c);
  
  // Restore the previous canvas state; context
  m_canvas->set_context(saved);
  return (c);
}
