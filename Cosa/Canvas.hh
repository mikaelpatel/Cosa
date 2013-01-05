/**
 * @file Cosa/Canvas.hh
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
 * Virtual Canvas device/IOStream; abstraction of small screens,
 * LCD/TFT. See Cosa/SPI/ST7735R.hh for an example of usage.
 *
 * @section Limitations
 * Color model is 16-bit RBG<5,6,5>. Canvas size is max 256x256.
 *
 * @section Acknowledgement
 * Inspired by graphics library by ladyada/adafruit.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_HH__
#define __COSA_CANVAS_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"
#include "Cosa/Font/System5x7.hh"

class Canvas : public IOStream::Device {

protected:
  /**
   * Current drawing color; 16-bit RGB<5,6,5>
   */
  uint16_t m_canvas_color;
  uint16_t m_pen_color;

  /**
   * Current position (turtle graphics style)
   */
  struct {
    uint8_t x;
    uint8_t y;
  } m_cursor;

  /**
   * Text handling; font, color, scale and port.
   */
  Font* m_font;
  uint16_t m_text_color;
  uint8_t m_text_scale;
  struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
  } m_text_port;
public:
  /**
   * Basic color palette.
   */
  enum {
    BLACK = 0x0000U,
    WHITE = 0xFFFFU,
    RED = 0xF800U,
    GREEN = 0x07E0U,
    BLUE = 0x001FU,
    YELLOW = RED + GREEN,
    CYAN = GREEN + BLUE,
    MAGENTA = RED + BLUE
  };
  
  /**
   * Screen size; width/height and orientation
   */
  uint8_t WIDTH;
  uint8_t HEIGHT;
  enum {
    PORTRAIT = 0,
    LANDSCAPE = 1,
  };

  /**
   * Character/line spacing
   */
  uint8_t CHAR_SPACING;
  uint8_t LINE_SPACING;

  /**
   * Construct canvas object and initiate.
   * @param[in] width screen width.
   * @param[in] height screen height.
   * @param[in] font text font (default 5x7).
   */
  Canvas(uint8_t width, uint8_t height, Font* font = &system5x7) :
    IOStream::Device(),
    m_canvas_color(WHITE),
    m_pen_color(BLACK),
    m_font(font),
    m_text_color(BLACK),
    m_text_scale(1),
    WIDTH(width),
    HEIGHT(height),
    CHAR_SPACING(1),
    LINE_SPACING(2)
  {
    set_cursor(0, 0);
    set_text_port(0, 0, width, height);
  }

  /**
   * Start interaction with device. Must override.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin() = 0;

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
   * Get current drawing color.
   * @return color.
   */
  uint16_t get_pen_color()
  {
    return (m_pen_color);
  }

  /**
   * Set current drawing color.
   * @param[in] color
   */
  void set_pen_color(uint16_t color)
  {
    m_pen_color = color;
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
   * Set current text font.
   * @param[in] font
   */
  void set_font(Font* font)
  {
    m_font = font;
  }

  /**
   * Create 16-bit color from primary colors (RGB).
   * @param[in] red
   * @param[in] green
   * @param[in] blue
   * @return color.
   */
  uint16_t color(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * Create color shade (0..100%)
   * @param[in] scale
   * @return color shade.
   */
  uint16_t shade(uint16_t color, uint8_t scale);

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
  }

  /**
   * Get current cursor position.
   * @param[out] x
   * @param[out] y
   */
  void get_cursor(uint8_t& x, uint8_t& y)
  {
    x = m_cursor.x;
    y = m_cursor.y;
  }

  /**
   * Set current cursor position.
   * @param[in] x
   * @param[in] y
   */
  void set_cursor(uint8_t x, uint8_t y)
  {
    m_cursor.x = x;
    m_cursor.y = y;
  }

  /**
   * Move current cursor to delta position.
   * @param[in] dx
   * @param[in] dy
   */
  void move_cursor(int8_t dx, int8_t dy)
  {
    m_cursor.x += dx;
    m_cursor.y += dy;
  }

  /**
   * Set screen orientation.
   * @param[in] direction (LANDSCAPE/PORTRAIT)
   */
  virtual void set_orientation(uint8_t direction) {}

  /**
   * Set pixel with current color.
   * @param[in] x
   * @param[in] y
   */
  virtual void draw_pixel(uint8_t x, uint8_t y);

  /**
   * Set pixel at cursor position with current color.
   */
  void draw_pixel()
  {
    draw_pixel(m_cursor.x, m_cursor.y);
  }

  /**
   * Draw bitmap with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   * @param[in] width
   * @param[in] height
   */
  virtual void draw_bitmap(uint8_t x, uint8_t y, const uint8_t* bp, 
			   uint8_t width, uint8_t height);

  /**
   * Draw bitmap at cursor position with current color.
   * @param[in] bp
   * @param[in] width
   * @param[in] height
   */
  void draw_bitmap(const uint8_t* bp, uint8_t width, uint8_t height)
  {
    draw_bitmap(m_cursor.x, m_cursor.y, bp, width, height);
  }
  
  /**
   * Draw icon at given position with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   */
  virtual void draw_icon(uint8_t x, uint8_t y, const uint8_t* bp);

  /**
   * Draw icon at cursor position with current color.
   * @param[in] bp
   */
  void draw_icon(const uint8_t* bp)
  {
    draw_icon(m_cursor.x, m_cursor.y, bp);
  }
  
  /**
   * Draw line with current color.
   * @param[in] x0 
   * @param[in] y0
   * @param[in] x1
   * @param[in] y1
   */
  virtual void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

  /**
   * Draw line to given position with current color. Update cursor to
   * new position.
   * @param[in] x
   * @param[in] y
   */
  void draw_line(uint8_t x, uint8_t y)
  {
    draw_line(m_cursor.x, m_cursor.y, x, y);
    m_cursor.x = x;
    m_cursor.y = y;
  }

  /**
   * Draw vertical line with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  virtual void draw_vertical_line(uint8_t x, uint8_t y, uint8_t length)
  {
    draw_line(x, y, x, y + length);
  }

  /**
   * Draw vertical line with given length and current color. Update
   * cursor to new position.
   * @param[in] length
   */
  void draw_vertical_line(uint8_t length)
  {
    draw_line(m_cursor.x, m_cursor.y + length);
  }

  /**
   * Draw horizontal line with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  virtual void draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length)
  {
    draw_line(x, y, x + length, y);
  }

  /**
   * Draw horizontal line with given length and current color. Update
   * cursor to new position.
   * @param[in] length
   */
  void draw_horizontal_line(uint8_t length)
  {
    draw_line(m_cursor.x + length, m_cursor.y);
  }

  /**
   * Draw rectangle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  virtual void draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

  /**
   * Draw rectangle at cursor position with current color.
   * @param[in] width
   * @param[in] height
   */
  void draw_rect(uint8_t width, uint8_t height)
  {
    draw_rect(m_cursor.x, m_cursor.y, width, height);
  }

  /**
   * Fill rectangle with current color. Must override.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  virtual void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) = 0;

  /**
   * Fill rectangle at cursor position with current color.
   * @param[in] width
   * @param[in] height
   */
  void fill_rect(uint8_t width, uint8_t height)
  {
    fill_rect(m_cursor.x, m_cursor.y, width, height);
  }
  
  /**
   * Fill screen with current color.
   */
  virtual void fill_screen()
  {
    uint16_t saved = m_pen_color;
    m_pen_color = m_canvas_color;
    fill_rect(0, 0, WIDTH, HEIGHT);
    m_pen_color = saved;
  }

  /**
   * Draw circle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  virtual void draw_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Draw circle at cursor position with current color.
   * @param[in] radius
   */
  void draw_circle(uint8_t radius)
  {
    draw_circle(m_cursor.x, m_cursor.y, radius);
  }

  /**
   * Fill circle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  virtual void fill_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Fill circle at cursor position with current color.
   * @param[in] radius
   */
  void fill_circle(uint8_t radius)
  {
    fill_circle(m_cursor.x, m_cursor.y, radius);
  }

  /**
   * Draw character with current color and font.
   * @param[in] c
   */
  virtual void draw_char(char c);

  /**
   * Draw string in current color and font.
   * @param[in] s
   */
  virtual void draw_string(char* s)
  {
    char c;
    while ((c = *s++) != 0) draw_char(c);
  }

  /**
   * Draw string from program memory with current color and font.
   * @param[in] s
   */
  virtual void draw_string_P(const char* s)
  {
    char c;
    while ((c = pgm_read_byte(s++)) != 0) draw_char(c);
  }

  /**
   * @override
   * Write character at current cursor position, with current text
   * color, scale and font.  
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * Stop sequence of interaction with device. Must override.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end() = 0;

  /**
   * Drawing instructions and arguments (in program memory).
   */
  enum {
    END_SCRIPT = 0,		// (void)
    CALL_SCRIPT,		// (uint8_t ix) index of script in table
    SET_CANVAS_COLOR,		// (uint8_t r, g, b) 24-bit color code
    SET_PEN_COLOR,		// (uint8_t r, g, b) 24-bit color code
    SET_TEXT_COLOR,		// (uint8_t r, g, b) 24-bit color code
    SET_TEXT_SCALE,		// (uint8_t s) scale factor (1..n)
    SET_TEXT_PORT,		// (uint8_t x, y, w, h) text port
    SET_CURSOR,			// (uint8_t x, y) cursor position
    MOVE_CURSOR,		// (int8_t dx, dy) cursor delta position
    DRAW_BITMAP,		// (uint8_t ix, w, h) index, width and height
    DRAW_ICON,	       		// (uint8_t ix) index to icon in table
    DRAW_PIXEL,			// (void)
    DRAW_LINE,			// (uint8_t x, y) line end position
    DRAW_RECT,			// (uint8_t w, h) width and height of rectangle
    FILL_RECT,			// (uint8_t w, h) width and height of rectangle
    FILL_SCREEN,		// (void)
    DRAW_CIRCLE,		// (uint8_t r) radius of circle
    FILL_CIRCLE,		// (uint8_t r) radius of circle
    DRAW_CHAR,			// (uint8_t c) character
    DRAW_STRING_P		// (uint8_t ix) index of string in table
  };

  /**
   * Run canvas drawing script. Table may contain sub-scripts and strings.
   * All should be in program memory.
   * @param[in] ix script to run.
   * @param[in] tab script table.
   * @param[in] max size of script table.
   */
  void run(uint8_t ix, PGM_VOID_P* tab, uint8_t max);
};

#endif
