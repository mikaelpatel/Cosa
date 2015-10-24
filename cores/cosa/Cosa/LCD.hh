/**
 * @file Cosa/LCD.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_LCD_HH
#define COSA_LCD_HH

#include "Cosa/Types.h"
#include "Cosa/SPI.hh"
#include "Cosa/Keypad.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/AnalogPin.hh"

/**
 * Common interface for LCD handlers; class LCD::Device as base
 * for device drivers and class LCD::IO for device port abstraction
 * with two predefined implementation classes; Serial3W and SPI3W.
 * Serial3W uses OutputPins and SPI3W uses the SPI driver.
 */
class LCD : public IOStream {
public:

  /**
   * LCD device driver interface. LCD delegates to device driver.
   */
  class Device : public IOStream::Device {
  public:
    /** Text display mode. */
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
    {}

    /**
     * @override{LCD::Device}
     * Start display for text output. Returns true if successful
     * otherwise false.
     * @return boolean.
     */
    virtual bool begin() = 0;

    /**
     * @override{LCD::Device}
     * Stop display and power down. Returns true if successful
     * otherwise false.
     */
    virtual bool end() = 0;

    /**
     * @override{LCD::Device}
     * Turn display backlight on.
     */
    virtual void backlight_on() {}

    /**
     * @override{LCD::Device}
     * Turn display backlight off.
     */
    virtual void backlight_off() {}

    /**
     * @override{LCD::Device}
     * Set display contrast level.
     * @param[in] level to set.
     */
    virtual void display_contrast(uint8_t level)
    {
      UNUSED(level);
    }

    /**
     * @override{LCD::Device}
     * Turn display on.
     */
    virtual void display_on() = 0;

    /**
     * @override{LCD::Device}
     * Turn display off.
     */
    virtual void display_off() = 0;

    /**
     * @override{LCD::Device}
     * Display normal mode.
     */
    virtual void display_normal() {}

    /**
     * @override{LCD::Device}
     * Display inverse mode.
     */
    virtual void display_inverse() {}

    /**
     * @override{LCD::Device}
     * Clear display and move cursor to home.
     */
    virtual void display_clear() = 0;

    /**
     * Get current cursor position.
     * @param[out] x.
     * @param[out] y.
     */
    void get_cursor(uint8_t& x, uint8_t& y) const
      __attribute__((always_inline))
    {
      x = m_x;
      y = m_y;
    }

    /**
     * @override{LCD::Device}
     * Set cursor position to given position.
     * @param[in] x.
     * @param[in] y.
     */
    virtual void set_cursor(uint8_t x, uint8_t y) = 0;

    /**
     * Get tab step.
     * @return tab step.
     */
    uint8_t get_tab_step() const
    {
      return (m_tab);
    }

    /**
     * Set tab step to given value.
     * @param[in] step tab.
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
    TextMode text_mode(TextMode mode)
      __attribute__((always_inline))
    {
      TextMode previous = (TextMode) m_mode;
      m_mode = mode;
      return (previous);
    }

  protected:
    uint8_t m_x;		//!< Cursor position x.
    uint8_t m_y;		//!< Cursor position y.
    uint8_t m_tab;		//!< Tab step.
    uint8_t m_mode;		//!< Text mode.
  };

  /**
   * Abstract LCD IO adapter to isolate communication specific
   * functions and allow access over software serial or hardware SPI.
   */
  class IO {
  public:
    /**
     * @override{LCD::IO}
     * Start of data/command transfer block.
     */
    virtual void begin() = 0;

    /**
     * @override{LCD::IO}
     * End of data/command transfer block.
     */
    virtual void end() = 0;

    /**
     * @override{LCD::IO}
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data) = 0;

    /**
     * @override{LCD::IO}
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(const void* buf, size_t size) = 0;
  };

  /**
   * LCD IO adapter for serial 3 wire (OutputPin).
   */
  class Serial3W : public IO {
  public:
    /**
     * Construct display device driver adapter with given pins.
     * @param[in] sdin screen data pin (default D6/D0).
     * @param[in] sclk screen clock pin (default D7/D1).
     * @param[in] sce screen chip enable pin (default D9/D3).
     */
#if !defined(BOARD_ATTINY)
    Serial3W(Board::DigitalPin sdin = Board::D6,
	     Board::DigitalPin sclk = Board::D7,
	     Board::DigitalPin sce = Board::D9) :
      m_sdin(sdin, 0),
      m_sclk(sclk, 0),
      m_sce(sce, 1)
    {}
#else
    Serial3W(Board::DigitalPin sdin = Board::D0,
	     Board::DigitalPin sclk = Board::D1,
	     Board::DigitalPin sce = Board::D3) :
      m_sdin(sdin, 0),
      m_sclk(sclk, 0),
      m_sce(sce, 1)
    {}
#endif

    /**
     * @override{LCD::IO}
     * Start of data/command transfer block.
     */
    virtual void begin()
    {
      m_sce.clear();
    }

    /**
     * @override{LCD::IO}
     * End of data/command transfer block.
     */
    virtual void end()
    {
      m_sce.set();
    }

    /**
     * @override{LCD::IO}
     * Write byte (8bit) to display. Must be in data/command transfer
     * block.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data)
    {
      m_sdin.write(data, m_sclk);
    }

    /**
     * @override{LCD::IO}
     * Write character buffer to display. Must be in data/command transfer
     * block.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(const void* buf, size_t size)
    {
      uint8_t* dp = (uint8_t*) buf;
      while (size--) m_sdin.write(*dp++, m_sclk);
    }

  protected:
    OutputPin m_sdin;		//!< Serial data input.
    OutputPin m_sclk;		//!< Serial clock input.
    OutputPin m_sce;		//!< Chip enable.
  };

  /**
   * LCD IO adapter for 3 wire SPI (MOSI, SCK and SCE).
   */
  class SPI3W : public IO, public SPI::Driver {
  public:
    /**
     * Construct display device driver adapter with given pins.
     * Implicit usage of SPI SCK(D13/D4) and MOSI(D11/D5).
     * @param[in] sce screen chip enable pin (default D9/D3).
     */
#if !defined(BOARD_ATTINY)
    SPI3W(Board::DigitalPin sce = Board::D9) : IO(), SPI::Driver(sce) {}
#else
    SPI3W(Board::DigitalPin sce = Board::D3) : IO(), SPI::Driver(sce) {}
#endif

    /**
     * @override{LCD::IO}
     * Start of data/command transfer block.
     */
    virtual void begin()
    {
      spi.acquire(this);
      spi.begin();
    }

    /**
     * @override{LCD::IO}
     * End of data/command transfer block.
     */
    virtual void end()
    {
      spi.end();
      spi.release();
    }

    /**
     * @override{LCD::IO}
     * Write byte (8bit) to display. Must be in data/command transfer
     * block.
     * @param[in] data (8b) to write.
     */
    virtual void write(uint8_t data)
    {
      spi.transfer(data);
    }

    /**
     * @override{LCD::IO}
     * Write character buffer to display. Must be in data/command transfer
     * block.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write(const void* buf, size_t size)
    {
      spi.write(buf, size);
    }
  };

  /**
   * LCD Keypad shield, keypad handler. The class represents the
   * necessary configuration; keypad sensor on analog pin A0 and
   * mapping vector.
   */
  class Keypad : public ::Keypad {
  public:
    // Key index
    enum {
      NO_KEY = 0,
      SELECT_KEY,
      LEFT_KEY,
      DOWN_KEY,
      UP_KEY,
      RIGHT_KEY
    } __attribute__((packed));

    /** LCD Keypad constructor with internal key map. */
    Keypad(Job::Scheduler* scheduler, Board::AnalogPin pin = Board::A0) :
      ::Keypad(scheduler, pin, m_map)
    {}

  private:
    /** Analog reading to key index map. */
    static const uint16_t m_map[] PROGMEM;
  };
};

#endif
