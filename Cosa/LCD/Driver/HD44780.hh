/**
 * @file Cosa/LCD/Driver/HD44780.hh
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

#ifndef __COSA_LCD_DRIVER_HD44780_HH__
#define __COSA_LCD_DRIVER_HD44780_HH__

#include "Cosa/Board.hh"
#if defined(__ARDUINO_TINYX5__)
#error "Cosa/LCD/Driver/HD4480.hh: board not supported"
#endif

#if !defined(__ARDUINO_TINY__)
#include "Cosa/TWI/Driver/PCF8574.hh"
#endif
#include "Cosa/LCD.hh"
#include "Cosa/Pins.hh"

/**
 * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
 * for LCD/IOStream access. Binding to trace, etc. Supports simple text 
 * scroll, cursor, and handling of special characters such as carriage-
 * return, form-feed, back-space, horizontal tab and new-line. 
 *
 * @section See Also
 * For furter details see Product Specification, Hitachi, HD4478U,
 * ADE-207-272(Z), '99.9, Rev. 0.0.
 */
class HD44780 : public LCD::Device {
protected:
  /**
   * Abstract HD44780 LCD IO handler to isolate communication specific
   * functions and allow access over parallel and serial interfaces;
   * Ports and I2C/TWI.
   */
  class IO {
  public:
    /**
     * @override
     * Initiate IO port. Called by HD44780::begin().
     */
    virtual void setup() = 0;

    /**
     * @override
     * Write LSB nibble (4bit) to display.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data) = 0;

    /**
     * @override
     * Set data/command mode; zero(0) for command, non-zero(1) for data mode. 
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag) = 0;

    /**
     * @override
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag) = 0;
  };

  /**
   * Bus Timing Characteristics (in micro-seconds), fig. 25, pp. 50
   */
  static const uint16_t SETUP_TIME = 1;
  static const uint16_t ENABLE_PULSE_WIDTH = 1;
  static const uint16_t HOLD_TIME = 1;
  static const uint16_t SHORT_EXEC_TIME = 50;
  static const uint16_t LONG_EXEC_TIME = 2000;
  static const uint16_t POWER_ON_TIME = 32;
  static const uint16_t INIT0_TIME = 4500;
  static const uint16_t INIT1_TIME = 150;

  /**
   * Instructions (Table 6, pp. 24), RS(0), RW(0)
   */
  enum {
    CLEAR_DISPLAY = 0x01,    	// Clears entrire display and return home
    RETURN_HOME = 0x02,	     	// Sets DDRAM 0 in address counter
    ENTRY_MODE_SET = 0x04,	// Sets cursor move direction and display shift
    CONTROL_SET = 0x08,	 	// Set display, cursor and blinking controls
    SHIFT_SET = 0x10,		// Set cursor and shifts display 
    FUNCTION_SET = 0x20,	// Sets interface data length, line and font.
    SET_CGRAM_ADDR = 0x40,	// Sets CGRAM address
    SET_CGRAM_MASK = 0x3f,	// Mask CGRAM address (6-bit)
    SET_DDRAM_ADDR = 0x80,	// Sets DDRAM address
    SET_DDRAM_MASK = 0x7f	// Mask DDRAM address (7-bit)
  } __attribute__((packed));

  /**
   * ENTRY_MODE_SET attributes
   */
  enum { 
    DISPLAY_SHIFT = 0x01,	// Shift the entire display not cursor
    INCREMENT = 0x02,		// Increment (right) on write
    DECREMENT = 0x00		// Decrement (left) on write
  } __attribute__((packed));

  /**
   * CONTROL_SET attributes
   */
  enum {
    BLINK_ON = 0x01,		// The character indicated by cursor blinks
    CURSOR_ON = 0x02,		// The cursor is displayed
    DISPLAY_ON = 0x04,		// The display is on
  } __attribute__((packed));

  /**
   * SHIFT_SET attributes
   */
  enum {
    MOVE_LEFT = 0x00,		// Moves cursor and shifts display
    MOVE_RIGHT = 0x04,		// without changing DDRAM contents
    CURSOR_MODE = 0x00,
    DISPLAY_MOVE = 0x08
  } __attribute__((packed));

  /**
   * FUNCTION_SET attributes
   */
  enum {
    DATA_LENGTH_4BITS = 0x00,	// Sets the interface data length, 4-bit or
    DATA_LENGTH_8BITS = 0x10,	// - 8-bit
    NR_LINES_1 = 0x00,		// Sets the number of display lines, 1 or
    NR_LINES_2 = 0x08,		// - 2.
    FONT_5X8DOTS = 0x00,	// Sets the character font, 5X8 dots or
    FONT_5X10DOTS = 0x04	// - 5X10 dots
  } __attribute__((packed));

  /**
   * @override
   * Write data or command to display.
   * @param[in] data to write.
   */
  void write(uint8_t data);
    
  /**
   * Set display attribute and update driver mirror variable.
   * @param[in,out] cmd command variable.
   * @param[in] mask function.
   */
  void set(uint8_t& cmd, uint8_t mask) 
  { 
    write(cmd |= mask); 
  }

  /**
   * Clear display attribute and update driver mirror variable.
   * @param[in,out] cmd command variable.
   * @param[in] mask function.
   */
  void clear(uint8_t& cmd, uint8_t mask) 
  { 
    write(cmd &= ~mask); 
  }

  /**
   * Set communication in data stream mode.
   */
  void set_data_mode();

  /**
   * Set communication in instruction stream mode.
   */
  void set_instruction_mode();

  // Display pins and state (mirror of device registers)
  IO* m_io;			// IO port handler
  uint8_t m_mode;		// Entry mode
  uint8_t m_cntl;		// Control
  uint8_t m_func;		// Function set

public:
  // Max size of custom character font bitmap
  static const uint8_t BITMAP_MAX = 8;
  
  // Display width (characters per line) and height (lines)
  const uint8_t WIDTH;
  const uint8_t HEIGHT;

  /**
   * Construct HD44780 LCD connected to given io port handler. The
   * display is initiated when calling begin(). 
   * @param[in] io handler.
   * @param[in] width of display, characters per line (Default 16).
   * @param[in] height of display, number of lines (Default 2).
   */
  HD44780(IO* io, uint8_t width = 16, uint8_t height = 2) :
    LCD::Device(),
    m_io(io),
    m_mode(ENTRY_MODE_SET | INCREMENT),
    m_cntl(CONTROL_SET),
    m_func(FUNCTION_SET | DATA_LENGTH_4BITS | NR_LINES_2 | FONT_5X8DOTS),
    WIDTH(width),
    HEIGHT(height)
  {
  }
  
  /**
   * @override
   * Start display for text output. Returns true if successful
   * otherwise false.
   * @return boolean.
   */
  virtual bool begin();

  /**
   * @override
   * Stop display and power down. Returns true if successful otherwise
   * false.
   */
  virtual bool end();

  /**
   * @override
   * Turn display backlight on. 
   */
  virtual void backlight_on();

  /**
   * @override
   * Turn display backlight off. 
   */
  virtual void backlight_off();

  /**
   * @override
   * Turn display on. 
   */
  virtual void display_on();

  /**
   * @override
   * Turn display off. 
   */
  virtual void display_off();

  /**
   * Set display scrolling left.
   */
  void display_scroll_left() 
  { 
    write(SHIFT_SET | DISPLAY_MOVE | MOVE_LEFT); 
  }

  /**
   * Set display scrolling right.
   */
  void display_scroll_right() 
  { 
    write(SHIFT_SET | DISPLAY_MOVE |  MOVE_RIGHT); 
  }
  
  /**
   * Clear display and move cursor to home.
   */
  virtual void display_clear();

  /**
   * Move cursor to home position.
   */
  void cursor_home();

  /**
   * Turn underline cursor on.
   */
  void cursor_underline_on() 
  { 
    set(m_cntl, CURSOR_ON);  
  }

  /**
   * Turn underline cursor off.
   */
  void cursor_underline_off() 
  { 
    clear(m_cntl, CURSOR_ON);  
  }

  /**
   * Turn cursor blink on.
   */
  void cursor_blink_on() 
  { 
    set(m_cntl, BLINK_ON); 
  }

  /**
   * Turn cursor blink off.
   */
  void cursor_blink_off() 
  { 
    clear(m_cntl, BLINK_ON); 
  }

  /**
   * @override
   * Set cursor position to given position.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

  /**
   * Set text flow left-to-right.
   */
  void text_flow_left_to_right() 
  { 
    set(m_mode, INCREMENT);
  }

  /**
   * Set text flow right-to-left.
   */
  void text_flow_right_to_left() 
  { 
    clear(m_mode, INCREMENT); 
  }

  /**
   * Set text scroll left adjust.
   */
  void text_scroll_left_adjust() 
  { 
    set(m_mode, DISPLAY_SHIFT); 
  }

  /**
   * Set text scroll right adjust.
   */
  void text_scroll_right_adjust() 
  { 
    clear(m_mode, DISPLAY_SHIFT); 
  }

  /**
   * Set custom character bitmap to given id (0..7). 
   * @param[in] id character.
   * @param[in] bitmap pointer to bitmap.
   */
  void set_custom_char(uint8_t id, const uint8_t* bitmap);

  /**
   * Set custom character bitmap to given id (0..7). 
   * The bitmap should be stored in program memory.
   * @param[in] id character.
   * @param[in] bitmap pointer to program memory bitmap.
   */
  void set_custom_char_P(uint8_t id, const uint8_t* bitmap);

  /**
   * @override
   * Write character to display. Handles carriage-return-line-feed, back-
   * space, alert, horizontal tab and form-feed. Returns character or EOF 
   * on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * IO Port. Arduino pins directly to LCD in 4-bit mode. Data port is 
   * implicitly defined (D4..D7).
   */
  class Port : public IO {
  private:
    OutputPin m_rs;		// Register select (0/instruction, 1/data)
    OutputPin m_en;		// Starts data read/write
    OutputPin m_bt;		// Back-light control (0/on, 1/off)

  public:
    /**
     * Construct HD44780 4-bit parallel port connected to given command
     * and enable pin. Data pins are implicit; D4..D7 for Arduino
     * Standard, Mighty and ATtinyX4. D10..D13 for Arduino/Mega. Connect
     * to LCD pins D4..D7.  
     * @param[in] rs command/data select pin (Default D8).
     * @param[in] en enable pin (Default D9).
     * @param[in] bt backlight pin (Default D10).
     */
    Port(Board::DigitalPin rs = Board::D8, 
	 Board::DigitalPin en = Board::D9,
	 Board::DigitalPin bt = Board::D10) :
      m_rs(rs, 0),
      m_en(en, 0),
      m_bt(bt, 1)
    {
    }

    /**
     * @override
     * Initiate 4-bit parallel port (D4..D7).
     */
    virtual void setup();

    /**
     * @override
     * Write LSB nibble to display using parallel port (D4..D7).
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);
    
    /**
     * @override
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

#if !defined(__ARDUINO_TINY__)
  /**
   * IO handler for HD44780 (LCD-II) Dot Matix Liquid Crystal Display
   * Controller/Driver when using the MJKDZ IO expander board based on
   * PCF8574. 
   */
  class MJKDZ : public IO, private PCF8574 {
  private:
    union {
      uint8_t as_uint8;		/**< Unsigned byte access */
      struct {
	uint8_t data:4;		/**< Data port (P0..P3) */
	uint8_t en:1;		/**< Enable/pulse (P4) */
	uint8_t rw:1;		/**< Read/Write (P5) */
	uint8_t rs:1;		/**< Command/Data select (P6) */
	uint8_t bt:1;		/**< Back-light (P7) */
      };
    } m_port;

  public:
    /**
     * Construct HD44780 IO port handler using the MJKDZ I2C/TWI
     * I/O expander with given sub-address (A0..A2).
     * @param[in] subaddr sub-address (0..7, default 7).
     */
    MJKDZ(uint8_t subaddr = 7) : 
      PCF8574(PCF8574::ADDR, subaddr)
    {
      m_port.as_uint8 = 0;
    }
    
    /**
     * @override
     * Initiate TWI interface.
     */
    virtual void setup();

    /**
     * @override
     * Write nibble to display using TWI interface.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override
     * Set instruction/data mode; zero for instruction, non-zero for
     * data mode. 
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };

  /**
   * IO handler for HD44780 (LCD-II) Dot Matix Liquid Crystal Display
   * Controller/Driver when using the DFRobot IO expander board based 
   * on PCF8574. 
   */
  class DFRobot : public IO, private PCF8574 {
  private:
    union {
      uint8_t as_uint8;
      struct {
	uint8_t rs:1;		/**< Command/Data select (P0) */
	uint8_t rw:1;		/**< Read/Write (P1) */
	uint8_t en:1;		/**< Enable/pulse (P2) */
	uint8_t bt:1;		/**< Back-light (P3) */
	uint8_t data:4;		/**< Data port (P4..P7) */
      };
    } m_port;

  public:
    /**
     * Construct HD44780 IO port handler using the DFRobot I2C/TWI
     * I/O expander with given sub-address (A0..A2).
     * @param[in] subaddr sub-address (0..7, default 7).
     */
    DFRobot(uint8_t subaddr = 7) : 
      PCF8574(PCF8574::ADDR, subaddr)
    {
      m_port.as_uint8 = 0;
    }
    
    /**
     * @override
     * Initiate TWI interface.
     */
    virtual void setup();

    /**
     * @override
     * Write nibble to display using TWI interface.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override
     * Set instruction/data mode; zero for instruction, non-zero for
     * data mode. 
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);
  };
#endif
};

#endif
