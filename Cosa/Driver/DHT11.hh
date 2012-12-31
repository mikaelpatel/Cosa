/**
 * @file Cosa/Driver/DHT11.hh
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
 * DHT11 Humidity & Temperature Sensor device driver. Subclass
 * and implement the event handler, on_event(), to allow periodic
 * read of device (attach to watchdog timeout queue).
 *
 * @section Circuit
 * Connect DHT11 to pin, VCC and ground. A pullup resistor from
 * the pin to VCC should be used. Most DHT11 modules have a built-in 
 * pullup resistor.
 *
 * @section Limitations
 * The driver will turn off interrupt handling during data read 
 * from the device. 
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_DRIVER_DHT11_HH__
#define __COSA_DRIVER_DHT11_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Thing.hh"

class DHT11 : private Thing {

public:
  /**
   * Size of data buffer.
   */
  static const uint8_t DATA_MAX = 5;

private:
  /**
   * Input/Output pin, Data buffer and latest pin level.
   */
  IOPin m_pin;
  uint8_t m_latest;
  uint8_t m_data[DATA_MAX];

  /** 
   * Read the next bit from the device given number of level
   * changes. Return one(1) if the bit was set, zero(0) if clear,
   * otherwise a negative error code. 
   * @param[in] changes number of level transitions.
   * @return one(1) if the bit was set, zero(0) if clear, otherwise a
   * negative error code. 
   */
  int8_t read_bit(uint8_t changes);
  
  /**
   * Read data from the device. Return true(1) if successful otherwise
   * false(0).    
   * @return bool.
   */
  bool read_data();

public:
  /**
   * Construct connection to a DHT11 device on given in/output-pin.
   * @param[pin] pin data.
   */
  DHT11(uint8_t pin) : m_pin(pin) {}

  /**
   * Read temperature and humidity from the device. Return true(1) and
   * values if successful otherwise false(0).  
   * @param[out] temperature reading.
   * @param[out] humidity reading.
   * @return bool.
   */
  bool read(uint8_t& temperature, uint8_t& humidity)
  {
    if (!read_data()) return (0);
    temperature = get_temperature();
    humidity = get_humidity();
  }

  /**
   * Return temperature from latest read.
   * @return temperature.
   */
  uint8_t get_temperature()
  {
    return (m_data[2]);
  }

  /**
   * Return humidity from latest read.
   * @return humidity.
   */
  uint8_t get_humidity()
  {
    return (m_data[0]);
  }

  /**
   * @override
   * Default device event handler function. Attach to watchdog
   * timer queue, Watchdog::attach(), to allow perodic reading.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value)
  {
    read_data();
  }
};

#endif
