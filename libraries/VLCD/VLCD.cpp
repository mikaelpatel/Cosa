/**
 * @file VLCD.cpp
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

#include "VLCD.hh"
#include "Cosa/Watchdog.hh"

void
VLCD::Slave::on_request(void* buf, size_t size)
{
  UNUSED(buf);

  // Check for non command prefix
  char c = (char) m_buf[0];
  if (c != (char) COMMAND) {
    m_lcd->putchar(c);
    for (uint8_t i = 1; i < size; i++)
      m_lcd->putchar(m_buf[i]);
    return;
  }
  // Handle command: COMMAND(uint8_t cmd)
  if (size == 2) {
    uint8_t cmd = m_buf[1];
    switch (cmd) {
    case BACKLIGHT_OFF_CMD: m_lcd->backlight_off(); return;
    case BACKLIGHT_ON_CMD: m_lcd->backlight_on(); return;
    case DISPLAY_OFF_CMD: m_lcd->display_off(); return;
    case DISPLAY_ON_CMD: m_lcd->display_on(); return;
    case INIT_CMD:
      m_lcd->display_clear();
      m_lcd->display_on();
      m_lcd->backlight_on();
      info_t* info = (info_t*) m_buf;
      info->major = MAJOR;
      info->minor = MINOR;
      // Fix: Should be defined by LCD
      info->width = 16;
      info->height = 2;
      return;
    }
  }
  // Handle command: SET_CURSOR(uint8_t x, uint8_t y)
  if (size == 3) {
    uint8_t x = m_buf[1];
    uint8_t y = m_buf[2];
    m_lcd->set_cursor(x, y);
    return;
  }
}

void
VLCD::write(uint8_t cmd)
{
  uint8_t buf[2];
  buf[0] = Slave::COMMAND;
  buf[1] = cmd;
  twi.acquire(this);
  uint8_t retry = 3;
  do {
    int res = twi.write(buf, sizeof(buf));
    if (res == sizeof(buf)) break;
  } while (--retry);
  twi.release();
}

bool
VLCD::begin()
{
  sleep(1);
  write(Slave::INIT_CMD);
  twi.acquire(this);
  uint8_t retry = 3;
  info_t info;
  do {
    int res = twi.read(&info, sizeof(info));
    if (res == sizeof(info)) break;
  } while (--retry);
  twi.release();
  MAJOR = info.major;
  MINOR = info.minor;
  WIDTH = info.width;
  HEIGHT = info.height;
  return (true);
}

bool
VLCD::end()
{
  display_off();
  return (true);
}

void
VLCD::backlight_off()
{
  write(Slave::BACKLIGHT_OFF_CMD);
}

void
VLCD::backlight_on()
{
  write(Slave::BACKLIGHT_ON_CMD);
}

void
VLCD::display_off()
{
  write(Slave::DISPLAY_OFF_CMD);
}

void
VLCD::display_on()
{
  write(Slave::DISPLAY_ON_CMD);
}

void
VLCD::display_clear()
{
  putchar('\f');
}

void
VLCD::set_cursor(uint8_t x, uint8_t y)
{
  uint8_t buf[3];
  buf[0] = 0;
  buf[1] = x;
  buf[2] = y;
  twi.acquire(this);
  twi.write(buf, sizeof(buf));
  twi.release();
  m_x = x;
  m_y = y;
}

int
VLCD::putchar(char c)
{
  twi.acquire(this);
  int n = twi.write(&c, sizeof(c));
  twi.release();
  if (n != 1) return (-1);
  if (c >= ' ') return (c & 0xff);
  switch (c) {
  case '\b':
    if (m_x > 0) m_x -= 1;
    break;
  case '\f':
    m_x = 0;
    m_y = 0;
    break;
  case '\n':
    m_x = 0;
    m_y += 1;
    if (m_y > HEIGHT) m_y = 0;
    break;
  case '\r':
    m_x = 0;
    break;
  case '\t':
    m_x += m_tab - (m_x % m_tab);
    m_y += (m_x >= WIDTH);
    if (m_x >= WIDTH) m_x = 0;
    break;
  }
  return (c & 0xff);
}

int
VLCD::write(const void* buf, size_t size)
{
  twi.acquire(this);
  int n = twi.write((void*) buf, size);
  twi.release();
  if (n < 0) return (-1);
  m_x += size;
  return (n);
}
