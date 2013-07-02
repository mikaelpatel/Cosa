/**
 * @file Cosa/LCD.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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

#ifndef __COSA_LCD_HH__
#define __COSA_LCD_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

/**
 * Abstract LCD interface with common interface for LCD drivers.
 */
class LCD : public IOStream {
public:
  class Device : public IOStream::Device {
  protected:
    uint8_t m_x;		/**< Cursor position x */
    uint8_t m_y;		/**< Cursor position y */
    uint8_t m_tab;		/**< Tab step */
    uint8_t m_mode;		/**< Text mode */

  public:
    /** Text display mode */
    enum TextMode {
      NORMAL_TEXT_MODE = 0x00,
      INVERTED_TEXT_MODE = 0xff
    } __attribute__((packed));

    /**
     * Initate LCD device driver.
     */
    Device() :
      IOStream::Device(),
      m_x(0),
      m_y(0),
      m_tab(4),
      m_mode(0)
    {
    }
    
    /**
     * @override
     * Start display for text output. Returns true if successful 
     * otherwise false.
     * @return boolean.
     */
    virtual bool begin() = 0;

    /**
     * @override
     * Stop display and power down. Returns true if successful 
     * otherwise false.
     */
    virtual bool end() = 0;
    
    /**
     * @override
     * Turn display backlight on. 
     */
    virtual void backlight_on() {}

    /**
     * @override
     * Turn display backlight off. 
     */
    virtual void backlight_off() {}

    /**
     * @override
     * Turn display on. 
     */
    virtual void display_on() = 0;

    /**
     * @override
     * Turn display off. 
     */
    virtual void display_off() = 0;
  
    /**
     * @override
     * Display normal mode.
     */
    virtual void display_normal() {}

    /**
     * @override
     * Display inverse mode. 
     */
    virtual void display_inverse() {}

    /**
     * @override
     * Clear display and move cursor to home.
     */
    virtual void display_clear() = 0;

    /**
     * Get current cursor position.
     * @param[out] x.
     * @param[out] y.
     */
    void get_cursor(uint8_t& x, uint8_t& y)
    {
      x = m_x;
      y = m_y;
    }

    /**
     * @override
     * Set cursor position to given position.
     * @param[in] x.
     * @param[in] y.
     */
    virtual void set_cursor(uint8_t x, uint8_t y) = 0;

    /**
     * Get tab step.
     * @return tab step.
     */
    uint8_t get_tab_step()
    {
      return (m_tab);
    }

    /**
     * Set tab step to given value.
     * @param[in] tab step.
     */
    void set_tab_step(uint8_t step)
    {
      m_tab = step;
    }

    /**
     * Set text mode. Return previous text mode.
     * @param[in] mode new text mode.
     * @return previous text mode.
     */
    TextMode set_text_mode(TextMode mode)
    {
      TextMode previous = (TextMode) m_mode;
      m_mode = mode;
      return (previous);
    }
  };
};

#endif
