/**
 * @file Cosa/Keypad.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2014, Mikael Patel
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

#include "Cosa/Keypad.hh"

void 
Keypad::Key::on_change(uint16_t value)
{
  uint8_t nr = 0;
  while (value < (uint16_t) pgm_read_word(&m_map[nr])) nr++;
  if (nr == m_latest) return;
  if (nr != 0)
    m_keypad->on_key_down(nr);
  else
    m_keypad->on_key_up(m_latest);
  m_latest = nr;
}

void 
Keypad::on_event(uint8_t type, uint16_t value)
{
  UNUSED(value);
  if (type != Event::TIMEOUT_TYPE) return;
  m_key.sample_request(Event::SAMPLE_COMPLETED_TYPE);
}

const uint16_t LCDKeypad::m_map[] __PROGMEM = { 
  1000, 700, 400, 300, 100, 0
};
