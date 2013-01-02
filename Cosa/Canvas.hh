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
 * Color model is 16-bit RBG<5,6,5>.
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
#include "Cosa/Font5x7.hh"

class Canvas : public IOStream::Device {

private:
  /**
   * @override
   * Write character to canvas device.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c) 
  { 
    draw_char(c);
    return (1);
  }

protected:
  /**
   * Current drawing color; RGB<5,6,5>
   */
  uint16_t m_canvas_color;
  uint16_t m_pen_color;
  
  /**
   * Text handling; font, color, scale and position.
   */
  Font* m_font;
  uint16_t m_text_color;
  uint8_t m_scale;
  uint8_t m_x;
  uint8_t m_y;

public:
  /**
   * Basic color palette.
   */
  enum {
    WHITE = 0xFFFFU,
    BLACK = 0x0000U,
    RED = 0xF800U,
    GREEN = 0x07E0U,
    BLUE = 0x001FU,
    CYAN = GREEN + BLUE,
    MAGENTA = RED + BLUE,
    YELLOW = RED + GREEN
  };
  
  /**
   * Screen size; width/height
   */
  const uint8_t SCREEN_WIDTH;
  const uint8_t SCREEN_HEIGHT;
  
  /**
   * Character/line spacing
   */
  uint8_t CHAR_SPACING;
  uint8_t LINE_SPACING;

  /**
   * Construct canvas object and initiate.
   */
  Canvas(uint8_t width, uint8_t height, Font* font = &font5x7) :
    IOStream::Device(),
    m_canvas_color(WHITE),
    m_pen_color(BLACK),
    m_font(font),
    m_text_color(BLACK),
    m_scale(1),
    m_x(0),
    m_y(0),
    SCREEN_WIDTH(width),
    SCREEN_HEIGHT(height),
    CHAR_SPACING(1),
    LINE_SPACING(2)
  {}

  /**
   * Start interaction with device.
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
   * Create color from primary colors (RGB).
   * @param[in] red
   * @param[in] green
   * @param[in] blue
   * @return color.
   */
  uint16_t color(uint8_t red, uint8_t green, uint8_t blue)
  {
    return (((red & 0x1f) << 11) | ((green & 0x3f) << 5) | (blue & 0x1f));
  }

  /**
   * Create gray scale shade (0..100%)
   * @param[in] scale
   * @return grayscale.
   */
  uint16_t grayscale(uint8_t scale)
  {
    if (scale > 100) scale = 100;
    uint8_t level = (scale * 0x1fU) / 100;
    return (color(level, level << 1, level));
  }

  /**
   * Create red shade (0..100%)
   * @param[in] scale
   * @return red shade.
   */
  uint16_t red(uint8_t scale)
  {
    if (scale > 100) scale = 100;
    uint8_t level = (scale * 0x1fU) / 100;
    return (color(level, 0, 0));
  }

  /**
   * Create green shade (0..100%)
   * @param[in] scale
   * @return green shade.
   */
  uint16_t green(uint8_t scale)
  {
    if (scale > 100) scale = 100;
    uint8_t level = (scale * 0x3fU) / 100;
    return (color(0, level, 0));
  }

  /**
   * Create blue shade (0..100%)
   * @param[in] scale
   * @return red shade.
   */
  uint16_t blue(uint8_t scale)
  {
    if (scale > 100) scale = 100;
    uint8_t level = (scale * 0x1fU) / 100;
    return (color(0, 0, level));
  }

  /**
   * Get current scale.
   * @return scale.
   */
  uint8_t get_scale()
  {
    return (m_scale);
  }

  /**
   * Set current scale (1..n).
   * @param[in] scale.
   */
  void set_scale(uint8_t scale)
  {
    m_scale = (scale > 0 ? scale : 1);
  }

  /**
   * Set current text position.
   * @param[out] x
   * @param[out] y
   */
  void get_cursor(uint8_t& x, uint8_t& y)
  {
    x = m_x;
    y = m_y;
  }

  /**
   * Set current text position.
   * @param[in] x
   * @param[in] y
   */
  void set_cursor(uint8_t x, uint8_t y)
  {
    m_x = x;
    m_y = y;
  }

  /**
   * Set pixel with current color.
   * @param[in] x
   * @param[in] y
   */
  virtual void draw_pixel(uint8_t x, uint8_t y);

  /**
   * Draw bitmap with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   * @param[in] width
   * @param[in] height
   * @param[in] scale
   */
  virtual void draw_bitmap(uint8_t x, uint8_t y, const uint8_t* bp, 
			   uint8_t width, uint8_t height);
  
  /**
   * Draw line with current color.
   * @param[in] x0 
   * @param[in] y0
   * @param[in] x1
   * @param[in] y1
   */
  virtual void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

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
   * Draw rectangle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  virtual void draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

  /**
   * Fill rectangle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  virtual void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) = 0;
  
  /**
   * Fill screen with current color.
   */
  virtual void fill_screen()
  {
    fill_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  /**
   * Draw circle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  virtual void draw_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Fill circle with current color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  virtual void fill_circle(uint8_t x, uint8_t y, uint8_t radius);

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
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end() = 0;
};

#endif
