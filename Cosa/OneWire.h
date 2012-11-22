/**
 * @file Cosa/OneWire.h
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
 * 1-Wire device driver support class. Note: this is for single
 * slave without search and match of rom codes.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_ONEWIRE_H__
#define __COSA_ONEWIRE_H__

#include "Cosa/Pins.h"
#include "Cosa/IOStream.h"
#include "Cosa/Trace.h"

class OneWire : public IOPin {
private:
  uint8_t _crc;

public:
  /**
   * ROM Commands 
   */
  enum {
    SEARCH_ROM = 0xF0,
    READ_ROM = 0x33,
    MATCH_ROM = 0x55,
    SKIP_ROM = 0xCC,
    ALARM_SEARCH = 0xEC
  };

  /**
   * Device connected to a 1-Wire pin
   */
  class Device {
  protected:
    static const uint8_t ROM_MAX = 8;
    uint8_t _rom[ROM_MAX];
    OneWire* _pin;
  public:
    /**
     * Construct one wire device.
     * @param[in] pin one wire bus.
     */
    Device(OneWire* pin) : _pin(pin) {}

    /**
     * Read device rom. This can only be used when there is only
     * one slave on the bus.
     * @return true(1) if successful otherwise false(0).
     */
    bool read_rom();

    /**
     * Match device rom. Address the slave device with the
     * rom code. Device specific function command should follow.
     * @return true(1) if successful otherwise false(0).
     */
    bool match_rom();

    /**
     * Skip device rom for boardcast or single device access.
     * Device specific function command should follow.
     * @return true(1) if successful otherwise false(0).
     */
    bool skip_rom();

    /**
     * Print device rom to output stream. Default stream
     * is the trace stream.
     * @param[in] stream to print rom to.
     */
    void print_rom(IOStream& stream = trace);
  };

  /**
   * Construct one wire bus connected to the given pin.
   * @param[in] pin number.
   */
  OneWire(uint8_t pin) : IOPin(pin) {}

  /**
   * Reset the one wire bus and check that at least one device is
   * presence.
   * @return true(1) if successful otherwise false(0).
   */
  bool reset();

  /**
   * Read the given number of bits from the one wire bus (slave).
   * Default number of bits is 8. Returns the value read MSB aligned.
   * The internal CRC is updated (see begin() and end()).
   * @param[in] bits to be read.
   * @return value read.
   */
  uint8_t read(uint8_t bits = CHARBITS);

  /**
   * Write the given value to the one wire bus. The bits are written
   * from LSB to MSB. 
   * @param[in] value.
   * @param[in] bits to be written.
   */
  void write(uint8_t value, uint8_t bits = CHARBITS);

  /**
   * Begin a read sequence with CRC.
   */
  void begin() { _crc = 0; }

  /**
   * End a read sequence and return the generated CRC. If the
   * read block contains a CRC last the returned value will be
   * zero(0).
   * @return generated CRC.
   */
  uint8_t end() { return (_crc); }
};

#endif
