/**
 * @file Si70XX.hh
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

#ifndef COSA_Si70XX_HH
#define COSA_Si70XX_HH

#include "Cosa/TWI.hh"
#include <math.h>

/**
 * Cosa TWI driver for Silicon Labs, Si70XX I2C Humidity and
 * Temperature Sensor. The device driver does not block on
 * measurements.
 *
 * @section Circuit
 * The GY-21 module with pull-up resistors for TWI signals and 3V3
 * internal voltage converter.
 * @code
 *                           GY-21
 *                       +------------+
 * (VCC)---------------1-|VIN     ( ) |
 * (VCC)---------------2-|GND         |
 * (A5/SCL)------------3-|SCL         |
 * (A4/SDA)------------4-|SDA         |
 *                       +------------+
 * @endcode

 * @section References
 * 1. http://www.silabs.com/products/sensors/humidity-sensors/Pages/si7013-20-21.aspx
 * 2. https://www.silabs.com/Support%20Documents/TechnicalDocs/Si7020-A20.pdf, Rev. 1.1 6/15.
 */
class Si70XX : private TWI::Driver {
public:
  /**
   * Create device driver instance.
   */
  Si70XX() : TWI::Driver(0x40) {}

  /**
   * Read configuration register, Return true(1) if successful
   * otherwise false(0).
   * @param[out] reg value.
   * @return bool.
   */
  bool read_user_register(uint8_t &reg)
  {
    return (read(READ_RHT_USER_REG_1, reg));
  }

  /**
   * Read electronic serial number, Return true(1) if successful
   * otherwise false(0).
   * @param[out] snr serial number register (64-bits).
   * @return bool.
   */
  bool read_electronic_serial_number(uint8_t snr[8]);

  /**
   * Read firmware revision, Return true(1) if successful otherwise
   * false(0).
   * @param[out] rev revision code.
   * @return bool.
   */
  bool read_firmware_revision(uint8_t &rev);

  /**
   * Issue a humidity measurement. Call read_humidity() for result of
   * measurement. Return true(1) if successful otherwise false(0).
   * @return bool.
   */
  bool measure_humidity()
  {
    return (issue(MEASURE_RH_NO_HOLD));
  }

  /**
   * Read humidity value after issued measurement, Return relativ
   * humidity.
   * @return relative humidity.
   */
  float read_humidity()
  {
    uint16_t value;
    if (!read(value)) (NAN);
    return (((125.00 * value) / 65536) - 6.00);
  }

  /**
   * Read temperature from humidity measurement, Return temperature
   * in Celcius.
   * @return temperature.
   */
  float read_humidity_temperature()
  {
    uint16_t value;
    if (!issue(READ_RH_TEMP)) return (NAN);
    if (!read(value, false)) return (NAN);
    return (((175.72 * value) / 65536) - 46.85);
  }

  /**
   * Issue a temperature measurement. Call read_temperature() for
   * result of measurement. Return true(1) if successful otherwise
   * false(0).
   * @return bool.
   */
  bool measure_temperature()
  {
    return (issue(MEASURE_TEMP_NO_HOLD));
  }

  /**
   * Read temperature from issued measurement, Return temperature in
   * Celcius.
   * @return temperature.
   */
  float read_temperature()
  {
    uint16_t value;
    if (!read(value)) return (NAN);
    return (((175.72 * value) / 65536) - 46.85);
  }

protected:
  /**
   * I2C Command Table (See tab. 11, pp. 19).
   */
  enum Command {
    MEASURE_RH_HOLD = 0xE5,      //!< Measure Relative Humidity, Hold Master Mode
    MEASURE_RH_NO_HOLD = 0xF5,   //!< Dito, No Hold Master Mode
    MEASURE_TEMP_HOLD = 0xE3,	 //!< Measure Temperature, Hold Master Mode
    MEASURE_TEMP_NO_HOLD = 0xF3, //!< Dito, No Hold Master Mode
    READ_RH_TEMP = 0xE0,     	 //!< Read Temperature from RH Measurement
    RESET = 0xFE,		 //!< Reset
    WRITE_RHT_USER_REG_1 = 0xE6, //!< Write RH/T User Register 1
    READ_RHT_USER_REG_1 = 0xE7,	 //!< Read RH/T User Register 1
    READ_ID_1 = 0xFA0F,		 //!< Read Electronic ID 1
    READ_ID_2 = 0xFCC8,		 //!< Read Electronic ID 2
    READ_REV = 0x84B8		 //!< Read Firmware Revision
  } __attribute__((packed));

  /**
   * Issue given command. Return true(1) if successful otherwise false(0).
   * @param[in] cmd command.
   * @return bool.
   */
  bool issue(uint8_t cmd);

  /**
   * Read 16-bit value after issued command. Return true(1) if
   * successful otherwise false(0).
   * @param[out] value register value.
   * @param[in] check crc value (default true).
   * @return bool.
   */
  bool read(uint16_t& value, bool check = true);

  /**
   * Read 8-bit register value given command. Return true(1) if
   * successful otherwise false(0).
   * @param[in] cmd command.
   * @param[out] value register value.
   * @return bool.
   */
  bool read(uint8_t cmd, uint8_t& value);
};

#endif
