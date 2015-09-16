/**
 * @file HD44780.hh
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

#ifndef COSA_HD44780_HH
#define COSA_HD44780_HH

#include "Cosa/SPI.hh"
#include "Cosa/LCD.hh"
#include "Cosa/OutputPin.hh"

/**
 * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
 * for LCD/IOStream access. Binding to trace, etc. Supports simple text
 * scroll, cursor, and handling of special characters such as carriage-
 * return, form-feed, back-space, horizontal tab and new-line.
 *
 * @section References
 * 1. Product Specification, Hitachi, HD4478U, ADE-207-272(Z), '99.9, Rev. 0.0.
 */
class HD44780 : public LCD::Device {
public:
  /**
   * Abstract HD44780 LCD IO adapter to isolate communication specific
   * functions and allow access over parallel and serial interfaces;
   * Ports, SR and I2C/TWI.
   */
  class IO {
  public:
    /**
     * @override{HD44780::IO}
     * Initiate IO port. Called by HD44780::begin(). Should return true(1)
     * for 8-bit mode otherwise false for 4-bit mode.
     * @return bool.
     */
    virtual bool setup() = 0;

    /**
     * @override{HD44780::IO}
     * Write LSB nibble (4bit) to display.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data) = 0;

    /**
     * @override{HD44780::IO}
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Write character buffer to display.
     * @param[in] buf pointer to buffer.
     * @param[in] size number of bytes in buffer.
     */
    virtual void write8n(const void* buf, size_t size);

    /**
     * @override{HD44780::IO}
     * Set data/command mode; zero(0) for command,
     * non-zero(1) for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag) = 0;

    /**
     * @override{HD44780::IO}
     * Set backlight on/off.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag) = 0;
  };

  /** Max size of custom character font bitmap. */
  static const uint8_t BITMAP_MAX = 8;

  /** Display width (characters per line). */
  const uint8_t WIDTH;

  /** Display height (lines). */
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
    WIDTH(width),
    HEIGHT(height),
    m_io(io),
    m_mode(ENTRY_MODE_SET | INCREMENT),
    m_cntl(CONTROL_SET),
    m_func(FUNCTION_SET | DATA_LENGTH_4BITS | NR_LINES_2 | FONT_5X8DOTS),
    m_offset((height == 4) && (width == 16) ? offset1 : offset0)
  {}

  /**
   * @override{LCD::Device}
   * Start display for text output. Returns true if successful
   * otherwise false.
   * @return boolean.
   */
  virtual bool begin();

  /**
   * @override{LCD::Device}
   * Stop display and power down. Returns true if successful
   * otherwise false.
   */
  virtual bool end();

  /**
   * @override{LCD::Device}
   * Turn display backlight on.
   */
  virtual void backlight_on();

  /**
   * @override{LCD::Device}
   * Turn display backlight off.
   */
  virtual void backlight_off();

  /**
   * @override{LCD::Device}
   * Turn display on.
   */
  virtual void display_on();

  /**
   * @override{LCD::Device}
   * Turn display off.
   */
  virtual void display_off();

  /**
   * Set display scrolling left.
   */
  void display_scroll_left()
    __attribute__((always_inline))
  {
    write(SHIFT_SET | DISPLAY_MOVE | MOVE_LEFT);
  }

  /**
   * Set display scrolling right.
   */
  void display_scroll_right()
    __attribute__((always_inline))
  {
    write(SHIFT_SET | DISPLAY_MOVE |  MOVE_RIGHT);
  }

  /**
   * @override{LCD::Device}
   * Clear display and move cursor to home(0, 0).
   */
  virtual void display_clear();

  /**
   * Clear to end of line.
   */
  void line_clear()
  {
    while (m_x < WIDTH) putchar(' ');
  }

  /**
   * Move cursor to home position(0, 0) .
   */
  void cursor_home();

  /**
   * Turn underline cursor on.
   */
  void cursor_underline_on()
    __attribute__((always_inline))
  {
    set(m_cntl, CURSOR_ON);
  }

  /**
   * Turn underline cursor off.
   */
  void cursor_underline_off()
    __attribute__((always_inline))
  {
    clear(m_cntl, CURSOR_ON);
  }

  /**
   * Turn cursor blink on.
   */
  void cursor_blink_on()
    __attribute__((always_inline))
  {
    set(m_cntl, BLINK_ON);
  }

  /**
   * Turn cursor blink off.
   */
  void cursor_blink_off()
    __attribute__((always_inline))
  {
    clear(m_cntl, BLINK_ON);
  }

  /**
   * @override{LCD::Device}
   * Set cursor position to given position.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

  /**
   * Set text flow left-to-right.
   */
  void text_flow_left_to_right()
    __attribute__((always_inline))
  {
    set(m_mode, INCREMENT);
  }

  /**
   * Set text flow right-to-left.
   */
  void text_flow_right_to_left()
    __attribute__((always_inline))
  {
    clear(m_mode, INCREMENT);
  }

  /**
   * Set text scroll left adjust.
   */
  void text_scroll_left_adjust()
    __attribute__((always_inline))
  {
    set(m_mode, DISPLAY_SHIFT);
  }

  /**
   * Set text scroll right adjust.
   */
  void text_scroll_right_adjust()
    __attribute__((always_inline))
  {
    clear(m_mode, DISPLAY_SHIFT);
  }

  /**
   * Set custom character bitmap for given identity (0..7).
   * @param[in] id character.
   * @param[in] bitmap pointer to bitmap.
   */
  void set_custom_char(uint8_t id, const uint8_t* bitmap);

  /**
   * Set custom character bitmap to given identity (0..7).
   * The bitmap should be stored in program memory.
   * @param[in] id character.
   * @param[in] bitmap pointer to program memory bitmap.
   */
  void set_custom_char_P(uint8_t id, const uint8_t* bitmap);

  /**
   * @override{IOStream::Device}
   * Write character to display. Handles carriage-return-line-feed, back-
   * space, alert, horizontal tab and form-feed. Returns character or EOF
   * on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

  /** Overloaded virtual member function write. */
  using IOStream::Device::write;

  /**
   * @override{IOStream::Device}
   * Write data from buffer with given size to device.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write(const void* buf, size_t size);

#if !defined(BOARD_ATTINYX5)
  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * IO Port. Arduino pins directly to LCD in 4-bit mode.
   *
   * @section Circuit
   * @code
   *                           HD44780
   *                       +------------+
   * (GND)---------------1-|VSS         |
   * (VCC)---------------2-|VDD         |
   *                     3-|VO          |
   * (D8)----------------4-|RS          |
   *                     5-|RW          |
   * (D9)----------------6-|EN          |
   *                     7-|D0          |
   *                     8-|D1          |
   *                     9-|D2          |
   *                    10-|D3          |
   * (D4/D0)------------11-|D4          |
   * (D5/D1)------------12-|D5          |
   * (D6/D2)------------13-|D6          |
   * (D7/D3)------------14-|D7          |
   * (VCC)-+------------15-|A           |
   *       ¡         +--16-|K           |
   *     [4K7]       |     +------------+
   *       |         |
   * (D10)-+-[10K]-|< NPN 9013
   * (/BT)           v
   *                 |
   * (GND)-----------+
   *
   * @endcode
   * The default pins are the LCD Keypad Shield.
   *
   * @section Limitations
   * Requires too many pins for ATtinyX5.
   */
  class Port4b : public IO {
  public:
    /**
     * Construct HD44780 4-bit parallel port connected to given command,
     * enable and backlight pin. Data pins are implicit; D4..D7 for Arduino
     * Standard and Mighty. D0..D3 for ATtinyX4. Connect to LCD pins D4..D7.
     * @param[in] d0 data pin (Default D4).
     * @param[in] d1 data pin (Default D5).
     * @param[in] d2 data pin (Default D6).
     * @param[in] d3 data pin (Default D7).
     * @param[in] rs command/data select pin (Default D8).
     * @param[in] en enable pin (Default D9).
     * @param[in] bt backlight pin (Default D10).
     */
    Port4b(Board::DigitalPin d0 = Board::D4,
	   Board::DigitalPin d1 = Board::D5,
	   Board::DigitalPin d2 = Board::D6,
	   Board::DigitalPin d3 = Board::D7,
	   Board::DigitalPin rs = Board::D8,
	   Board::DigitalPin en = Board::D9,
	   Board::DigitalPin bt = Board::D10) :
      m_d0(d0, 0),
      m_d1(d1, 0),
      m_d2(d2, 0),
      m_d3(d3, 0),
      m_rs(rs, 0),
      m_en(en, 0),
      m_bt(bt, 1)
    {}

    /**
     * @override{HD44780::IO}
     * Initiate 4-bit parallel port. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override{HD44780::IO}
     * Write LSB nibble to display data pins.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override{HD44780::IO}
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);

  protected:
    /** Execution time delay (us). */
    static const uint16_t SHORT_EXEC_TIME = 32;

    OutputPin m_d0;		//!< Data pin; d0.
    OutputPin m_d1;		//!< Data pin; d1.
    OutputPin m_d2;		//!< Data pin; d2.
    OutputPin m_d3;		//!< Data pin; d3.
    OutputPin m_rs;		//!< Register select (0/instruction, 1/data).
    OutputPin m_en;		//!< Starts data read/write.
    OutputPin m_bt;		//!< Back-light control (0/on, 1/off).
  };
#endif

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * Shift Register 3-Wire Port (SR3W), 74HC595/74HC164 (SR[pin]),
   * with digital output pins.
   *
   * @section Circuit
   * @code
   *                         74HC595    (VCC)
   *                       +----U----+    |
   * (LCD D5)------------1-|Q1    VCC|-16-+
   * (LCD D6)------------2-|Q2     Q0|-15-----------(LCD D4)
   * (LCD D7)------------3-|Q3    SER|-14-----------(SDA/D7)
   * (LCD RS)------------4-|Q4    /OE|-13-----------(GND)
   * (LCD BT)------------5-|Q5   RCLK|-12-----------(EN/D5)
   *                     6-|Q6   SCLK|-11-----------(SCL/D6)
   *                     7-|Q7    /MR|-10-----------(VCC)
   *                   +-8-|GND   Q6'|--9
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   * (LCD EN)---------------------------------------(EN/D5)
   * (LCD RW)---------------------------------------(GND)
   * (LCD K)----------------------------------------(GND)
   * (LCD A)-----------------[330]------------------(VCC)
   *
   * @endcode
   *
   * Alternative circuit with 74HC164.
   *
   * @code
   *                         74HC164    (VCC)
   *                       +----U----+    |
   * (SDA/D7)----------+-1-|DSA   VCC|-14-+
   *                   +-2-|DSB    Q7|-13
   * (LCD D4)------------3-|Q0     Q6|-12
   * (LCD D5)------------4-|Q1     Q5|-11--------(LCD BT)
   * (LCD D6)------------5-|Q2     Q4|-10--------(LCD RS)
   * (LCD D7)------------6-|Q3    /MR|--9-----------(VCC)
   *                   +-7-|GND    CP|--8--------(SCL/D6)
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   * (LCD EN)---------------------------------------(EN/D5)
   * (LCD RW)---------------------------------------(GND)
   * (LCD K)----------------------------------------(GND)
   * (LCD A)-----------------[330]------------------(VCC)
   *
   * @endcode
   *
   * @section Performance
   * The LSB of the shift register is used to allow reduction
   * of number of shift operations (i.e. 6-bit shift).
   */
  class SR3W : public IO {
  public:
    /**
     * Construct HD44780 3-wire serial port connected to given serial
     * data, clock and enable pulse pin.
     * @param[in] sda serial data pin (Default D7, Tiny/D1)
     * @param[in] scl serial clock pin (Default D6, Tiny/D2)
     * @param[in] en enable pulse (Default D5, Tiny/D3)
     */
#if !defined(BOARD_ATTINY)
    SR3W(Board::DigitalPin sda = Board::D7,
	 Board::DigitalPin scl = Board::D6,
	 Board::DigitalPin en = Board::D5);
#else
    SR3W(Board::DigitalPin sda = Board::D1,
	 Board::DigitalPin scl = Board::D2,
	 Board::DigitalPin en = Board::D3);
#endif

    /**
     * @override{HD44780::IO}
     * Initiate serial port. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override{HD44780::IO}
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override{HD44780::IO}
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);

  protected:
    /** Execution time delay (us). */
    static const uint16_t SHORT_EXEC_TIME = 20;

    /** Shift register port bit fields; little endian. */
    union port_t {
      uint8_t as_uint8;		//!< Unsigned byte access.
      struct {
	uint8_t data:4;		//!< Data port (Q0..Q3).
	uint8_t rs:1;		//!< Command/Data select (Q4).
	uint8_t bt:1;		//!< Back-light control (Q5).
	uint8_t app2:1;		//!< Application bit#2 (Q6).
	uint8_t app1:1;		//!< Application bit#1 (Q7).
      };
      operator uint8_t()
      {
	return (as_uint8);
      }
      port_t()
      {
	as_uint8 = 0;
      }
    };
    port_t m_port;		//!< Port setting.
    OutputPin m_sda;		//!< Serial data output.
    OutputPin m_scl;		//!< Serial clock.
    OutputPin m_en;		//!< Starts data read/write.
  };

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * Shift Register 3-Wire Port using SPI (SR3WSPI), 74HC595 (SR[pin]).
   * The enable pulse pin acts as the SPI chip select.
   *
   * @section Circuit
   * @code
   *                         74HC595    (VCC)
   *                       +----U----+    |
   * (LCD D5)------------1-|Q1    VCC|-16-+
   * (LCD D6)------------2-|Q2     Q0|-15-----------(LCD D4)
   * (LCD D7)------------3-|Q3    SER|-14-----------(MOSI/D11)
   * (LCD RS)------------4-|Q4    /OE|-13-----------(GND)
   * (LCD BT)------------5-|Q5   RCLK|-12-----------(EN/D5)
   *                     6-|Q6   SCLK|-11-----------(SCK/D13)
   *                     7-|Q7    /MR|-10-----------(VCC)
   *                   +-8-|GND   Q6'|--9
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   * (LCD EN)---------------------------------------(EN/D5)
   * (LCD RW)---------------------------------------(GND)
   * (LCD K)----------------------------------------(GND)
   * (LCD A)-----------------[330]------------------(VCC)
   * @endcode
   *
   * Alternative circuit with 74HC164.
   *
   * @code
   *                         74HC164    (VCC)
   *                       +----U----+    |
   * (D11/MOSI)--------+-1-|DSA   VCC|-14-+
   *                   +-2-|DSB    Q7|-13
   * (LCD D4)------------3-|Q0     Q6|-12
   * (LCD D5)------------4-|Q1     Q5|-11--------(LCD BT)
   * (LCD D6)------------5-|Q2     Q4|-10--------(LCD RS)
   * (LCD D7)------------6-|Q3    /MR|--9-----------(VCC)
   *                   +-7-|GND    CP|--8-------(SCK/D13)
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   * (LCD EN)---------------------------------------(EN/D5)
   * (LCD RW)---------------------------------------(GND)
   * (LCD K)----------------------------------------(GND)
   * (LCD A)-----------------[330]------------------(VCC)
   *
   * @endcode
   *
   * @section Performance
   * The SPI transfer is so fast that a longer delay is required.
   */
  class SR3WSPI : public IO, public SPI::Driver {
  public:
    /**
     * Construct HD44780 4-wire serial port connected to given enable
     * and chip select pin. Uses the SPI::MOSI(D11) and SPI:SCK(D13) pins.
     * @param[in] en enable pulse (Default D5, Tiny/D3)
     */
#if !defined(BOARD_ATTINY)
    SR3WSPI(Board::DigitalPin en = Board::D5);
#else
    SR3WSPI(Board::DigitalPin en = Board::D3);
#endif

    /**
     * @override{HD44780::IO}
     * Initiate serial port. Returns false.
     * @return bool.
     */
    virtual bool setup();

    /**
     * @override{HD44780::IO}
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override{HD44780::IO}
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);

  protected:
    /** Execution time delay (us). */
#if !defined(BOARD_ATTINY)
    static const uint16_t SHORT_EXEC_TIME = 24;
#else
    static const uint16_t SHORT_EXEC_TIME = 8;
#endif

    /** Shift register port bit fields; little endian. */
    union port_t {
      uint8_t as_uint8;		//!< Unsigned byte access.
      struct {
	uint8_t data:4;		//!< Data port (Q0..Q3).
	uint8_t rs:1;		//!< Command/Data select (Q4).
	uint8_t bt:1;		//!< Back-light control (Q5).
	uint8_t app2:1;		//!< Application bit#2 (Q6).
	uint8_t app1:1;		//!< Application bit#1 (Q7).
      };
      operator uint8_t()
      {
	return (as_uint8);
      }
      port_t()
      {
	as_uint8 = 0;
      }
    };
    port_t m_port;		//!< Port setting.
  };

  /**
   * HD44780 (LCD-II) Dot Matix Liquid Crystal Display Controller/Driver
   * Shift Register 4-Wire/8-bit Port, 74HC595 (SR[pin]), with digital
   * output pins.
   *
   * @section Circuit
   * @code
   *                         74HC595    (VCC)
   *                       +----U----+    |
   * (LCD D1)------------1-|Q1    VCC|-16-+
   * (LCD D2)------------2-|Q2     Q0|-15-----------(LCD D0)
   * (LCD D3)------------3-|Q3    /OE|-13-----------(GND)
   * (LCD D4)------------4-|Q4    SER|-14-----------(SDA/D7)
   * (LCD D5)------------5-|Q5   RCLK|-12-----------(EN/D5)
   * (LCD D6)------------6-|Q6   SCLK|-11-----------(SCL/D6)
   * (LCD D7)------------7-|Q7    /MR|-10-----------(VCC)
   *                   +-8-|GND   Q6'|-9
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   *
   * (LCD RS)---------------------------------------(SDA/D7)
   * (LCD EN)---------------------------------------(EN/D5)
   * (LCD BT)---------------------------------------(BT/D4)
   * (LCD RW)---------------------------------------(GND)
   * (LCD K)----------------------------------------(GND)
   * (LCD A)-----------------[330]------------------(VCC)
   * @endcode
   *
   * Alternative circuit with 74HC164.
   *
   * @code
   *                         74HC164    (VCC)
   *                       +----U----+    |
   * (D7/SDA)----------+-1-|DSA   VCC|-14-+
   *                   +-2-|DSB    Q7|-13--------(LCD D7)
   * (LCD D4)------------3-|Q0     Q6|-12--------(LCD D6)
   * (LCD D5)------------4-|Q1     Q5|-11--------(LCD D5)
   * (LCD D6)------------5-|Q2     Q4|-10--------(LCD D4)
   * (LCD D7)------------6-|Q3    /MR|--9-----------(VCC)
   *                   +-7-|GND    CP|--8--------(SCL/D6)
   *                   |   +---------+
   *                   |      0.1uF
   *                 (GND)-----||----(VCC)
   *
   * (LCD RS)---------------------------------------(SDA/D7)
   * (LCD EN)---------------------------------------(EN/D5)
   * (LCD BT)---------------------------------------(BT/D4)
   * (LCD RW)---------------------------------------(GND)
   * (LCD K)----------------------------------------(GND)
   * (LCD A)-----------------[330]------------------(VCC)
   * @endcode
   *
   * @section Performance
   * Delay required even when using Cosa serial write. No
   * need for SPI. SCL/SDA can still be connected to other
   * inputs. The Backlight control pin (BT) can be removed
   * if always on.
   *
   * @section Acknowledgement
   * Inspired by AVR2LCD, a solution by Frank Henriquez. The original
   * 74HC164 based design is by Stefan Heinzmann and Marc Simons.
   * http://frank.bol.ucla.edu/avr2lcd.htm
   * http://web.archive.org/web/20100210142839/
   * http://home.iae.nl/users/pouweha/lcd/lcd_examp.shtml#_3
   */
  class SR4W : public IO {
  public:
    /**
     * Construct HD44780 4-wire/8-bit serial port connected to given
     * data, clock, enable and backlight control pins.
     * @param[in] sda serial data pin (Default D7, Tiny/D1)
     * @param[in] scl serial clock pin (Default D6, Tiny/D2)
     * @param[in] en enable pulse (Default D5, Tiny/D3)
     * @param[in] bt backlight control (Default D4, Tiny/D4)
     */
#if !defined(BOARD_ATTINY)
    SR4W(Board::DigitalPin sda = Board::D7,
	 Board::DigitalPin scl = Board::D6,
	 Board::DigitalPin en = Board::D5,
	 Board::DigitalPin bt = Board::D4);
#else
    SR4W(Board::DigitalPin sda = Board::D1,
	 Board::DigitalPin scl = Board::D2,
	 Board::DigitalPin en = Board::D3,
	 Board::DigitalPin bt = Board::D4);
#endif

    /**
     * @override{HD44780::IO}
     * Initiate port for 8-bit serial mode. Returns true(1).
     * @return true(1).
     */
    virtual bool setup();

    /**
     * @override{HD44780::IO}
     * Write LSB nibble to display using serial port.
     * @param[in] data (4b) to write.
     */
    virtual void write4b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Write byte (8bit) to display.
     * @param[in] data (8b) to write.
     */
    virtual void write8b(uint8_t data);

    /**
     * @override{HD44780::IO}
     * Set instruction/data mode using given rs pin; zero for
     * instruction, non-zero for data mode.
     * @param[in] flag.
     */
    virtual void set_mode(uint8_t flag);

    /**
     * @override{HD44780::IO}
     * Set backlight on/off using bt pin.
     * @param[in] flag.
     */
    virtual void set_backlight(uint8_t flag);

  protected:
    /** Execution time delay (us). */
    static const uint16_t SHORT_EXEC_TIME = 16;

    OutputPin m_sda;		//!< Serial data output.
    OutputPin m_scl;		//!< Serial clock.
    OutputPin m_en;		//!< Starts data read/write.
    OutputPin m_bt;		//!< Backlight control.
    uint8_t m_rs;		//!< Command/Data select.
  };

  /**
   * Bus Timing Characteristics (in micro-seconds), fig. 25, pp. 50.
   */
  static const uint16_t LONG_EXEC_TIME = 1600;
  static const uint16_t POWER_ON_TIME = 48;
  static const uint16_t INIT0_TIME = 4500;
  static const uint16_t INIT1_TIME = 150;

  /**
   * Instructions (Table 6, pp. 24), RS(0), RW(0).
   */
  enum {
    CLEAR_DISPLAY = 0x01,    	//!< Clears entrire display and return home.
    RETURN_HOME = 0x02,	     	//!< Sets DDRAM 0 in address counter.
    ENTRY_MODE_SET = 0x04,	//!< Sets cursor move direction and display shift.
    CONTROL_SET = 0x08,	 	//!< Set display, cursor and blinking controls.
    SHIFT_SET = 0x10,		//!< Set cursor and shifts display.
    FUNCTION_SET = 0x20,	//!< Sets interface data length, line and font.
    SET_CGRAM_ADDR = 0x40,	//!< Sets CGRAM address.
    SET_CGRAM_MASK = 0x3f,	//!< - Mask (6-bit).
    SET_DDRAM_ADDR = 0x80,	//!< Sets DDRAM address.
    SET_DDRAM_MASK = 0x7f,	//!< - Mask (7-bit).
    BIAS_RESISTOR_SET = 0x04,	//!< Bias resistor select.
    BIAS_RESISTOR_MASK = 0x03,	//!< - Mask (2-bit).
    COM_SEG_SET = 0x40,		//!< COM SEG direction select.
    COM_SET_MASK = 0x0f,	//!< - mask (4 bit).
    SET_DDATA_LENGTH = 0x80,	//!< Set display data length.
    SET_DDATA_MASK = 0x7f	//!< - mask (7 bit, 0..79 => 1..80).
  } __attribute__((packed));

  /**
   * ENTRY_MODE_SET attributes.
   */
  enum {
    DISPLAY_SHIFT = 0x01,	//!< Shift the entire display not cursor.
    INCREMENT = 0x02,		//!< Increment (right) on write.
    DECREMENT = 0x00		//!< Decrement (left) on write.
  } __attribute__((packed));

  /**
   * CONTROL_SET attributes.
   */
  enum {
    BLINK_ON = 0x01,		//!< The character indicated by cursor blinks.
    CURSOR_ON = 0x02,		//!< The cursor is displayed.
    DISPLAY_ON = 0x04,		//!< The display is on.
  } __attribute__((packed));

  /**
   * SHIFT_SET attributes.
   */
  enum {
    MOVE_LEFT = 0x00,		//!< Moves cursor and shifts display.
    MOVE_RIGHT = 0x04,		//!< without changing DDRAM contents.
    CURSOR_MODE = 0x00,
    DISPLAY_MOVE = 0x08
  } __attribute__((packed));

  /**
   * FUNCTION_SET attributes.
   */
  enum {
    DATA_LENGTH_4BITS = 0x00,	//!< Sets the interface data length, 4-bit or.
    DATA_LENGTH_8BITS = 0x10,	//!< - 8-bit.
    NR_LINES_1 = 0x00,		//!< Sets the number of display lines, 1 or.
    NR_LINES_2 = 0x08,		//!< - 2.
    FONT_5X8DOTS = 0x00,	//!< Sets the character font, 5X8 dots or.
    FONT_5X10DOTS = 0x04,	//!< - 5X10 dots.
    BASIC_SET = 0x00,		//!< Sets basic instruction set.
    EXTENDED_SET = 0x04		//!< - extended instruction set.
  } __attribute__((packed));

  /** Row offset tables for display dimensions (16X1, 16X2, 16X4, 20X4). */
  static const uint8_t offset0[] PROGMEM;
  static const uint8_t offset1[] PROGMEM;

  /** Display pins and state (mirror of device registers). */
  IO* m_io;			//!< IO port handler.
  uint8_t m_mode;		//!< Entry mode.
  uint8_t m_cntl;		//!< Control.
  uint8_t m_func;		//!< Function set.
  const uint8_t* m_offset;	//!< Row offset table.

  /**
   * Write data or command to display.
   * @param[in] data to write.
   */
  void write(uint8_t data)
    __attribute__((always_inline))
  {
    m_io->write8b(data);
  }

  /**
   * Set display attribute and update driver mirror variable.
   * @param[in,out] cmd command variable.
   * @param[in] mask function.
   */
  void set(uint8_t& cmd, uint8_t mask)
    __attribute__((always_inline))
  {
    m_io->write8b(cmd |= mask);
  }

  /**
   * Clear display attribute and update driver mirror variable.
   * @param[in,out] cmd command variable.
   * @param[in] mask function.
   */
  void clear(uint8_t& cmd, uint8_t mask)
    __attribute__((always_inline))
  {
    m_io->write8b(cmd &= ~mask);
  }

  /**
   * Set communication in data stream mode.
   */
  void set_data_mode()
    __attribute__((always_inline))
  {
    m_io->set_mode(1);
  }

  /**
   * Set communication in instruction stream mode.
   */
  void set_instruction_mode()
    __attribute__((always_inline))
  {
    m_io->set_mode(0);
  }
};
#endif
