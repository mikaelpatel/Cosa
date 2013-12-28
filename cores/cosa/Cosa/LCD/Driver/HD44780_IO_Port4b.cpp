/**
 * @file Cosa/LCD/Driver/HD44780_IO_Port4b.cpp
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

/**
 * Data direction and port register for data transfer (D4..D7 for
 * Arduino Standard/USB/Mighty/Mega and D0..D3 for TinyX4).
 */
#if defined(__ARDUINO_STANDARD__)
# define DDR DDRD
# define PORT PORTD
# define POS 4
#elif defined(__ARDUINO_MEGA__)			\
  || defined(__ARDUINO_MIGHTY__)		\
  || defined(__ARDUINO_STANDARD_USB__)
# define DDR DDRB
# define PORT PORTB
# define POS 4
#elif defined(__ARDUINO_TINYX4__)
# define DDR DDRA
# define PORT PORTA
# define POS 0
#elif defined(__ARDUINO_TINYX61__)
# define DDR DDRA
# define PORT PORTA
# define POS 4
#endif

#define MASK (0x0f << POS)
#define WRITE4B(data) PORT = ((((data) & 0x0f) << POS) | (PORT & ~MASK))

bool
HD44780::Port4b::setup()
{
  synchronized {
    DDR |= MASK;
  }
  return (false);
}

void 
HD44780::Port4b::write4b(uint8_t data)
{
  synchronized {
    WRITE4B(data);
    m_en._toggle();
    m_en._toggle();
  }
}

void 
HD44780::Port4b::write8b(uint8_t data)
{
  synchronized {
    WRITE4B(data >> 4);
    m_en._toggle();
    m_en._toggle();
    WRITE4B(data);
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
