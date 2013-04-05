/**
 * @file Cosa/Canvas.hh
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
 * @section Description
 * Virtual Canvas device; abstraction of small screens, LCD/TFT. 
 * Device drivers need to override at least begin(), fill_rect()
 * and end(). See Cosa/SPI/ST7735R.hh for an example of usage.
 *
 * @section Limitations
 * Color model is 16-bit RGB<5,6,5>. Canvas size is max 256x256.
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

// Forward declaration of default system font
class Font;
class System5x7;
extern System5x7 system5x7;

class Canvas {
public:
  /**
   * 16-bit RGB<5,6,5> color.
   */
  union color16_t {
    uint16_t rgb;
    struct {
      unsigned int blue:5;
      unsigned int green:6;
      unsigned int red:5;
    };
    color16_t()
    {
      rgb = 0;
    }
    color16_t(uint16_t color) 
    {
      rgb = color;
    }
    color16_t(uint8_t r, uint8_t g, uint8_t b) 
    {
      red = r;
      green = g;
      blue = b;
    }
  };

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
   * circle<x, y, radius> data structure.
   */
  struct circle8_t {
    uint8_t x;
    uint8_t y;
    uint8_t radius;
  };

  /**
   * Drawing context; canvas, pen and text color. Font and text scale.
   */
  class Context {
  protected:
    color16_t m_pen_color;
    color16_t m_canvas_color;
    color16_t m_text_color;
    uint8_t m_text_scale;
    Font* m_font;
    pos8_t m_cursor;

  public:
    /**
     * Construct a drawing context with default pen color(BLACK),
     * canvas color(WHITE), text color(BLACK), text scale(1),
     * and cursor at (0, 0).
     * @param[in] font default is the system font.
     */
    Context(Font* font = (Font*) &system5x7) :
      m_pen_color(BLACK),
      m_canvas_color(WHITE),
      m_text_color(BLACK),
      m_text_scale(1),
      m_font(font)
    {
      set_cursor(0, 0);
    }

    /**
     * Get context canvas color.
     * @return color.
     */
    color16_t get_canvas_color()
    {
      return (m_canvas_color);
    }

    /**
     * Set context canvas color. Return previous color.
     * @param[in] color
     * @return previous color.
     */
    color16_t set_canvas_color(color16_t color)
    {
      color16_t previous = m_canvas_color;
      m_canvas_color = color;
      return (previous);
    }

    /**
     * Get context drawing color.
     * @return color.
     */
    color16_t get_pen_color()
    {
      return (m_pen_color);
    }

    /**
     * Set context drawing color. Return previous color.
     * @param[in] color
     * @return previous color.
     */
    color16_t set_pen_color(color16_t color)
    {
      color16_t previous = m_pen_color;
      m_pen_color = color;
      return (previous);
    }

    /**
     * Get context text color.
     * @return color.
     */
    color16_t get_text_color()
    {
      return (m_text_color);
    }

    /**
     * Set context text color. Return previous color.
     * @param[in] color
     * @return previous color.
     */
    color16_t set_text_color(color16_t color)
    {
      color16_t previous = m_text_color;
      m_text_color = color;
      return (previous);
    }

    /**
     * Get context text font.
     */
    Font* get_text_font()
    {
      return (m_font);
    }

    /**
     * Set context text font. Return previous color.
     * @param[in] font
     * @return previous font.
     */
    Font* set_text_font(Font* font)
    {
      Font* previous = m_font;
      m_font = font;
     return (previous);
    }

    /**
     * Get context text scale.
     * @return text scale.
     */
    uint8_t get_text_scale()
    {
      return (m_text_scale);
    }

    /**
     * Set context text scale (1..n). Return previous text scale.
     * @param[in] scale.
     * @return previous scale.
     */
    uint8_t set_text_scale(uint8_t scale)
    {
      uint8_t previous = m_text_scale;
      m_text_scale = (scale > 0 ? scale : 1);
      return (previous);
    }

    /**
     * Get context cursor position.
     * @param[out] x
     * @param[out] y
     */
    void get_cursor(uint8_t& x, uint8_t& y)
    {
      x = m_cursor.x;
      y = m_cursor.y;
    }

    /**
     * Set context cursor position.
     * @param[in] x
     * @param[in] y
     */
    void set_cursor(uint8_t x, uint8_t y)
    {
      m_cursor.x = x;
      m_cursor.y = y;
    }

    /**
     * Move context cursor to delta position.
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
   * that requires a drawing context. See Textbox() for an example.
   */
  class Element : public Context {
  protected:
    Canvas* m_canvas;
  public:
    /**
     * Construct an element with the default context on the given 
     * canvas.
     * @param[in] canvas
     * @param[in] font
     */
    Element(Canvas* canvas, Font* font = (Font*) &system5x7) :
      Context(font),
      m_canvas(canvas)
    {
    }
  };
  
protected:
  /**
   * Canvas context; default and current. Delegation pattern.
   */
  static Context context;
  Context* m_context;

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
  } __attribute__((packed));

  /**
   * Construct canvas object and initiate.
   * @param[in] width screen width.
   * @param[in] height screen height.
   * @param[in] context canvas state.
   */
  Canvas(uint8_t width, uint8_t height, Context* context = &Canvas::context) :
    m_context(context),
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
   * Get current canvas context.
   * @return context.
   */
  Context* get_context()
  {
    return (m_context);
  }

  /**
   * Set current canvas context. Return previous context.
   * @param[in] context.
   * @return previous context.
   */
  Context* set_context(Context* context)
  {
    Context* previous = m_context;
    m_context = context;
    return (previous);
  }

  /**
   * Get current canvas color.
   * @return color.
   */
  color16_t get_canvas_color()
  {
    return (m_context->get_canvas_color());
  }

  /**
   * Set current canvas color. Return previous color.
   * @param[in] color
   * @return previous color.
   */
  color16_t set_canvas_color(color16_t color)
  {
    return (m_context->set_canvas_color(color));
  }

  /**
   * Get current drawing color.
   * @return color.
   */
  color16_t get_pen_color()
  {
    return (m_context->get_pen_color());
  }

  /**
   * Set current drawing color. Return previous color.
   * @param[in] color
   * @return previous color.
   */
  color16_t set_pen_color(color16_t color)
  {
    return (m_context->set_pen_color(color));
  }

  /**
   * Get current text color.
   * @return color.
   */
  color16_t get_text_color()
  {
    return (m_context->get_text_color());
  }

  /**
   * Set current text color. Return previous color.
   * @param[in] color
   * @return previous color.
   */
  color16_t set_text_color(color16_t color)
  {
    return (m_context->set_text_color(color));
  }

  /**
   * Get current text font.
   */
  Font* get_text_font()
  {
    return (m_context->get_text_font());;
  }

  /**
   * Set current text font. Return previous font.
   * @param[in] font
   * @return previous font.
   */
  Font* set_text_font(Font* font)
  {
    return (m_context->set_text_font(font));
  }

  /**
   * Get current text scale.
   * @return text scale.
   */
  uint8_t get_text_scale()
  {
    return (m_context->get_text_scale());
  }

  /**
   * Set current text scale (1..n). Return previous scale.
   * @param[in] scale.
   * @return previous scale.
   */
  uint8_t set_text_scale(uint8_t scale)
  {
    return (m_context->set_text_scale(scale));
  }

  /**
   * Get current cursor position.
   * @param[out] x
   * @param[out] y
   */
  void get_cursor(uint8_t& x, uint8_t& y)
  {
    m_context->get_cursor(x, y);
  }

  /**
   * Set current cursor position.
   * @param[in] x
   * @param[in] y
   */
  void set_cursor(uint8_t x, uint8_t y)
  {
    m_context->set_cursor(x, y);
  }

  /**
   * Move current cursor to delta position.
   * @param[in] dx
   * @param[in] dy
   */
  void move_cursor(int8_t dx, int8_t dy)
  {
    m_context->move_cursor(dx, dy);
  }

  /**
   * Create 16-bit color from primary colors (RGB).
   * @param[in] red
   * @param[in] green
   * @param[in] blue
   * @return color.
   */
  color16_t color(uint8_t red, uint8_t green, uint8_t blue)
  {
    return (color16_t(red, green, blue));
  }

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
   * @return direction (Canvas::LANDSCAPE/PORTRAIT)
   */
  virtual uint8_t get_orientation();

  /**
   * Set screen orientation. Return previous orientation.
   * @param[in] direction (Canvas::LANDSCAPE/PORTRAIT)
   * @return previous orientation.
   */
  virtual uint8_t set_orientation(uint8_t direction);

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
   * Draw bitmap with current pen color. The bitmap must be stored
   * in program memory.
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
   * Draw bitmap at cursor position with current pen color. The bitmap
   * must be stored in program memory.
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
   * Draw icon at given position with current pen color. The icon must
   * be stored in program memory.
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
   * Draw icon at given position with current pen color. The icon must
   * be stored in program memory.
   * @param[in] x 
   * @param[in] y
   * @param[in] bp
   * @param[in] scale
   */
  virtual void draw_icon(uint8_t x, uint8_t y, const uint8_t* bp, 
			 uint8_t scale = 1);

  /**
   * Draw icon at cursor position with current pen color. The icon
   * must be stored in program memory.
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
   * @param[in] x1
   * @param[in] y1
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
  virtual void draw_vertical_line(uint8_t x, uint8_t y, uint8_t length);

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
  virtual void draw_horizontal_line(uint8_t x, uint8_t y, uint8_t length);

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
   * delta positions, terminate with 0, 0. Update cursor to end position.
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
  virtual void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

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
   * Draw round corner rectangle with current pen color.
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   * @param[in] radius
   */
  virtual void draw_roundrect(uint8_t x, uint8_t y, 
			      uint8_t width, uint8_t height,
			      uint8_t radius);

  /**
   * Draw round corner rectangle at cursor position with current pen color.
   * @param[in] width
   * @param[in] height
   * @param[in] radius
   */
  void draw_roundrect(uint8_t width, uint8_t height, uint8_t radius)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_roundrect(x, y, width, height, radius);
  }

  /**
   * Fill round corner rectangle with current pen color. 
   * @param[in] x 
   * @param[in] y
   * @param[in] width
   * @param[in] height
   * @param[in] radius
   */
  virtual void fill_roundrect(uint8_t x, uint8_t y, 
			      uint8_t width, uint8_t height,
			      uint8_t radius);

  /**
   * Fill round corner rectangle at cursor position with current pen color.
   * @param[in] width
   * @param[in] height
   * @param[in] radius
   */
  void fill_roundrect(uint8_t width, uint8_t height, uint8_t radius)
  {
    uint8_t x, y;
    get_cursor(x, y);
    fill_roundrect(x, y, width, height, radius);
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
   * Draw character with current text color, font and scale.
   * @param[in] x position.
   * @param[in] y position.
   * @param[in] c character.
   */
  virtual void draw_char(uint8_t x, uint8_t y, char c);

  /**
   * Draw character with current text color, font and scale.
   * @param[in] c character.
   */
  void draw_char(char c)
  {
    uint8_t x, y;
    get_cursor(x, y);
    draw_char(x, y, c);
  }
  
  /**
   * Draw string in current text color, font and scale.
   * @param[in] s string.
   */
  virtual void draw_string(char* s);

  /**
   * Draw string from program memory with current text color and font.
   * @param[in] s string in program memory (PSTR).
   */
  virtual void draw_string_P(const char* s);

  /**
   * Fill screen with canvas color.
   */
  virtual void fill_screen();

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
    DRAW_ROUNDRECT,
    FILL_ROUNDRECT,
    DRAW_CIRCLE,
    FILL_CIRCLE,
    DRAW_CHAR,
    DRAW_STRING,
    FILL_SCREEN
  } __attribute__((packed));

  /**
   * Run canvas drawing script. Table may contain sub-scripts, strings
   * and icons. All should be in program memory. Use the support macro
   * set to write scripts.
   * @param[in] ix script to run.
   * @param[in] tab script table in program memory.
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
#define CANVAS_DRAW_ROUNDRECT(w, h, r) Canvas::DRAW_ROUNDRECT, w, h, r,
#define CANVAS_FILL_ROUNDRECT(w, h, r) Canvas::FILL_ROUNDRECT, w, h, r,
#define CANVAS_DRAW_CIRCLE(r) Canvas::DRAW_CIRCLE, r,
#define CANVAS_FILL_CIRCLE(r) Canvas::FILL_CIRCLE, r,
#define CANVAS_DRAW_CHAR(c) Canvas::DRAW_CHAR, c,
#define CANVAS_DRAW_STRING(ix) Canvas::DRAW_STRING, ix,
#define CANVAS_FILL_SCREEN() Canvas::FILL_SCREEN,
#define CANVAS_END_SCRIPT Canvas::END_SCRIPT };

#endif
