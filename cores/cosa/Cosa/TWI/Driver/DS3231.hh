/**
 * @file Cosa/TWI/Driver/DS3231.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, Mikael Patel
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

#ifndef __COSA_TWI_DRIVER_DS3231_HH__
#define __COSA_TWI_DRIVER_DS3231_HH__

#include "Cosa/TWI.hh"
#include "Cosa/Time.hh"
#include "Cosa/IOStream.hh"

/**
 * Driver for the DS3231, Extremely Accurate I2C-Integrated
 * RTC/TCXO/Crystal. 
 *
 * @section Circuit
 * The Mini RTC pro module with pull-up resistors (4K7) for TWI signals.
 * @code
 *                        Mini RTC pro
 *                       +------------+
 *                     1-|32KHz       |
 *                     2-|SQW         |
 * (A5/SCL)------------3-|SCL         |
 * (A4/SDA)------------4-|SDA         |
 * (GND)---------------5-|GND         |
 * (GND)---------------6-|VCC         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Maxim Integrated product description; 
 * http://datasheets.maximintegrated.com/en/ds/DS3231.pdf
 */
class DS3231 : private TWI::Driver {
public:
  /**
   * Alarm1 register sub-set type and mask bits (Table 2, pp. 12)
   */
  struct alarm1_t {
    enum {
      ONCE_PER_SEC = 0x0f,
      WHEN_SEC_MATCH = 0x0e,
      WHEN_MIN_SEC_MATCH = 0x0c,
      WHEN_TIME_MATCH = 0x08,
      WHEN_DATE_TIME_MATCH = 0x00,
      WHEN_DAY_TIME_MATCH = 0x10
    } __attribute__ ((packed));
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    union {
      uint8_t day;
      uint8_t date;
    };

    /**
     * Convert alarm time to binary representation (from BCD). 
     * Apply after reading from device and before any calculation.
     * Note that output stream operator requires BCD form.
     */
    void to_binary()
    {
      ::to_binary(&seconds, sizeof(alarm1_t));
    }

    /**
     * Convert alarm time to BCD representation (from binary).
     * Apply after setting new value and writing to the device.
     * Note that output stream operator requires BCD form.
     */
    void to_bcd()
    {
      ::to_bcd(&seconds, sizeof(alarm1_t));
    }
  };
  
  /**
   * Alarm2 register sub-set type and mask bits (Table 2, pp. 12)
   */
  struct alarm2_t {
    enum {
      ONCE_PER_MIN = 0x07,
      WHEN_MIN_MATCH = 0x06,
      WHEN_TIME_MATCH = 0x04,
      WHEN_DATE_TIME_MATCH = 0x00,
      WHEN_DAY_TIME_MATCH = 0x08
    } __attribute__ ((packed));
    uint8_t minutes;
    uint8_t hours;
    union {
      uint8_t day;
      uint8_t date;
    };

    /**
     * Convert alarm time to binary representation (from BCD). 
     * Apply after reading from device and before any calculation.
     * Note that output stream operator requires BCD form.
     */
    void to_binary()
    {
      ::to_binary(&minutes, sizeof(alarm2_t));
    }

    /**
     * Convert alarm time to BCD representation (from binary).
     * Apply after setting new value and writing to the device.
     * Note that output stream operator requires BCD form.
     */
    void to_bcd()
    {
      ::to_bcd(&minutes, sizeof(alarm2_t));
    }
  };
  
  /**
   * Special-Purpose Register: Control Register (pp. 13)
   */
  union control_t {
    uint8_t as_uint8;
    struct {
      uint8_t a1ie:1;		/**< Alarm 1 Interrupt Enable */
      uint8_t a2ie:1;		/**< Alarm 2 Interrupt Enable */
      uint8_t intcn:1;		/**< Interrupt Control */
      uint8_t rs:2;		/**< Rate Select */
      uint8_t conv:1;		/**< Convert Temperature */
      uint8_t bbsqw:1;		/**< Battery-Backup Square-Wave Enable */
      uint8_t eosc:1;		/**< Enable Oscillator */
    };
  };

  /**
   * Special-Purpose Register: Status Register (pp. 14)
   */
  union status_t {
    uint8_t as_uint8;
    struct {
      uint8_t a1f:1;		/**< Alarm 1 Flag */
      uint8_t a2f:1;		/**< Alarm 2 Flag */
      uint8_t bsy:1;		/**< Busy */
      uint8_t en32khz:1;	/**< Enable 32kHz Output */
      uint8_t reserved:3;	/**< Reserved(0) */
      uint8_t osf:1;		/**< Oscillator Stop Flag */
    };
  };

  /**
   * The Timekeeper Registers (Figure 1. pp. 11)
   */
  struct timekeeper_t {
    time_t clock;
    alarm1_t alarm1;
    alarm2_t alarm2;
    control_t control;
    status_t status;
    int8_t aging;
    int16_t temp;
  };
  static const uint8_t ALARM1_OFFSET = sizeof(time_t);
  static const uint8_t ALARM2_OFFSET = ALARM1_OFFSET + sizeof(alarm1_t);
  static const uint8_t TEMP_OFFSET = sizeof(timekeeper_t) - sizeof(int16_t);

  /**
   * Construct DS3231 device with bus address(0x68).
   */
  DS3231() : 
    TWI::Driver(0x68) 
  {
  }

  /**
   * Read register block with the given size into the buffer from the
   * position. Return number of bytes read or negative error code.
   * @param[in] regs buffer to read from register block.
   * @param[in] size number of bytes to read.
   * @param[in] pos address in register file to read from.
   * @return number of bytes or negative error code.
   */
  int read(void* regs, uint8_t size, uint8_t pos = 0);

  /**
   * Write register block at given position with the contents from
   * buffer. Return number of bytes written or negative error code. 
   * @param[in] regs buffer to write to register block.
   * @param[in] size number of bytes to write.
   * @param[in] pos address in register file to read write to.
   * @return number of bytes or negative error code.
   */
  int write(void* regs, uint8_t size, uint8_t pos = 0);

  /**
   * Read current time from real-time clock. Return true(1) if
   * successful otherwise false(0). 
   * @param[out] now time structure return value.
   * @return boolean.
   */
  bool get_time(time_t& now) 
  {
    return (read(&now, sizeof(now)) == sizeof(now));
  }

  /**
   * Set the real-time clock to the given time. Return true(1) if
   * successful otherwise false(0). 
   * @param[in] now time structure to set.
   * @return boolean.
   */
  bool set_time(time_t& now)
  {
    return (write(&now, sizeof(now)) == sizeof(now));
  }

  /**
   * Read alarm1 setting, time and mask, from real-time clock. Return
   * true(1) if successful otherwise false(0).
   * @param[out] alarm time structure.
   * @param[out] mask alarm mask bits.
   * @return boolean.
   */
  bool get_alarm1(alarm1_t& alarm, uint8_t& mask)
  {
    return (get(&alarm, sizeof(alarm), ALARM1_OFFSET, mask));
  }

  /**
   * Set real-time clock alarm1 with the given time and mask. Return
   * true(1) if successful otherwise false(0). 
   * @param[in] alarm time structure to set.
   * @param[out] mask alarm mask bits.
   * @return boolean.
   */
  bool set_alarm1(alarm1_t& alarm, uint8_t mask)
  {
    return (set(&alarm, sizeof(alarm), ALARM1_OFFSET, mask));
  }

  /**
   * Read alarm2 setting, time and mask, from real-time clock. Return
   * true(1) if successful otherwise false(0).
   * @param[out] alarm time structure.
   * @param[out] mask alarm mask bits.
   * @return boolean.
   */
  bool get_alarm2(alarm2_t& alarm, uint8_t& mask)
  {
    return (get(&alarm, sizeof(alarm), ALARM2_OFFSET, mask));
  }

  /**
   * Set real-time clock alarm with the given time and mask. Return
   * true(1) if successful otherwise false(0). 
   * @param[in] alarm time structure to set.
   * @param[out] mask alarm mask bits.
   * @return boolean.
   */
  bool set_alarm2(alarm2_t& alarm, uint8_t mask)
  {
    return (set(&alarm, sizeof(alarm), ALARM2_OFFSET, mask));
  }

  /**
   * Read temperature from real-time clock, signed fixpoint<8:2>
   * @return temperature.
   */
  int16_t get_temperature();

private:
  /**
   * Read alarm setting, time and mask, from real-time clock. Return
   * true(1) if successful otherwise false(0).
   * @param[out] alarm time structure.
   * @param[in] size of alarm time structure.
   * @param[in] offset to alarm time structure in timekeeper.
   * @param[out] mask alarm mask bits.
   * @return boolean.
   */
  bool get(void* alarm, uint8_t size, uint8_t offset, uint8_t& mask);

  /**
   * Set real-time clock alarm with the given time and mask. Return
   * true(1) if successful otherwise false(0). 
   * @param[in] alarm time structure to set.
   * @param[in] size of alarm time structure.
   * @param[in] offset to alarm time structure in timekeeper.
   * @param[out] mask alarm mask bits.
   * @return boolean.
   */
  bool set(void* alarm, uint8_t size, uint8_t offset, uint8_t mask);
};

/**
 * Print the alarm to the given stream with the format (DD HH:MM:SS).
 * The values should be in BCD i.e. not converted to binary. 
 * @param[in] outs output stream.
 * @param[in] t alarm time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, DS3231::alarm1_t& t);

/**
 * Print the alarm to the given stream with the format (DD HH:MM).
 * The values should be in BCD i.e. not converted to binary. 
 * @param[in] outs output stream.
 * @param[in] t alarm time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, DS3231::alarm2_t& t);

/**
 * Print the timekeeper structure to the given stream. The timekeeper
 * values should be in BCD i.e. not converted to binary.  
 * @param[in] outs output stream.
 * @param[in] t alarm time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, DS3231::timekeeper_t& t);

#endif
