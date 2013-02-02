/**
 * @file Cosa/TWI/Driver/DS1307.hh
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
 * @section Description
 * Driver for the DS1307, 64 X 8, Serial I2C Real-Time Clock.
 * For further details see Maxim Integrated product description; 
 * http://datasheets.maximintegrated.com/en/ds/DS1307.pdf
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_TWI_DRIVER_DS1307_HH__
#define __COSA_TWI_DRIVER_DS1307_HH__

#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/Trace.hh"

class DS1307 : private TWI::Driver {
private:
  static const uint8_t ADDR = 0xD0;
  static const uint8_t RAM_MAX = 0x40;

public:
  /**
   * The Timekeeper Registers (Table 2, pp. 8)
   */
  struct timekeeper_t {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
    uint8_t cntl;

    /**
     * Convert time to binary representation (from BCD). 
     * Apply after reading from device and before any calculation.
     */
    void to_binary();

    /**
     * Convert time to BCD representation (from binary).
     * Apply after setting new value and writing to the device.
     */
    void to_bcd();

    /**
     * Print the date to the given stream with the given format.
     * Default format is PSTR("%d-%d-%d %d:%d:%d").
     * @param[in] stream output stream (default trace).
     * @param[in] format string in program memory (default above).
     */
    void print(IOStream& stream = trace, const char* format = 0);
  };

  /**
   * Read ram block with the given size into the buffer from the position.
   * Return number of bytes read or negative error code.
   * @param[in] buf buffer to read from ram.
   * @param[in] size number of bytes to read.
   * @param[in] pos address in ram to read from.
   * @return number of bytes or negative error code.
   */
  int read_ram(void* buf, uint8_t size, uint8_t pos = 0);

  /**
   * Write ram block at given position with the contents from buffer.
   * Return number of bytes written or negative error code.
   * @param[in] buf buffer to write to ram.
   * @param[in] size number of bytes to write.
   * @param[in] pos address in ram to read write to.
   * @return number of bytes or negative error code.
   */
  int write_ram(void* buf, uint8_t size, uint8_t pos = 0);

  /**
   * Read current time from real-time clock. Return true(1)
   * if successful otherwise false(0).
   * @param[out] now time structure return value.
   * @return boolean.
   */
  bool get_time(timekeeper_t& now) 
  {
    return (read_ram(&now, sizeof(now)) == sizeof(now));
  }

  /**
   * Set the current time from real-time clock with the given
   * time. Return true(1) if successful otherwise false(0).
   * @param[in] now time structure to set.
   * @return boolean.
   */
  bool set_time(timekeeper_t& now)
  {
    return (write_ram(&now, sizeof(now)) == sizeof(now) ||
	    write_ram(&now, sizeof(now), sizeof(timekeeper_t)) == sizeof(now));
  }

};

#endif
