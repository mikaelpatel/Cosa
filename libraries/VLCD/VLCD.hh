/**
 * @file VLCD.hh
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

#ifndef COSA_VLCD_HH
#define COSA_VLCD_HH

#include "Cosa/LCD.hh"
#include "Cosa/TWI.hh"

/**
 * Virtual Dot Matix Liquid Crystal Display Controller/Driver
 * for LCD/IOStream access. Acts as a proxy for an LCD slave.
 */
class VLCD : public LCD::Device, private TWI::Driver {
public:
  // Display protocol version (valid after initialization, begin())
  uint8_t MAJOR;
  uint8_t MINOR;

  // Display width (characters per line) and height (lines)
  uint8_t WIDTH;
  uint8_t HEIGHT;

  /**
   * Construct Virtual LCD connected to given TWI address.
   * @param[in] addr address of Virtual LCD (Default 0x5a);
   */
  VLCD(uint8_t addr = 0x5a) :
    LCD::Device(),
    TWI::Driver(addr),
    MAJOR(0),
    MINOR(0),
    WIDTH(0),
    HEIGHT(0)
  {}

  /**
   * @override{LCD::Device}
   * Start display for text output. Initiate display and retrieve
   * version and dimension information (MAJOR/MINOR and WIDTH/HEIGHT).
   * Returns true if successful otherwise false.
   * @return boolean.
   */
  virtual bool begin();

  /**
   * @override{LCD::Device}
   * Stop display and power down. Returns true if successful otherwise
   * false.
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
   * @override{LCD::Device}
   * Clear display and move cursor to home.
   */
  virtual void display_clear();

  /**
   * @override{LCD::Device}
   * Set cursor position to given position.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

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

  /**
   * Virtual LCD Slave device
   */
  class Slave : public TWI::Slave {
    friend class VLCD;
  public:
    // Max size of buffer
    static const uint8_t BUF_MAX = 40;

    /**
     * Construct Virtual LCD Slave with given LCD and TWI address.
     * @param[in] lcd implementation.
     * @param[in] addr TWI address (default 0x5a).
     */
    Slave(LCD::Device* lcd, uint8_t addr = 0x5a) :
      TWI::Slave(addr),
      m_lcd(lcd)
    {
      write_buf(m_buf, sizeof(m_buf));
      read_buf(m_buf, sizeof(m_buf));
    }

    /**
     * @override{TWI::Slave}
     * Slave request handler; parse and dispatch LCD functions.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    virtual void on_request(void* buf, size_t size);

  private:
    // Version code
    static const uint8_t MAJOR = 1;
    static const uint8_t MINOR = 0;

    // Command prefix byte
    static const uint8_t COMMAND = 255;

    // Command codes
    enum {
      BACKLIGHT_OFF_CMD = 0,
      BACKLIGHT_ON_CMD,
      DISPLAY_OFF_CMD,
      DISPLAY_ON_CMD,
      INIT_CMD = 0xff
    } __attribute__((packed));

    // Buffer for transactions (max string write)
    uint8_t m_buf[BUF_MAX];

    // The actual LCD implementation
    LCD::Device* m_lcd;
  };

private:
  // Version information
  struct info_t {
    uint8_t major;
    uint8_t minor;
    uint8_t width;
    uint8_t height;
  };

  /**
   * Write given command to slave device.
   * @param[in] cmd command.
   */
  void write(uint8_t cmd);
};

#endif
