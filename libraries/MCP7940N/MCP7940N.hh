/**
 * @file MCP7940N.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
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

#ifndef COSA_MCP7940N_HH
#define COSA_MCP7940N_HH

#include "Cosa/TWI.hh"
#include "Cosa/Time.hh"
#include "Cosa/IOStream.hh"
#include "Cosa/ExternalInterrupt.hh"

/**
 * Driver for the MCP7940N, Low-Cost I2C Real-Time Clock/Calendar (RTCC)
 * with SRAM and Battery Switchover.
 *
 * @section References
 * 1. Microchip MCP7940N data sheet;
 * http://ww1.microchip.com/downloads/en/DeviceDoc/20005010F.pdf
 */
class MCP7940N : private TWI::Driver {
public:
  /**
   * The RTCC configuration/status bitfields. Embedded in day field (pp. 18).
   */
  union config_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access.
      uint8_t day:3;		//!< Day in 3 bits (1..7).
      uint8_t vbaten:1;		//!< External Battery Enable.
      uint8_t vbat:1;		//!< Battey Used Status.
      uint8_t oscon:1;		//!< Oscillator Running Status.
      uint8_t reserved:2;	//!< Reserved.
    };
    config_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
  };

  /**
   * The RTCC control register bitfields (pp. 26).
   */
  union control_t {
    uint8_t as_uint8;		//!< Unsigned byte access.
    struct {			//!< Bitfield access.
      uint8_t sqwfs:2;		//!< Square Wave Clock Output Frequency.
      uint8_t crstrim:1;	//!< Coarse Trim Mode Enable bit.
      uint8_t extosc:1;		//!< External Oscillator Input bit.
      uint8_t alm0en:1;		//!< Alarm 0 Module Enable.
      uint8_t alm1en:1;		//!< Alarm 1 Module Enable.
      uint8_t sqwen:1;		//!< Square-Ware Enable.
      uint8_t out:1;		//!< Output Control.
    };
    control_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
  };

  /**
   * Rate Selection in control register (rs, pp. 10).
   */
  enum {
    SQW_1_HZ,			//!< 1 Hz
    SQW_4_096_KHZ,		//!< 4.096 Hz
    SQW_8_192_KHZ,		//!< 8.192 Hz
    SQW_32_768_KHZ		//!< 32.768 Hz
  } __attribute__((packed));

  /**
   * The Alarm setting (pp. 9-11).
   */
  struct alarm_t {
    uint8_t seconds;		//!< 00-59 Seconds.
    uint8_t minutes;		//!< 00-59 Minutes.
    uint8_t hours;		//!< 00-23 Hours.
    uint8_t day;		//!< 01-7 Day.
    uint8_t date;		//!< 01-31 Date.
    uint8_t month;		//!< 01-12 Month.

    /**
     * Convert time to binary representation (from BCD).
     * Apply after reading from device and before any calculation.
     */
    void to_binary()
      __attribute__((always_inline))
    {
      ::to_binary(&seconds, sizeof(alarm_t));
    }

    /**
     * Convert time to BCD representation (from binary).
     * Apply after setting new value and writing to the device.
     */
    void to_bcd()
      __attribute__((always_inline))
    {
      ::to_bcd(&seconds, sizeof(alarm_t));
    }

    /**
     * Alarm configuration/status register (part of day value, pp. 11)
     */
    union config_t {		//!< Day, alarm configuration and status.
      uint8_t as_uint8;		//!< Unsigned byte access.
      struct {			//!< Bitfield access.
	uint8_t day:3;		//!< Day in BCD (1-7).
	uint8_t triggered:1;	//!< Alarm Interrupt Flag.
	uint8_t when:3;		//!< Alarm Match.
	uint8_t polarity:1;	//!< Alarm Pin (MFP) Polarity.
      };
      config_t(uint8_t value = 0)
      {
	as_uint8 = value;
      }
    };
  };

  /**
   * Alarm match types (pp. 10-11).
   */
  enum {
    WHEN_SEC_MATCH = 0x00,	//!< Seconds match.
    WHEN_MIN_MATCH = 0x01,	//!< Minutes match.
    WHEN_HOUR_MATCH = 0x02,	//!< Hours match (takes into accout 12/24 hour).
    WHEN_DAY_MATCH = 0x03,	//!< Matches the current Day.
    WHEN_DATE_MATCH = 0x04,	//!< Date.
    WHEN_TIME_MATCH = 0x07	//!< Sec, Min, Hour, Day, Date, Month.
  } __attribute__ ((packed));

  /**
   * Timestamp type (pp. 9-10). Same as time_t except without year.
   */
  struct timestamp_t {
    uint8_t seconds;		//!< 00-59 Seconds.
    uint8_t minutes;		//!< 00-59 Minutes.
    uint8_t hours;		//!< 00-23 Hours.
    uint8_t day;		//!< 01-07 Day.
    uint8_t date;		//!< 01-31 Date.
    uint8_t month;		//!< 01-12 Month.

    /**
     * Convert time to binary representation (from BCD).
     * Apply after reading from device and before any calculation.
     */
    void to_binary()
      __attribute__((always_inline))
    {
      ::to_binary(&seconds, sizeof(timestamp_t));
    }

    /**
     * Convert time to BCD representation (from binary).
     * Apply after setting new value and writing to the device.
     */
    void to_bcd()
      __attribute__((always_inline))
    {
      ::to_bcd(&seconds, sizeof(time_t));
    }
  };

  /**
   * The RTCC Memory Map (Table 4-1. pp. 9).
   */
  struct rtcc_t {
    time_t clock;		//!< Current time.
    control_t control;		//!< Device control register.
    int8_t calibration;		//!< Time calibration (in clock cycles).
    uint8_t reserved1;		//!< Reserved - do not use.
    alarm_t alarm0;		//!< Alarm 0 setting (time/configuration).
    uint8_t reserved2;		//!< Reserved - do not use.
    alarm_t alarm1;		//!< Alarm 1 setting (time/configuration).
    uint8_t reserved3;		//!< Reserved - do not use.
    timestamp_t vcc_failed;	//!< Timestamp when Vcc fails.
    timestamp_t vcc_restored;	//!< Timestamp when Vcc restored.
  };

  /** Start of application RAM. */
  const static uint8_t RAM_START = sizeof(rtcc_t);

  /** End of application RAM. */
  const static uint8_t RAM_END = 0x5f;

  /** Max size of application RAM (64 bytes). */
  const static uint8_t RAM_MAX = RAM_END - RAM_START + 1;

  /**
   * Construct MCP7940N device with bus address(0x6f).
   * @param[in] pin alarm interrupt pin (Default EXT1/EXT0).
   */
#if !defined(BOARD_ATTINY)
  MCP7940N(Board::ExternalInterruptPin pin = Board::EXT1) :
    TWI::Driver(0x6f),
    m_alarm_irq(pin)
  {}
#else
  MCP7940N(Board::ExternalInterruptPin pin = Board::EXT0) :
    TWI::Driver(0x6f),
    m_alarm_irq(pin)
  {}
#endif

  /**
   * Read current time from real-time clock. Return true(1) if
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
   * Set given real-time clock alarm with the given time and
   * configuration. Return true(1) if successful otherwise false(0).
   * @param[in] nr alarm number (0..1).
   * @param[in] alarm time structure to set.
   * @param[in] when alarm should trigger.
   * @return boolean.
   */
  bool set_alarm(uint8_t nr, time_t& alarm, uint8_t when);

  /**
   * Read given real-time clock alarm time and configuration. Return
   * true(1) if successful otherwise false(0).
   * @param[in] nr alarm number (0..1).
   * @param[out] alarm time structure.
   * @param[out] when alarm should trigger.
   * @return boolean.
   */
  bool get_alarm(uint8_t nr, time_t& alarm, uint8_t& when);

  /**
   * Clear given real-time clock alarm. Return true(1) if successful
   * otherwise false(0).
   * @param[in] nr alarm number (0..1).
   * @return boolean.
   */
  bool clear_alarm(uint8_t nr);

  /**
   * Check any pending alarms (signalled on interrupt pin). Returns
   * alarm pending (0 for no alarms, 1 for alarm0, 2 for alarm1 and 3
   * for both)
   * @return alarm triggered or zero for no alarms pending.
   */
  uint8_t pending_alarm();

  /**
   * Enable/disable square_wave generation.
   * @param[in] flag.
   */
  bool square_wave(bool flag);

protected:
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
   * Alarm Interrupt Handler.
   */
  class AlarmInterrupt : public ExternalInterrupt {
  public:
    AlarmInterrupt(Board::ExternalInterruptPin pin);
    virtual void on_interrupt(uint16_t arg = 0);
  protected:
    friend class MCP7940N;
    bool m_triggered;
  };

  /** Alarm Interrupt Pin */
  AlarmInterrupt m_alarm_irq;
};

/**
 * Print the alarm to the given stream with the format (DD HH:MM).
 * The values should be in BCD i.e. not converted to binary.
 * @param[in] outs output stream.
 * @param[in] t alarm time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, MCP7940N::alarm_t& t);

/**
 * Print the RTCC structure to the given stream. The RTCC
 * values should be in BCD i.e. not converted to binary.
 * @param[in] outs output stream.
 * @param[in] t alarm time structure.
 * @return iostream.
 */
IOStream& operator<<(IOStream& outs, MCP7940N::rtcc_t& t);
#endif
