/**
 * @file Cosa/IOStream/Driver/HD44780.cpp
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

#include "Cosa/Board.hh"
#if !defined(__ARDUINO_TINYX5__)
#include "Cosa/IOStream/Driver/HD44780.hh"
#include "Cosa/Watchdog.hh"

// Bus Timing Characteristics, fig. 25, pp. 50
#define SETUP_TIME 1
#define ENABLE_PULSE_WIDTH 1
#define HOLD_TIME 1
#define SHORT_EXEC_TIME 50
#define LONG_EXEC_TIME 2000
#define POWER_ON_TIME 32
#define INIT0_TIME 4500
#define INIT1_TIME 150

/**
 * Data direction and port register for data/command transfer.
 */
#if defined(__ARDUINO_STANDARD__)
# define DDR DDRD
# define PORT PORTD
#elif defined(__ARDUINO_TINYX4__)
# define DDR DDRA
# define PORT PORTA
#elif defined(__ARDUINO_MEGA__) || defined(__ARDUINO_MIGHTY__)
# define DDR DDRB
# define PORT PORTB
#endif

/**
 * Setup data port.
 */
#define setup4bit() DDR |= 0xf0

/**
 * Write data to port. Transfer most significant 4-bits.
 * @param[in] data.
 */
#define write4bit(data) PORT = (((data) & 0xf0) | (PORT & 0x0f))

void 
HD44780::pulse()
{
  DELAY(SETUP_TIME);
  m_en.pulse(ENABLE_PULSE_WIDTH);
  DELAY(HOLD_TIME);
}

void 
HD44780::write(uint8_t data)
{
  write4bit(data);
  pulse();
  write4bit(data << 4);
  pulse();
  DELAY(SHORT_EXEC_TIME);
}

void 
HD44780::display_clear() 
{
  write(CLEAR_DISPLAY); 
  m_x = 0; 
  m_y = 0; 
  m_mode |= INCREMENT;
  DELAY(LONG_EXEC_TIME);
}

void 
HD44780::cursor_home() 
{ 
  write(RETURN_HOME); 
  m_x = 0; 
  m_y = 0; 
  DELAY(LONG_EXEC_TIME);
}

void 
HD44780::set_cursor(uint8_t x, uint8_t y)
{
  const uint8_t offset[] = { 0x00, 0x40, 0x14, 0x54 };
  if (x >= WIDTH) x = 0;
  if (y >= HEIGHT) y = 0;
  write(SET_DDRAM_ADDR | ((x + offset[y]) & SET_DDRAM_MASK));
  m_x = x;
  m_y = y;
}

void 
HD44780::set_custom_char(uint8_t id, const uint8_t* bitmap)
{
  write(SET_CGRAM_ADDR | ((id << 3) & SET_CGRAM_MASK));
  asserted(m_rs) {
    for (uint8_t i = 0; i < BITMAP_MAX; i++) {
      write(*bitmap++);
    }
  }
}

void 
HD44780::set_custom_char_P(uint8_t id, const uint8_t* bitmap)
{
  write(SET_CGRAM_ADDR | ((id << 3) & SET_CGRAM_MASK));
  asserted(m_rs) {
    for (uint8_t i = 0; i < BITMAP_MAX; i++, bitmap++) {
      write(pgm_read_byte(bitmap));
    }
  }
}

bool 
HD44780::begin()
{
  // Initiate display; See fig. 24, 4-bit interface, pp. 46
  setup4bit();
  Watchdog::delay(POWER_ON_TIME);
  write4bit(FUNCTION_SET | DATA_LENGTH_8BITS);
  pulse();
  DELAY(INIT0_TIME);
  write4bit(FUNCTION_SET | DATA_LENGTH_8BITS);
  pulse();
  DELAY(INIT0_TIME);
  write4bit(FUNCTION_SET | DATA_LENGTH_8BITS);
  pulse();
  DELAY(INIT1_TIME);
  write4bit(FUNCTION_SET | DATA_LENGTH_4BITS);
  pulse();

  // Initialization with the function, control and mode setting
  write(m_func);
  write(m_cntl);
  display_on();
  display_clear();
  write(m_mode);
  return (true);
}

bool 
HD44780::end()
{
  display_off();
  return (true);
}

int 
HD44780::putchar(char c)
{
  // Check for special characters; carriage-return-line-feed
  if (c == '\n') {
    uint8_t x, y;
    set_cursor(0, m_y + 1);
    get_cursor(x, y);
    asserted(m_rs) {
      for (uint8_t i = 0; i < WIDTH; i++) write(' ');
    }
    set_cursor(x, y);
    return (c);
  }

  // Check for special character: back-space
  if (c == '\b') {
    set_cursor(m_x - 1, m_y);
    return (c);
  }

  // Check for special character: alert
  if (c == '\a') {
    // Fix: Should have some indication
    return (c);
  }

  // Check for special character: form-feed
  if (c == '\f') {
    display_clear();
    return (c);
  }

  // Check for horizontal tab
  if (c == '\t') {
    uint8_t x = m_x + m_tab - (m_x % m_tab);
    uint8_t y = m_y + (x >= WIDTH);
    set_cursor(x, y);
    return (c);
  }

  // Write character
  if (m_x == WIDTH) putchar('\n');
  m_x += 1;
  asserted(m_rs) {
    write(c);
  }

  return (c & 0xff);
}
#endif
