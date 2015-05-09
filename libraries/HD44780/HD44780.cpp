/**
 * @file HD44780.cpp
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

#include "HD44780.hh"

// DDR offset table
// 0: 40X2, 20X4, 20X2, 16X2, 16X1
// 1: 16X4
const uint8_t HD44780::offset0[] __PROGMEM = { 0x00, 0x40, 0x14, 0x54 };
const uint8_t HD44780::offset1[] __PROGMEM = { 0x00, 0x40, 0x10, 0x50 };

void
HD44780::IO::write8b(uint8_t data)
{
  write4b(data >> 4);
  write4b(data);
}

void
HD44780::IO::write8n(void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  while (size--) write8b(*bp++);
}

bool
HD44780::begin()
{
  // Initiate display; See fig. 24, 4-bit interface, pp. 46.
  // http://web.alfredstate.edu/weimandn/lcd/lcd_initialization/-
  // LCD%204-bit%20Initialization%20v06.pdf
  const uint8_t FS0 = (FUNCTION_SET | DATA_LENGTH_8BITS);
  const uint8_t FS1 = (FUNCTION_SET | DATA_LENGTH_4BITS);
  bool mode = m_io->setup();
  delay(POWER_ON_TIME);
  if (!mode) {
    m_io->write4b(FS0 >> 4);
    DELAY(INIT0_TIME);
    m_io->write4b(FS0 >> 4);
    DELAY(INIT1_TIME);
    m_io->write4b(FS0 >> 4);
    DELAY(INIT1_TIME);
    m_io->write4b(FS1 >> 4);
    DELAY(INIT1_TIME);
  }
  // 8-bit initialization
  else {
    set(m_func, DATA_LENGTH_8BITS);
  }

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
  if (x >= WIDTH) x = 0;
  if (y >= HEIGHT) y = 0;
  uint8_t offset = (uint8_t) pgm_read_byte(&m_offset[y]);
  write(SET_DDRAM_ADDR | ((x + offset) & SET_DDRAM_MASK));
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

    // Carriage-return: move to start of line
    if (c == '\r') {
      set_cursor(0, m_y);
      return (c);
    }

    // New-line: clear line
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

    // Form-feed: clear the display
    if (c == '\f') {
      display_clear();
      return (c);
    }

    // Back-space: move cursor back one step (if possible)
    if (c == '\b') {
      set_cursor(m_x - 1, m_y);
      return (c);
    }

    // Alert: blink the backlight
    if (c == '\a') {
      backlight_off();
      delay(32);
      backlight_on();
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

int
HD44780::write(void* buf, size_t size)
{
  set_data_mode();
  {
    m_io->write8n(buf, size);
  }
  set_instruction_mode();
  m_x += size;
  return (size);
}

#include "Cosa/Board.hh"
#if !defined(BOARD_ATTINYX5)
#include "HD44780.hh"

bool
HD44780::Port4b::setup()
{
  return (false);
}

void
HD44780::Port4b::write4b(uint8_t data)
{
  synchronized {
    m_d0._set(data & 0x01);
    m_d1._set(data & 0x02);
    m_d2._set(data & 0x04);
    m_d3._set(data & 0x08);
    m_en._toggle();
    m_en._toggle();
  }
}

void
HD44780::Port4b::write8b(uint8_t data)
{
  synchronized {
    m_d0._set(data & 0x10);
    m_d1._set(data & 0x20);
    m_d2._set(data & 0x40);
    m_d3._set(data & 0x80);
    m_en._toggle();
    m_en._toggle();
    m_d0._set(data & 0x01);
    m_d1._set(data & 0x02);
    m_d2._set(data & 0x04);
    m_d3._set(data & 0x08);
    m_en._toggle();
    m_en._toggle();
  }
  DELAY(SHORT_EXEC_TIME);
}

void
HD44780::Port4b::set_mode(uint8_t flag)
{
  m_rs.write(flag);
}

void
HD44780::Port4b::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}

#endif

HD44780::SR3W::SR3W(Board::DigitalPin sda,
		    Board::DigitalPin scl,
		    Board::DigitalPin en) :
  m_port(),
  m_sda(sda),
  m_scl(scl),
  m_en(en)
{
}

bool
HD44780::SR3W::setup()
{
  return (false);
}

void
HD44780::SR3W::write4b(uint8_t data)
{
  m_port.data = data;
  uint8_t value = m_port;
  synchronized {
    m_sda._write(value & 0x20);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x10);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x08);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x04);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x02);
    m_scl._toggle();
    m_scl._toggle();
    m_sda._write(value & 0x01);
    m_scl._toggle();
    m_scl._toggle();
    m_en._toggle();
    m_en._toggle();
  }
}

void
HD44780::SR3W::write8b(uint8_t data)
{
  write4b(data >> 4);
  write4b(data);
#if (I_CPU >= 16)
  DELAY(SHORT_EXEC_TIME);
#endif
}

void
HD44780::SR3W::set_mode(uint8_t flag)
{
  m_port.rs = flag;
}

void
HD44780::SR3W::set_backlight(uint8_t flag)
{
  m_port.bt = flag;
}

HD44780::SR3WSPI::SR3WSPI(Board::DigitalPin en) :
  SPI::Driver(en, SPI::PULSE_HIGH),
  m_port()
{
}

bool
HD44780::SR3WSPI::setup()
{
  return (false);
}

void
HD44780::SR3WSPI::write4b(uint8_t data)
{
  m_port.data = data;
  spi.acquire(this);
    spi.begin();
      spi.transfer(m_port);
    spi.end();
  spi.release();
}

void
HD44780::SR3WSPI::write8b(uint8_t data)
{
  spi.acquire(this);
    spi.begin();
      m_port.data = (data >> 4);
      spi.transfer(m_port);
      m_port.data = data;
      m_cs._toggle();
      m_cs._toggle();
      spi.transfer(m_port);
    spi.end();
  spi.release();
  DELAY(SHORT_EXEC_TIME);
}

void
HD44780::SR3WSPI::set_mode(uint8_t flag)
{
  m_port.rs = flag;
}

void
HD44780::SR3WSPI::set_backlight(uint8_t flag)
{
  m_port.bt = flag;
}

HD44780::SR4W::SR4W(Board::DigitalPin sda,
		    Board::DigitalPin scl,
		    Board::DigitalPin en,
		    Board::DigitalPin bt) :
  m_sda(sda),
  m_scl(scl),
  m_en(en),
  m_bt(bt, 1),
  m_rs(0)
{
}

bool
HD44780::SR4W::setup()
{
  return (true);
}

void
HD44780::SR4W::write4b(uint8_t data)
{
  write8b(data);
}

void
HD44780::SR4W::write8b(uint8_t data)
{
  m_sda.write(data, m_scl);
  synchronized {
    m_sda._write(m_rs);
    m_en._toggle();
    m_en._toggle();
  }
  DELAY(SHORT_EXEC_TIME);
}

void
HD44780::SR4W::write8n(void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  while (size--) write8b(*bp++);
}

void
HD44780::SR4W::set_mode(uint8_t flag)
{
  m_rs = flag;
}

void
HD44780::SR4W::set_backlight(uint8_t flag)
{
  m_bt.write(flag);
}
