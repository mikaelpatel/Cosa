/**
 * @file Cosa/Driver/DHT.hh
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

#ifndef __COSA_DRIVER_DHT_HH__
#define __COSA_DRIVER_DHT_HH__

#include "Cosa/Types.h"
#include "Cosa/Pins.hh"
#include "Cosa/Linkage.hh"

/**
 * DHT11/22 Humidity & Temperature Sensor abstract device driver. 
 */
class DHT : public Link {
protected:
  /** Size of data buffer */
  static const uint8_t DATA_MAX = 5;

  /** Last data elemement index */
  static const uint8_t DATA_LAST = DATA_MAX - 1;

  /**
   * Data read from the device. Allow mapping between received byte 
   * vector and data fields.
   */
  union data_t {
    uint8_t as_byte[DATA_MAX];
    struct {
      int16_t humidity;
      int16_t temperature;
      uint8_t chksum;
    };
  };
  
  IOPin m_pin;			/** Pin connected to device */
  data_t m_data;		/** Latest received data */
  data_t m_offset;		/** Calibration offset */
  uint8_t m_latest;		/** Latest pin level read */
  
  /** 
   * Read the next bit from the device given number of level
   * changes. Return one(1) if the bit was set, zero(0) if clear,
   * otherwise a negative error code. 
   * @param[in] changes number of level transitions.
   * @return one(1) if the bit was set, zero(0) if clear, otherwise 
   * a negative error code(-1). 
   */
  int8_t read_bit(uint8_t changes);
  
  /**
   * Read data from the device. Return true(1) if successful otherwise
   * false(0).    
   * @return bool.
   */
  bool read_data();

  /**
   * Adjust data from the device. Communication protocol is the same
   * for the DHT device family but data representation is different,
   * i.e. data resolution and accuracy. Overridden by DHT11 and DHT22.
   */
  virtual void adjust_data() = 0;

public:
  /**
   * Construct connection to a DHT device on given in/output-pin.
   * Set humidity and temperature calibration offsets to zero.
   * @param[in] pin data.
   */
  DHT(Board::DigitalPin pin) : 
    Link(), 
    m_pin(pin)
  {
    m_offset.humidity = 0;
    m_offset.temperature = 0;
  }

  /**
   * Read temperature and humidity from the device. Return true(1) and
   * values if successful otherwise false(0).  
   * @param[out] humidity reading.
   * @param[out] temperature reading.
   * @return bool.
   */
  bool read(int16_t& humidity, int16_t& temperature)
  {
    if (!read_data()) return (false);
    humidity = get_humidity();
    temperature = get_temperature();
    return (true);
  }

  /**
   * Return temperature from latest read adjusted with given calibration
   * offset.
   * @return temperature.
   */
  int16_t get_temperature()
  {
    return (m_data.temperature);
  }

  /**
   * Return humidity from latest read adjusted with given calibration 
   * offset.
   * @return humidity.
   */
  int16_t get_humidity()
  {
    return (m_data.humidity);
  }

  /**
   * Set calibration offset for temperature and humidity readings.
   * The given values are added to the values read from the device.
   * @param[in] temperature.
   * @param[in] humidity.
   */
  void calibrate(int16_t humidity, int16_t temperature)
  {
    m_offset.humidity = humidity;
    m_offset.temperature = temperature;
  }

  /**
   * @override
   * Default device event handler function. Attach to watchdog
   * timer queue, Watchdog::attach(), to allow perodic reading.
   * @param[in] type the type of event.
   * @param[in] value the event value.
   */
  virtual void on_event(uint8_t type, uint16_t value);
};

#endif
