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
 * Virtual Canvas device; abstraction of small screens, LCD/TFT. 
 * See Cosa/SPI/ST7735R.hh for an example of usage.
 *
 * @section Limitations
 * Color model is 16-bit RBG<5,6,5>. Canvas size is max 256x256.
 *
 * @section Acknowledgements
 * Inspired by GFX graphics library by ladyada/adafruit, the glcd
 * library by Michael Margolis and Bill Perry, and scd library by
 * Sungjune Lee. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_CANVAS_HH__
#define __COSA_CANVAS_HH__

#include "Cosa/Types.h"

class Font;
class System5x7;
extern System5x7 system5x7;

class Canvas {
public:
  /**
   * 16-bit RGB<5,6,5> color.
   */
  typedef uint16_t color16_t;

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
   * Canvas position<x,y>.
   */
  struct pos8_t {
    uint8_t x;
    uint8_t y;
  };

  /**
   * Rectangle<x, y, width, height> data structure.
   */
  struct rect8_t {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
  };

  /**
   * Palette: the drawing context.
   */
  class Palette {
  protected:
    color16_t m_pen_color;
    color16_t m_canvas_color;
    color16_t m_text_color;
    uint8_t m_text_scale;
    Font* m_font;
    pos8_t m_cursor;
  public:
    /**
     * Construct a drawing context.
     * @param[in] font default is the system font.
     */
    Palette(Font* font = (Font*) &system5x7) :
      m_pen_color(BLACK),
      m_canvas_color(WHITE),
      m_text_color(BLACK),
      m_text_scale(1),
      m_font(font)
    {
      set_cursor(0, 0);
    }

    /**
     * Get palette canvas color.
     * @return color.
     */
    color16_t get_canvas_color()
    {
      return (m_canvas_color);
    }

    /**
     * Set palette canvas color.
     * @param[in] color
     */
    void set_canvas_color(color16_t color)
    {
      m_canvas_color = color;
    }

    /**
     * Get palette drawing color.
     * @return color.
     */
    color16_t get_pen_color()
    {
      return (m_pen_color);
    }

    /**
     * Set palette drawing color.
     * @param[in] color
     */
    void set_pen_color(color16_t color)
    {
      m_pen_color = color;
    }

    /**
     * Get palette text color.
     * @return color.
     */
    color16_t get_text_color()
    {
      return (m_text_color);
    }

    /**
     * Set palette text color.
     * @param[in] color
     */
    void set_text_color(color16_t color)
    {
      m_text_color = color;
    }

    /**
     * Get palette text font.
     */
    Font* get_text_font()
    {
      return (m_font);
    }

    /**
     * Set palette text font.
     * @param[in] font
     */
    void set_text_font(Font* font)
    {
      m_font = font;
    }

    /**
     * Get palette text scale.
     * @return text scale.
     */
    uint8_t get_text_scale()
    {
      return (m_text_scale);
    }

    /**
     * Set palette text scale (1..n).
     * @param[in] scale.
     */
    void set_text_scale(uint8_t scale)
    {
      m_text_scale = (scale > 0 ? scale : 1);
    }

    /**
     * Get palette cursor position.
     * @param[out] x
     * @param[out] y
     */
    void get_cursor(uint8_t& x, uint8_t& y)
    {
      x = m_cursor.x;
      y = m_cursor.y;
    }

    /**
     * Set palette cursor position.
     * @param[in] x
     * @param[in] y
     */
    void set_cursor(uint8_t x, uint8_t y)
    {
      m_cursor.x = x;
      m_cursor.y = y;
    }

    /**
     * Move palette cursor to delta position.
     * @param[in] dx
     * @param[in] dy
     */
    void move_cursor(int8_t dx, int8_t dy)
    {
      m_cursor.x += dx;
      m_cursor.y += dy;
    }
  };

  /**
   * Canvas drawing elements; base class for larger drawing structure
   * that requires a drawing context.
   */
  class Element : public Palette {
  protected:
    Canvas* m_canvas;
  public:
    Element(Canvas* canvas, Font* font = (Font*) &system5x7) :
      Palette(font),
      m_canvas(canvas)
    {
    }
  };
  
protected:
  /**
   * Canvas palette; default and current.
   */
  static Palette palette;
  Palette* m_palette;

  /**
   * Canvas direction.
   */
  uint8_t m_direction;

public:
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
   * Construct canvas object and initiate.
   * @param[in] width screen width.
   * @param[in] height screen height.
   * @param[in] palette canvas state.
   */
  Canvas(uint8_t width, uint8_t height, Palette* palette = &Canvas::palette) :
    m_palette(palette),
    m_direction(PORTRAIT),
    WIDTH(width),
    HEIGHT(height)
  {
  }

  /**
   * Start interaction with device. Must override.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin() = 0;

  /**
   * Get current canvas palette.
   * @return palette.
   */
  Palette* get_palette()
  {
    return (m_palette);
  }

  /**
   * Set current canvas palette.
   * @param[in] palette.
   */
  void set_palette(Palette* palette)
  {
    m_palette = palette;
  }

  /**
   * Get current canvas color.
   * @return color.
   */
  color16_t get_canvas_color()
  {
    return (m_palette->get_canvas_color());
  }

  /**
   * Set current canvas color.
   * @param[in] color
   */
  void set_canvas_color(color16_t color)
  {
    m_palette->set_canvas_color(color);
  }

  /**
   * Get current drawing color.
   * @return color.
   */
  color16_t get_pen_color()
  {
    return (m_palette->get_pen_color());
  }

  /**
   * Set current drawing color.
   * @param[in] color
   */
  void set_pen_color(color16_t color)
  {
    m_palette->set_pen_color(color);
  }

  /**
   * Get current text color.
   * @return color.
   */
  color16_t get_text_color()
  {
    return (m_palette->get_text_color());
  }

  /**
   * Set current text color.
   * @param[in] color
   */
  void set_text_color(color16_t color)
  {
    m_palette->set_text_color(color);
  }

  /**
   * Get current text font.
   */
  Font* get_text_font()
  {
    return (m_palette->get_text_font());;
  }

  /**
   * Set current text font.
   * @param[in] font
   */
  void set_text_font(Font* font)
  {
    m_palette->set_text_font(font);
  }

  /**
   * Get current text scale.
   * @return text scale.
   */
  uint8_t get_text_scale()
  {
    return (m_palette->get_text_scale());
  }

  /**
   * Set current text scale (1..n).
   * @param[in] scale.
   */
  void set_text_scale(uint8_t scale)
  {
    m_palette->set_text_scale(scale);
  }

  /**
   * Get current cursor position.
   * @param[out] x
   * @param[out] y
   */
  void get_cursor(uint8_t& x, uint8_t& y)
  {
    m_palette->get_cursor(x, y);
  }

  /**
   * Set current cursor position.
   * @param[in] x
   * @param[in] y
   */
  void set_cursor(uint8_t x, uint8_t y)
  {
    m_palette->set_cursor(x, y);
  }

  /**
   * Move current cursor to delta position.
   * @param[in] dx
   * @param[in] dy
   */
  void move_cursor(int8_t dx, int8_t dy)
  {
    m_palette->move_cursor(dx, dy);
  }

  /**
   * Create 16-bit color from primary colors (RGB).
   * @param[in] red
   * @param[in] green
   * @param[in] blue
   * @return color.
   */
  color16_t color(uint8_t red, uint8_t green, uint8_t blue);

  /**
   * Create color shade (0..100%)
   * @param[in] color
   * @param[in] scale
   * @return color shade.
   */
  color16_t shade(color16_t color, uint8_t scale);

  /**
   * Blend the two colors.
   * @param[in] c1
   * @param[in] c2
   * @return color blend.
   */
  color16_t blend(color16_t c1, color16_t c2);

  /**
   * Get screen orientation.
   * @return direction (LANDSCAPE/PORTRAIT)
   */
  virtual uint8_t get_orientation() 
  {
    return (m_direction);
  }

  /**
   * Set screen orientation.
   * @param[in] direction (LANDSCAPE/PORTRAIT)
   */
  virtual void set_orientation(uint8_t direction) 
  {
    m_direction = (direction & 1);
  }

  /**
   * Set pixel with current pen color.
   * @param[in] x
   * @param[in] y
   */
  virtual void draw_pixel(uint8_t x, uint8_t y);

  /**
   * Set pixel at cursor position with current pen color.
   */
  void draw_pixel()
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_pixel(x, y);
  }

  /**
   * Draw bitmap with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   * @param[in] width
   * @param[in] height
   * @param[in] scale
   */
  virtual void draw_bitmap(uint8_t x, uint8_t y, const uint8_t* bp, 
			   uint8_t width, uint8_t height, 
			   uint8_t scale = 1);

  /**
   * Draw bitmap at cursor position with current pen color.
   * @param[in] bp
   * @param[in] width
   * @param[in] height
   * @param[in] scale
   */
  void draw_bitmap(const uint8_t* bp, 
		   uint8_t width, uint8_t height,
		   uint8_t scale = 1)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_bitmap(x, y, bp, width, height, scale);
  }
  
  /**
   * Draw icon at given position with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   * @param[in] width
   * @param[in] height
   * @param[in] scale
   */
  virtual void draw_icon(uint8_t x, uint8_t y, const uint8_t* bp,
			 uint8_t width, uint8_t height,
			 uint8_t scale = 1);

  /**
   * Draw icon at given position with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   * @param[in] scale
   */
  virtual void draw_icon(uint8_t x, uint8_t y, const uint8_t* bp, 
			 uint8_t scale = 1)
  {
    uint8_t width = pgm_read_byte(bp++);
    uint8_t height = pgm_read_byte(bp++);
    draw_icon(x, y, bp, width, height, scale);
  }

  /**
   * Draw icon at cursor position with current pen color.
   * @param[in] bp
   * @param[in] scale
   */
  void draw_icon(const uint8_t* bp, uint8_t scale = 1)
  {
    uint8_t width = pgm_read_byte(bp++);
    uint8_t height = pgm_read_byte(bp++);
    uint8_t x, y;
    get_cursor(x, y);
    draw_icon(x, y, bp, width, height, scale);
  }
  
  /**
   * Draw line with current pen color.
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
  void draw_line(uint8_t x1, uint8_t y1)
  {
    uint8_t x0, y0;
    get_cursor(x0, y0);
    draw_line(x0, y0, x1, y1);
    set_cursor(x1, y1);
  }

  /**
   * Draw vertical line with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  virtual void draw_vertical_line(uint8_t x, uint8_t y, uint8_t length)
  {
    draw_line(x, y, x, y + length);
  }

  /**
   * Draw vertical line with given length and current pen color. Update
   * cursor to new position.
   * @param[in] length
   */
  void draw_vertical_line(uint8_t length)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_line(x, y + length);
  }

  /**
   * Draw horizontal line with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] length
   */
  virtual void draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length)
  {
    draw_line(x, y, x + length, y);
  }

  /**
   * Draw horizontal line with given length and current pen color. Update
   * cursor to new position.
   * @param[in] length
   */
  void draw_horizontal_line(uint8_t length)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_line(x + length, y);
  }

  /**
   * Draw polygon from program memory with current pen color. Vector of 
   * delta positions, terminate with 0, 0. Update cursor to new position.
   * @param[in] poly
   * @param[in] scale
   */
  virtual void draw_poly_P(const int8_t* poly, uint8_t scale = 1);

  /**
   * Draw stroke from program memory with current pen color. Vector of 
   * delta positions, terminated with 0, 0. The cursor is moved for
   * when both dx and dy are zero or negative. Update cursor to new
   * position. 
   * @param[in] stroke
   * @param[in] scale
   */
  virtual void draw_stroke_P(const int8_t* stroke, uint8_t scale = 1);

  /**
   * Draw rectangle with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  virtual void draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

  /**
   * Draw rectangle at cursor position with current pen color.
   * @param[in] width
   * @param[in] height
   */
  void draw_rect(uint8_t width, uint8_t height)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_rect(x, y, width, height);
  }

  /**
   * Fill rectangle with current pen color. Must override.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   */
  virtual void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) = 0;

  /**
   * Fill rectangle at cursor position with current pen color.
   * @param[in] width
   * @param[in] height
   */
  void fill_rect(uint8_t width, uint8_t height)
  {
    uint8_t x, y;
    get_cursor(x, y);
    fill_rect(x, y, width, height);
  }
  
  /**
   * Fill screen with current pen color.
   */
  virtual void fill_screen()
  {
    color16_t saved = get_pen_color();
    set_pen_color(get_canvas_color());
    fill_rect(0, 0, WIDTH, HEIGHT);
    set_pen_color(saved);
  }

  /**
   * Draw circle with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  virtual void draw_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Draw circle at cursor position with current pen color.
   * @param[in] radius
   */
  void draw_circle(uint8_t radius)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_circle(x, y, radius);
  }

  /**
   * Fill circle with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] radius
   */
  virtual void fill_circle(uint8_t x, uint8_t y, uint8_t radius);

  /**
   * Fill circle at cursor position with current pen color.
   * @param[in] radius
   */
  void fill_circle(uint8_t radius)
  {
    uint8_t x, y;
    get_cursor(x, y);
    fill_circle(x, y, radius);
  }

  /**
   * Draw character with current text color and font.
   * @param[in] c
   */
  virtual void draw_char(char c);

  /**
   * Draw string in current text color and font.
   * @param[in] s
   */
  virtual void draw_string(char* s)
  {
    char c; 
    while ((c = *s++) != 0) 
      draw_char(c);
  }

  /**
   * Draw string from program memory with current text color and font.
   * @param[in] s
   */
  virtual void draw_string_P(const char* s)
  {
    char c; 
    while ((c = pgm_read_byte(s++)) != 0) 
      draw_char(c);
  }

  /**
   * Stop sequence of interaction with device. Must override.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end() = 0;

  /**
   * Canvas script instructions. See macro set below for arguments.
   */
  enum {
    END_SCRIPT = 0,
    CALL_SCRIPT,
    SET_CANVAS_COLOR,
    SET_PEN_COLOR,
    SET_TEXT_COLOR,
    SET_TEXT_SCALE,
    SET_TEXT_FONT,
    SET_CURSOR,
    MOVE_CURSOR,
    DRAW_BITMAP,
    DRAW_ICON,
    DRAW_PIXEL,
    DRAW_LINE,
    DRAW_POLY,
    DRAW_STROKE,
    DRAW_RECT,
    FILL_RECT,
    FILL_SCREEN,
    DRAW_CIRCLE,
    FILL_CIRCLE,
    DRAW_CHAR,
    DRAW_STRING
  };

  /**
   * Run canvas drawing script. Table may contain sub-scripts and strings.
   * All should be in program memory. Use support macro set to write scripts.
   * @param[in] ix script to run.
   * @param[in] tab script table.
   * @param[in] max size of script table.
   */
  void run(uint8_t ix, PGM_VOID_P* tab, uint8_t max);
};

/**
 * Script support macro. Generate script code in program memory.
 */
#define CANVAS_BEGIN_SCRIPT(name) const uint8_t name[] PROGMEM = {
#define CANVAS_CALL_SCRIPT(ix) Canvas::CALL_SCRIPT, ix,
#define CANVAS_SET_CANVAS_COLOR(r, g, b) Canvas::SET_CANVAS_COLOR, r, g, b,
#define CANVAS_SET_PEN_COLOR(r, g, b) Canvas::SET_PEN_COLOR, r, g, b,
#define CANVAS_SET_TEXT_COLOR(r, g, b) Canvas::SET_TEXT_COLOR, r, g, b,
#define CANVAS_SET_TEXT_SCALE(s) Canvas::SET_TEXT_SCALE, s,
#define CANVAS_SET_TEXT_PORT(x, y, w, h) Canvas::SET_TEXT_PORT,	x, y, w, h,
#define CANVAS_SET_TEXT_FONT(ix) Canvas::SET_TEXT_FONT, ix, 
#define CANVAS_SET_CURSOR(x, y) Canvas::SET_CURSOR, x, y,
#define CANVAS_MOVE_CURSOR(dx, dy) Canvas::MOVE_CURSOR,	dx, dy,
#define CANVAS_DRAW_BITMAP(ix, w, h, s) Canvas::DRAW_BITMAP, ix, w, h, s,
#define CANVAS_DRAW_ICON(ix, s) Canvas::DRAW_ICON, ix, s,
#define CANVAS_DRAW_PIXEL() Canvas::DRAW_PIXEL,	
#define CANVAS_DRAW_LINE(x, y) Canvas::DRAW_LINE, x, y,
#define CANVAS_DRAW_POLY(ix, s) Canvas::DRAW_POLY, ix, s,
#define CANVAS_DRAW_STROKE(ix, s) Canvas::DRAW_POLY, ix, s,
#define CANVAS_DRAW_RECT(w, h) Canvas::DRAW_RECT, w, h,
#define CANVAS_FILL_RECT(w, h) Canvas::FILL_RECT, w, h,
#define CANVAS_FILL_SCREEN() Canvas::FILL_SCREEN,
#define CANVAS_DRAW_CIRCLE(r) Canvas::DRAW_CIRCLE, r,
#define CANVAS_FILL_CIRCLE(r) Canvas::FILL_CIRCLE, r,
#define CANVAS_DRAW_CHAR(c) Canvas::DRAW_CHAR, c,
#define CANVAS_DRAW_STRING(ix) Canvas::DRAW_STRING, ix,
#define CANVAS_END_SCRIPT Canvas::END_SCRIPT };

#endif
