/**
 * @file Cosa/IOStream/UART.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012, Mikael Patel
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
 * @section Description
 * Basic UART device handler with internal buffering.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_UART_HH__
#define __COSA_UART_HH__

#include "Cosa/Types.h"
#include "Cosa/IOStream.hh"

class UART : public IOStream::Device {

public:
  /* As defined by IOStream::Device. Rest is inherited from null device */
  virtual int putchar(char c);
  virtual int flush();

  /**
   * Start UART device.
   * @param[in] baudrate serial bitrate.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint32_t baudrate = 9600);

  /**
   * Stop UART device.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

 private:
  static const uint32_t FLUSH_CYCLES_MAX = 1000000;
};

#endif
