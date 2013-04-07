/**
 * @file Cosa/OWI/Driver/DS18B20.hh
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

#ifndef __COSA_OWI_DRIVER_DS18B20_HH__
#define __COSA_OWI_DRIVER_DS18B20_HH__

#include "Cosa/OWI.hh"

/**
 * Driver for the DS18B20 Programmable Resolution 1-Write Digital
 * Thermometer.  
 * 
 * @section See Also
 * Maxim Integrated product description (REV: 042208) 
 */
class DS18B20 : public OWI::Driver {
private:
  /**
   * DS18B20 Function Commands (Table 3, pp. 12)
   */
  enum {
    FAMILY_CODE = 0x28,
    CONVERT_T = 0x44,
    READ_SCRATCHPAD = 0xBE,
    WRITE_SCRATCHPAD = 0x4E,
    COPY_SCRATCHPAD = 0x48,
    RECALL_E = 0xB8,
    READ_POWER_SUPPLY = 0xB4
  } __attribute__((packed));

  /**
   * DS18B20 Memory Map (Figure 7, pp. 7)
   */
  struct scratchpad_t {
    int16_t temperature;
    int8_t high_trigger;
    int8_t low_trigger;
    uint8_t configuration;
    uint8_t reserved[3];
    uint8_t crc;
  };
  scratchpad_t m_scratchpad;

public:
  /**
   * Max conversion time for 12-bit conversion (Table 2, pp. 8)
   */
  static const uint16_t MAX_CONVERSION_TIME = 750;

  /**
   * Construct a DS18B20 device connected to the given one wire bus
   * and device identity (in EEPROM). Default device identity is null.
   * @param[in] pin one wire bus pin.
   * @param[in] rom device identity (default null).
   */
  DS18B20(OWI* pin, const uint8_t* rom = 0) : 
    OWI::Driver(pin, rom) 
  {}

  /**
   * Connect to DS18B20 device with given index.
   * @param[in] index device order.
   * @return true(1) if successful otherwise false(0).
   */
  bool connect(uint8_t index)
  {
    return (OWI::Driver::connect(FAMILY_CODE, index));
  }

  /**
   * Set conversion resolution from 9..12 bits. Use write_scratchpad()
   * and copy_scratchpad() to update device.
   * Returns max conversion time in milli-seconds.
   * @param[in] bits resolution.
   * @return max conversion time (ms)
   */
  uint16_t set_resolution(uint8_t bits);

  /**
   * Set alarm trigger values; high and low threshold values.
   * Use write_scratchpad() and copy_scratchpad() to update device.
   * @param[in] high threshold. 
   * @param[in] low threshold.
   */
  void set_trigger(uint8_t high, uint8_t low)
  {
    m_scratchpad.high_trigger = high;
    m_scratchpad.low_trigger = low;
  }

  /**
   * Get the latest temperature reading from the local memory scratchpad.
   * Call convert_request() and read_scratchpad() before accessing the
   * scratchpad. Returns at highest resolution a fixed point<12,4>
   * point number. For 11-bit resolution, bit 0 is undefined, 10-bits
   * bit 1 and 0, and so on.
   * @return temperature
   */
  int16_t get_temperature()
  {
    return (m_scratchpad.temperature);
  }
  
  /**
   * Get conversion resolution. Use read_scratchpad() to read values
   * from device.
   * @return number of bits.
   */
  uint8_t get_resolution()
  {
    return (9 + (m_scratchpad.configuration >> 5));
  }

  /**
   * Get alarm trigger values; high and low threshold values.
   * Use read_scratchpad() to read values from device.
   * @param[out] high threshold. 
   * @param[out] low threshold.
   */
  void get_trigger(uint8_t& high, uint8_t& low)
  {
    high = m_scratchpad.high_trigger;
    low = m_scratchpad.low_trigger;
  }

  /**
   * Initiate a single temperature conversion. With the default
   * setting 12-bit resolution the max conversion time is 750 ms,
   * MAX_CONVERSION_TIME. 
   * @return true(1) if successful otherwise false(0).
   */
  bool convert_request();

  /**
   * Write the contents of the scratchpad triggers and 
   * configuration (3 bytes) to device.
   * @return true(1) if successful otherwise false(0).
   */
  bool write_scratchpad();

  /**
   * Read the contents of the scratchpad to local memory.
   * @return true(1) if successful otherwise false(0).
   */
  bool read_scratchpad();

  /**
   * Copy device scratchpad triggers and configuration data 
   * to device EEPROM.
   * @return true(1) if successful otherwise false(0).
   */
  bool copy_scratchpad();

  /**
   * Recall the alarm triggers and configuration from device EEPROM. 
   * @return true(1) if successful otherwise false(0).
   */
  bool recall();
};

#endif
