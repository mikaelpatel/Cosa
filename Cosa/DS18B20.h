/**
 * @file Cosa/DS18B20.h
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
 * Digital Thermometer.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_ONEWIRE_DS18B20__
#define __COSA_ONEWIRE_DS18B20__

#include "Cosa/OneWire.h"
#include "Cosa/IOStream.h"
#include "Cosa/Trace.h"

class DS18B20 : public OneWire::Device {
private:
  /**
   * DS18B20 Function Commands
   */
  enum {
    CONVERT_T = 0x44,
    WRITE_SCRATCHPAD = 0x4E,
    READ_SCRATCHPAD = 0xBE,
    COPY_SCRATCHPAD = 0x48,
    RECALL_E = 0xB8,
    READ_POWER_SUPPLY = 0xB4
  };
  static const uint8_t SCRATCHPAD_MAX = 9;
  uint8_t _scratchpad[SCRATCHPAD_MAX];

public:
  /**
   * Construct a DS18B20 device connected to the given one wire bus.
   * @param[in] pin one wire bus pin.
   */
  DS18B20(OneWire* pin) : OneWire::Device(pin) {}

  /**
   * Initiate a single temperature conversion.
   * @return true(1) if successful otherwise false(0).
   */
  bool convert_request();

  /**
   * Read the contents of the scratchpad to local memory.
   * @return true(1) if successful otherwise false(0).
   */
  bool read_scratchpad();

  /**
   * Print the contents of the local memory scratchpad to given stream.
   * The default stream is the trace stream.
   */
  void print_scratchpad(IOStream& stream = trace);

  /**
   * Get the latest temperature reading from the local memory scratchpad.
   * @return temperature as a 16bit, fixed point(4) number.
   */
  uint16_t get_temperature();
};

#endif
