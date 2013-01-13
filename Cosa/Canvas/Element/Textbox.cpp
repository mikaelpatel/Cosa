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
 * Canvas.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Canvas/Element/Textbox.hh"

int 
Textbox::putchar(char c)
{ 
  // Save current canvas drawing state
  uint8_t x, y; m_canvas->get_cursor(x, y);
  uint16_t pen_color = m_canvas->get_pen_color();
  uint16_t text_color = m_canvas->get_text_color();
  uint16_t canvas_color = m_canvas->get_canvas_color();
  uint8_t scale = m_canvas->get_text_scale();
  m_canvas->set_pen_color(m_canvas_color);
  m_canvas->set_text_color(m_text_color);
  m_canvas->set_cursor(m_caret.x, m_caret.y);
  m_canvas->set_text_scale(m_text_scale);
  m_canvas->set_canvas_color(m_canvas_color);

  // Draw only normal characters
  if (c >= ' ') {
    m_canvas->draw_char(c);
    m_canvas->get_cursor(m_caret.x, m_caret.y);
  }

  // Handle some special characters, new-line
  uint8_t width = (m_caret.x + m_font->WIDTH + CHAR_SPACING - m_text_port.x);
  if ((c == '\n') || (width > m_text_port.width)) {
    m_caret.x = m_text_port.x;
    m_caret.y += m_text_scale * (m_font->HEIGHT + LINE_SPACING);
    if (m_caret.y + m_font->HEIGHT > m_text_port.y + m_text_port.height)
      m_caret.y = m_text_port.y;
    m_canvas->fill_rect(m_caret.x, m_caret.y, m_text_port.width, 
			m_text_scale*(m_font->HEIGHT + LINE_SPACING));
  } 

  // form-feed
  else if (c == '\f') {
    m_canvas->fill_rect(m_text_port.x, m_text_port.y, m_text_port.width, 
			m_text_port.height);
    m_caret.x = m_text_port.x;
    m_caret.y = m_text_port.y;
  }

  // Restore the canvas drawing state
  m_canvas->set_canvas_color(canvas_color);
  m_canvas->set_pen_color(pen_color);
  m_canvas->set_text_color(text_color);
  m_canvas->set_text_scale(scale);
  m_canvas->set_cursor(x, y);
  return (c);
}
