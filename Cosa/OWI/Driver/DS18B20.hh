/**
 * @file Cosa/OWI/Driver/DS18B20.hh
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
 * Driver for the DS18B20 Programmable Resolution 1-Write
 * Digital Thermometer. See Maxim Integrated product description.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_OWI_DRIVER_DS18B20_HH__
#define __COSA_OWI_DRIVER_DS18B20_HH__

#include "Cosa/OWI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"

class DS18B20 : public OWI::Driver {
private:
  /**
   * DS18B20 Function Commands (Table 3, pp. 12)
   */
  enum {
    FAMILY_CODE = 0x28,
    CONVERT_T = 0x44,
    WRITE_SCRATCHPAD = 0x4E,
    READ_SCRATCHPAD = 0xBE,
    COPY_SCRATCHPAD = 0x48,
    RECALL_E = 0xB8,
    READ_POWER_SUPPLY = 0xB4
  } __attribute__((packed));

  /**
   * DS18B20 Memory Map (Figure 7, pp 7)
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

  /**
   * Read the contents of the scratchpad to local memory.
   * @return true(1) if successful otherwise false(0).
   */
  bool read_scratchpad();

  /**
   * Print the contents of the local memory scratchpad to given stream.
   * The default stream is the trace stream.
   * @param[in] stream output stream.
   */
  void print_scratchpad(IOStream& stream = trace);

public:
  /**
   * Construct a DS18B20 device connected to the given one wire bus.
   * @param[in] pin one wire bus pin.
   * @param[in] rom device identity (default null).
   */
  DS18B20(OWI* pin, const uint8_t* rom = 0) : OWI::Driver(pin, rom) {}

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
   * Initiate a single temperature conversion. With default setting
   * 12-bit resolution the max conversion time is 750 ms.
   * @return true(1) if successful otherwise false(0).
   */
  bool convert_request();

  /**
   * Read the temperture from device.
   * @return true(1) if successful otherwise false(0).
   */
  bool read_temperature()
  {
    return (read_scratchpad());
  }

  /**
   * Get the latest temperature reading from the local memory scratchpad.
   * @return temperature as a fixed<12,4> point number.
   */
  int16_t get_temperature();

  /**
   * Print the latest temperature reading to given stream. 
   * The default stream is the trace stream.
   * @param[in] prefix string.
   * @param[in] stream output stream.
   */
  void print_temperature_P(const char* prefix, IOStream& stream = trace);
};

#endif
