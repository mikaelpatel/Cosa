/**
 * @file Cosa/Soft/UART.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#ifndef __COSA_SOFT_UART_HH__
#define __COSA_SOFT_UART_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/IOStream.hh"

/**
 * Software version some of the AVR hardware.
 */
namespace Soft {

  /**
   * Soft UART for transmission only using the OutputPin serial 
   * write method. May be used for trace output from ATtiny
   * devices. See Cosa/IOStream/Device/UART.hh.
   */
  class UART : public IOStream::Device {
  private:
    static const uint8_t DATA_MASK = 7;
    OutputPin m_pin;
    uint8_t m_stops;
    uint8_t m_bits;
    uint16_t m_us;

  public:
    enum {
      DATA5 = 0,
      DATA6 = 1,
      DATA7 = 2,
      DATA8 = 3,
      DATA9 = 4,
      NO_PARITY = 0,
      EVEN_PARITY = 8,
      ODD_PARITY = 16,
      STOP1 = 0,
      STOP2 = 32
    } __attribute__((packed));

    /**
     * Construct Soft UART with transmitter on given output pin.
     * @param[in] pin transmitter (default D1).
     */
    UART(Board::DigitalPin pin = Board::D1) :
      IOStream::Device(),
      m_pin(pin),
      m_stops(2),
      m_bits(8),
      m_us(1000000UL / 9600)
    {
      m_pin.set();
    }

    /**
     * @override IOStream::Device
     * Write character to serial 
     * Returns character if successful otherwise on error or buffer full
     * returns EOF(-1),
     * @param[in] c character to write.
     * @return character written or EOF(-1).
     */
    virtual int putchar(char c)
    {
      uint16_t d = ((0xff00 | c) << 1);
      m_pin.write(d, m_bits + m_stops + 1, m_us);
      return (c);
    }

    /**
     * Start Soft UART device driver.
     * @param[in] baudrate serial bitrate (default 9600).
     * @param[in] format serial frame format (default async, 8data, 2stop bit)
     * @return true(1) if successful otherwise false(0)
     */
    bool begin(uint32_t baudrate = 9600UL, 
	       uint8_t format = DATA8 + NO_PARITY + STOP2)
    {
      m_stops = ((format & STOP2) != 0) + 1;
      m_bits = (5 + (format & DATA_MASK));
      m_us = 1000000UL / baudrate;
      return (true);
    }

    /**
     * Stop Soft UART device driver. 
     * @return true(1) if successful otherwise false(0)
     */
    bool end()
    {
      return (true);
    }
  };
};

#endif
