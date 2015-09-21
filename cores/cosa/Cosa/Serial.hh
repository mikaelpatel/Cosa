/**
 * @file Cosa/Serial.hh
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

#ifndef COSA_SERIAL_HH
#define COSA_SERIAL_HH

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

#if defined(BOARD_ATTINY)
# define UCSZ00 1
# define UCSZ01 2
# define USBS0 3
# define UPM00 4
# define UPM01 5
#endif

/**
 * Abstract Serial IOStream Device class. Common definitions for
 * hardware and software UARTs. Hardware serial format symbols are
 * used.
 */
class Serial : public IOStream::Device {
public:
  /** Default baudrate. */
  static const uint32_t DEFAULT_BAUDRATE = 9600L;

  /** Serial formats; DATA + PARITY + STOP. */
  enum {
    DATA5 = 0,
    DATA6 = _BV(UCSZ00),
    DATA7 = _BV(UCSZ01),
    DATA8 = _BV(UCSZ01) | _BV(UCSZ00),
    NO_PARITY = 0,
    EVEN_PARITY = _BV(UPM01),
    ODD_PARITY = _BV(UPM01) | _BV(UPM00),
    STOP1 = 0,
    STOP2 = _BV(USBS0),
  } __attribute__((packed));

  /** Default serial format. */
  static const uint8_t DEFAULT_FORMAT = DATA8 + NO_PARITY + STOP2;

  /**
   * Default Serial device driver constructor.
   */
  Serial() : IOStream::Device() {}

  /**
   * @override{Serial}
   * Start Serial device driver with given serial bitrate and serial
   * format.
   * @param[in] baudrate serial bitrate (default 9600).
   * @param[in] format serial frame format (default DATA8, NO PARITY, STOP2).
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool begin(uint32_t baudrate = DEFAULT_BAUDRATE,
		     uint8_t format = DEFAULT_FORMAT) = 0;

  /**
   * @override{Serial}
   * Stop Serial device driver.
   * @return true(1) if successful otherwise false(0)
   */
  virtual bool end()
  {
    return (true);
  }

  /**
   * @override{Serial}
   * Powerup Serial.
   */
  virtual void powerup() {}

  /**
   * @override{Serial}
   * Powerdown Serial.
   */
  virtual void powerdown() {}
};

#endif
