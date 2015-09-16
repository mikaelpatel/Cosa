/**
 * @file MAX72XX.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_MAX72XX_HH
#define COSA_MAX72XX_HH

#include "Cosa/LCD.hh"

/**
 * MAX72XX Serially Interfaced, 8-Digit LED Display Drivers, device
 * driver for IOStream access.
 *
 * The MAX7219/MAX7221 are compact, serial input/output common-cathode
 * display drivers that interface microprocessors (μPs) to 7-segment
 * numeric LED displays of up to 8 digits, bar-graph displays, or 64
 * individual LEDs. Included on-chip are a BCD code-B decoder,
 * multiplex scan circuitry, segment and digit drivers, and an 8x8
 * static RAM that stores each digit.
 *
 * @section Circuit
 * @code
 *                    MAX72XX/LCD::Serial3W
 *                       +------------+
 * (D6/D0)-------------1-|DIN         |
 * (GND)---------------4-|GND         |
 * (GND)---------------9-|GND         |
 * (D9/D3)------------12-|CS          |
 * (D7/D1)------------13-|CLK         |
 * (VCC)--------------19-|VCC         |
 *                       +------------+
 *
 *                       MAX72XX/LCD::SPI3W
 *                       +------------+
 * (MOSI/D11/D5)-------1-|DIN         |
 * (GND)---------------4-|GND         |
 * (GND)---------------9-|GND         |
 * (D9/D3)------------12-|CS          |
 * (SCK/D13/D4)-------13-|CLK         |
 * (VCC)--------------19-|VCC         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Product Specification, MAXIM, MAX7219/7221, 19-5542, Rev.4, 7/03.
 * http://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf
 */
class MAX72XX : public LCD::Device {
public:
  /** Display size. */
  static const uint8_t WIDTH = 8;
  static const uint8_t HEIGHT = 1;
  static const uint8_t LINES = 1;

  /**
   * Construct display device driver with given io adapter and font.
   * The font should be a character to segment mapping table in
   * program memory for character codes SPACE(0x20) to DEL(0x7f).
   * @param[in] io adapter, SPI or in/output pin based.
   * @param[in] font program memory (Default NULL).
   */
  MAX72XX(LCD::IO* io, const uint8_t* font = NULL);

  /**
   * @override{LCD::Device}
   * Start interaction with display. Turns display on, clears and sets
   * the contrast/intensity to mid-level(7).
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin();

  /**
   * @override{LCD::Device}
   * Stop sequence of interaction with device.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end();

  /**
   * @override{LCD::Device}
   * Set display contrast/intensity level (0..15).
   * @param[in] contrast level.
   */
  virtual void display_contrast(uint8_t level);

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
   * @override{LCD::Device}
   * Clear display and move cursor to home (0, 0).
   */
  virtual void display_clear();

  /**
   * @override{LCD::Device}
   * Set cursor to given position.
   * @param[in] x pixel position (0..WIDTH-1).
   * @param[in] y line position (0..LINES-1).
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

  /**
   * @override{IOStream::Device}
   * Write character to display. Handles carriage-return-line-feed,
   * backspace, alert, horizontal tab and form-feed. The period
   * character is translated to the 7-segment LED decimal point of the
   * previous written character. Returns character or EOF on error.
   * @param[in] c character to write.
   * @return character written or EOF(-1).
   */
  virtual int putchar(char c);

protected:
  /**
   * Register Address Map (Table 2, pp 7).
   */
  enum Register {
    NOP = 0x00,			//!< No-operation.
    DIGIT0 = 0x01,		//!< Digit 0 (encode or segment data).
    DIGIT1 = 0x02,		//!< Digit 1 (encode or segment data).
    DIGIT2 = 0x03,		//!< Digit 2 (encode or segment data).
    DIGIT3 = 0x04,		//!< Digit 3 (encode or segment data).
    DIGIT4 = 0x05,		//!< Digit 4 (encode or segment data).
    DIGIT5 = 0x06,		//!< Digit 5 (encode or segment data).
    DIGIT6 = 0x07,		//!< Digit 6 (encode or segment data).
    DIGIT7 = 0x08,		//!< Digit 7 (encode or segment data).
    DECODE_MODE = 0x09,		//!< Decode Mode (0..255, digit bitset).
    INTENSITY = 0x0a,		//!< Intensity (0..15, level).
    SCAN_LIMIT = 0x0b,		//!< Scan Limit (0..7, digits 1..8).
    DISPLAY_MODE = 0x0c,	//!< Display Mode (shutdown, normal).
    DISPLAY_TEST = 0x0f		//!< Display Test (0..1, on/off).
  } __attribute__((packed));

  /**
   * Shutdown Register Format (Table 3, pp. 7).
   */
  enum {
    SHUTDOWN_MODE = 0x00,	//!< Shutdown mode.
    NORMAL_OPERATION = 0x01	//!< Normal operation.
  } __attribute__((packed));

  /**
   * Decode-Mode Register (Table 4, pp. 7).
   */
  enum {
    NO_DECODE = 0x00,		//!< No decode for digits 7-0.
    ALL_DECODE = 0xff		//!< Code B decode for digits 7-0.
  } __attribute__((packed));

  /** Default font. */
  static const uint8_t font[] PROGMEM;

  /** Display pins and state. */
  LCD::IO* m_io;		//!< Display adapter.
  const uint8_t* m_font;	//!< Font in program memory.
  char m_latest;		//!< Latest character code.

  /**
   * Set register to the given value.
   * @param[in] reg register address.
   * @param[in] value.
   */
  void set(Register reg, uint8_t value);
};

#endif
