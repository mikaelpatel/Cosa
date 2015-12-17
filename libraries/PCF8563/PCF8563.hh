/**
 * @file PCF8563.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_PCF8563_HH
#define COSA_PCF8563_HH

#include "Cosa/TWI.hh"
#include "Cosa/Time.hh"
#include "Cosa/IOStream.hh"

/**
 * Driver for the PCF8563, Low-Power I2C Real-Time Clock/Calendar (RTCC).
 *
 * @section References
 * 1. NXP PCF8563 data sheet;
 * http://www.nxp.com/documents/data_sheet/PCF8563.pdf
 */
class PCF8563 : private TWI::Driver {
public:
  /**
   * Construct PCF8563 device with bus address(0x51).
   */
  PCF8563() :
    TWI::Driver(0x51)
  {}

  /**
   * Read current time from the real-time clock. Return true(1) if
   * successful otherwise false(0).
   * @param[out] now time structure return value.
   * @return boolean.
   */
  bool get_time(time_t& now);

  /**
   * Set the real-time clock to the given time. Return true(1) if
   * successful otherwise false(0).
   * @param[in] now time structure to set.
   * @return boolean.
   */
  bool set_time(time_t& now);

  /**
   * The Alarm setting (pp. 13-14).
   */
  struct alarm_t {
    uint8_t minutes;		//!< 00-59 Minutes (BCD).
    uint8_t hours;		//!< 00-23 Hours (BCD).
    uint8_t date;		//!< 01-31 Date (BCD).
    uint8_t day;		//!< 00-06 Day (BCD).

    /** Disable alarm match value. */
    static const uint8_t DISABLE = 0x80;

    /**
     * Default constructor will disable all alarm fields.
     * Assign BCD value(s) to enable alarm field(s).
     */
    alarm_t()
    {
      minutes = DISABLE;
      hours = DISABLE;
      date = DISABLE;
      day = DISABLE;
    }
  };

  /**
   * Set the real-time clock alarm. Return true(1) if successful
   * otherwise false(0).
   * @param[in] alarm match structure to set.
   * @return boolean.
   */
  bool set_alarm(alarm_t& alarm);

  /**
   * Read the real-time clock alarm setting. Return true(1) if
   * successful otherwise false(0).
   * @param[out] alarm match structure.
   * @return boolean.
   */
  bool get_alarm(alarm_t& alarm);

  /**
   * Clear the real-time clock alarm setting. Return true(1) if
   * successful otherwise false(0).
   * @return boolean.
   */
  bool clear_alarm();

  /**
   * Check any pending alarms (signalled on interrupt pin). Returns
   * true(1) alarm pending otherwise false(0).
   * @return boolean.
   */
  bool pending_alarm();

protected:
  /**
   * The RTCC control and status register 1 bitfields (pp. 7).
   */
  union control_status_1_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access.
      uint8_t N1:3;		//!< Unused.
      uint8_t TESTC:1;		//!< Power-On Reset override.
      uint8_t N2:1;		//!< Unused.
      uint8_t STOP:1;		//!< RTC source clock run.
      uint8_t N3:1;		//!< Unused.
      uint8_t TEST1:1;		//!< Normal mode(0), test mode(1).
    };
    control_status_1_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
  };

  /**
   * The RTCC control and status register 2 bitfields (pp. 7-8).
   */
  union control_status_2_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access.
      uint8_t TIE:1;		//!< Timer interrupt enable.
      uint8_t AIE:1;		//!< Alarm interrupt enable.
      uint8_t TF:1;		//!< Timer flag.
      uint8_t AF:1;		//!< Alarm flag.
      uint8_t TI_TP:1;		//!< Timer pulse interrupt enable.
      uint8_t N:3;		//!< Unused.
    };
    control_status_2_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
  };

  /**
   * The RTCC CLKOUT control and clock register bitfields (pp. 15).
   */
  union clock_control_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access.
      uint8_t FD:2;		//!< Clock output frequency.
      uint8_t unused1:5;	//!< Unused.
      uint8_t FE:1;		//!< Clock output enable.
    };
    clock_control_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
  };

  /**
   * Frequency output in control register (FD, pp. 15).
   */
  enum {
    FD_32_768_KHZ,		//!< 32.768 Hz
    FD_1_024_KHZ,		//!< 1.024 Hz
    FD_32_HZ,			//!< 32 Hz
    FD_1_HZ			//!< 1 Hz
  } __attribute__((packed));

  /**
   * The RTCC Timer control register bitfields (pp. 16).
   */
  union timer_control_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access.
      uint8_t TD:2;		//!< Timer frequency.
      uint8_t unused1:5;	//!< Unused.
      uint8_t TE:1;		//!< Timer enable.
    };
    timer_control_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
  };

  /**
   * Timer frequency in control register (TD, pp. 16).
   */
  enum {
    TD_4_096_KHZ,		//!< 4.096 Hz.
    TD_64_HZ,			//!< 64 Hz.
    TD_1_HZ,			//!< 1 Hz.
    TD_1v60_HZ			//!< 1/60 Hz.
  } __attribute__((packed));

  /**
   * The RTCC Register Map (Fig 1. pp. 3).
   */
  struct rtcc_t {
    control_status_1_t cs1;	//!< Control Status 1.
    control_status_2_t cs2;	//!< Control Status 2.
    time_t clock;		//!< Real-Time Clock.
    alarm_t alarm;		//!< Alarm Function.
    clock_control_t cc;		//!< Clock Output Control.
    timer_control_t tc;		//!< Timer Control.
    uint8_t timer;		//!< Timer Period.
  };

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
};

/**
 * Print alarm setting to given output stream.
 * @param[in] cout output stream.
 * @param[in] alarm setting.
 * @return output stream.
 */
IOStream& operator<<(IOStream& cout, PCF8563::alarm_t &alarm);

#endif
