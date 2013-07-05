/**
 * @file Cosa/LCD/Driver/HD44780.cpp
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
#include "Cosa/LCD/Driver/HD44780.hh"
#include "Cosa/Watchdog.hh"

void 
HD44780::IO::write8b(uint8_t data)
{
  write4b(data >> 4);
  write4b(data);
}

void 
HD44780::set_data_mode()
{
  m_io->set_mode(1);
}

void
HD44780::set_instruction_mode()
{
  m_io->set_mode(0);
}

bool 
HD44780::begin()
{
  // Initiate display; See fig. 24, 4-bit interface, pp. 46.
  // http://web.alfredstate.edu/weimandn/lcd/lcd_initialization/-
  // LCD%204-bit%20Initialization%20v06.pdf
  const uint8_t FS0 = ((FUNCTION_SET | DATA_LENGTH_8BITS) >> 4);
  const uint8_t FS1 = ((FUNCTION_SET | DATA_LENGTH_4BITS) >> 4);
  m_io->setup();
  Watchdog::delay(POWER_ON_TIME);
  m_io->write4b(FS0);
  DELAY(INIT0_TIME);
  m_io->write4b(FS0);
  DELAY(INIT1_TIME);
  m_io->write4b(FS0);
  DELAY(INIT1_TIME);
  m_io->write4b(FS1);
  DELAY(INIT1_TIME);

  // Initialization with the function, control and mode setting
  write(m_func);
  write(m_cntl);
  display_clear();
  write(m_mode);

  // Initialization completed. Turn on the display and backlight
  display_on();
  backlight_on();
  return (true);
}

bool 
HD44780::end()
{
  display_off();
  return (true);
}

void 
HD44780::backlight_on() 
{ 
  m_io->set_backlight(1);
}

void 
HD44780::backlight_off() 
{ 
  m_io->set_backlight(0);
}

void 
HD44780::display_on() 
{ 
  set(m_cntl, DISPLAY_ON); 
}

void 
HD44780::display_off() 
{ 
  clear(m_cntl, DISPLAY_ON); 
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
  set_data_mode();
  {
    for (uint8_t i = 0; i < BITMAP_MAX; i++)
      write(*bitmap++);
  }
  set_instruction_mode();
}

void 
HD44780::set_custom_char_P(uint8_t id, const uint8_t* bitmap)
{
  write(SET_CGRAM_ADDR | ((id << 3) & SET_CGRAM_MASK));
  set_data_mode();
  {
    for (uint8_t i = 0; i < BITMAP_MAX; i++, bitmap++)
      write(pgm_read_byte(bitmap));
  }
  set_instruction_mode();
}

int 
HD44780::putchar(char c)
{
  // Check for special characters
  if (c < ' ') {

    // Alert
    if (c == '\a') {
      // Fix: Should have some indication
      return (c);
    }

    // Back-space
    if (c == '\b') {
      set_cursor(m_x - 1, m_y);
      return (c);
    }

    // Form-feed
    if (c == '\f') {
      display_clear();
      return (c);
    }
    
    // Carriage-return-line-feed
    if (c == '\n') {
      uint8_t x, y;
      set_cursor(0, m_y + 1);
      get_cursor(x, y);
      set_data_mode();
      {
	for (uint8_t i = 0; i < WIDTH; i++) write(' ');
      }
      set_instruction_mode();
      set_cursor(x, y);
      return (c);
    }

    // Horizontal tab
    if (c == '\t') {
      uint8_t x = m_x + m_tab - (m_x % m_tab);
      uint8_t y = m_y + (x >= WIDTH);
      set_cursor(x, y);
      return (c);
    }
  }

  // Write character
  if (m_x == WIDTH) putchar('\n');
  m_x += 1;
  set_data_mode();
  {
    write(c);
  }
  set_instruction_mode();

  return (c & 0xff);
}
#endif
