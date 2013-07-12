/**
 * @file Cosa/VLCD.hh
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

#ifndef __COSA_VLCD_HH__
#define __COSA_VLCD_HH__

#include "Cosa/LCD.hh"
#include "Cosa/TWI.hh"

/**
 * Virtual Dot Matix Liquid Crystal Display Controller/Driver
 * for LCD/IOStream access. Acts as a proxy for an LCD slave.
 */
class VLCD : public LCD::Device {
private:
  // Max size of program memory string
  static const uint8_t BUF_MAX = 40;

  /**
   * Write given command to slave device.
   * @param[in] cmd command.
   */
  void write(uint8_t cmd);

public:
  /**
   * Virtual LCD Slave device
   */
  class Slave : public TWI::Slave {
    friend class VLCD;
  private:
    // Command prefix byte
    static const uint8_t COMMAND = 0;

    // Command codes
    enum {
      BACKLIGHT_OFF_CMD = 0,
      BACKLIGHT_ON_CMD,
      DISPLAY_OFF_CMD,
      DISPLAY_ON_CMD
    } __attribute__((packed));

    // Buffer for transaction
    static const uint8_t BUF_MAX = 64;
    uint8_t m_buf[BUF_MAX];

    // The actual LCD implementation
    LCD::Device* m_lcd;
    
  public:
    /**
     * Construct Virtual LCD Slave with given LCD and TWI address.
     * @param[in] lcd implementation.
     * @param[in] addr TWI address (default 0x5a).
     */
    Slave(LCD::Device* lcd, uint8_t addr = 0x5a) : 
      TWI::Slave(addr), 
      m_lcd(lcd)
    {
      set_write_buf(m_buf, sizeof(m_buf));
      set_read_buf(m_buf, sizeof(m_buf));
    }

    /**
     * Slave request handler; parse and dispatch LCD functions.
     * @param[in] buf buffer pointer.
     * @param[in] size of buffer.
     */
    virtual void on_request(void* buf, size_t size);
  };

  // Display TWI address
  const uint8_t ADDR;
  
  // Display width (characters per line) and height (lines)
  const uint8_t WIDTH;
  const uint8_t HEIGHT;
  
  /**
   * Construct Virtual LCD connected to given TWI address.
   * @param[in] addr address of Virtual LCD (Default 0x5a);
   * @param[in] width of display, characters per line (Default 16).
   * @param[in] height of display, number of lines (Default 2).
   */
  VLCD(uint8_t addr = 0x5a, uint8_t width = 16, uint8_t height = 2) :
    LCD::Device(),
    ADDR(addr),
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
   * Clear display and move cursor to home.
   */
  virtual void display_clear();

  /**
   * @override
   * Set cursor position to given position.
   * @param[in] x.
   * @param[in] y.
   */
  virtual void set_cursor(uint8_t x, uint8_t y);

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
   * @override
   * Write null terminated string to device. Terminating null is not written.
   * @param[in] s string to write.
   * @return zero(0) or negative error code.
   */
  virtual int puts(char* s);

  /**
   * Write null terminated string from program memory to device.
   * Terminating null is not written.
   * @param[in] s string in program memory to write.
   * @return zero(0) or negative error code.
   */
  virtual int puts_P(const char* s);

  /**
   * Write data from buffer with given size to device.
   * @param[in] buf buffer to write.
   * @param[in] size number of bytes to write.
   * @return number of bytes written or EOF(-1).
   */
  virtual int write(void* buf, size_t size);
};

#endif
