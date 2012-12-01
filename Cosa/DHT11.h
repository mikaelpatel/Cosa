/**
 * @file DHT11.h
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
 * DHT11 Humidity & Temperature Sensor device driver.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_DHT11_H__
#define __COSA_DHT11_H__

#include "Cosa/Pins.h"
#include "Cosa/IOStream.h"
#include "Cosa/Trace.h"

class DHT11 : private IOPin {

private:
  static const uint8_t DATA_MAX = 5;
  uint8_t _data[DATA_MAX];
  uint8_t _latest;

  /** 
   * Read the next bit from the device given number 
   * of level changes. Return one(1) if the bit was set,
   * zero(0) if clear, otherwise a negative error code.
   * @param[in] changes
   */
  int8_t read_bit(uint8_t changes);
  
public:
  /**
   * Construct connection to a DHT11 device on given pin.
   */
  DHT11(uint8_t pin) : IOPin(pin) {}

  /**
   * Sample the device. Return true(1) if successful 
   * otherwise false(0). 
   */
  bool sample();

  /**
   * Return sampled temperature.
   */
  uint8_t get_temperature() { return (_data[2]); }

  /**
   * Return sampled humidity.
   */
  uint8_t get_humidity() { return (_data[0]); }

  /**
   * Print humidity and temperature to given output stream.
   * Default is the trace stream.
   * @param[in] stream output stream.
   */
  void print(IOStream& stream = trace);
};

#endif
