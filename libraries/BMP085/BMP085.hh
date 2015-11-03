/**
 * @file BMP085.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013-2015, Mikael Patel
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

#ifndef COSA_BMP085_HH
#define COSA_BMP085_HH

#include "Cosa/Types.h"
#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa TWI driver for Bosch BMP085 Digital pressure sensor
 *
 * @section Circuit
 * The GY-80 10DOF module with pull-up resistors (4K7) for TWI signals and
 * 3V3 internal voltage converter.
 * @code
 *                           GY-80
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 *                     2-|3V3         |
 * (GND)---------------3-|GND         |
 * (A5/SCL)------------4-|SCL         |
 * (A4/SDA)------------5-|SDA         |
 *                     6-|M-DRDY      |
 *                     7-|A-INT1      |
 *                     8-|T-INT1      |
 *                     9-|P-XCLR      |
 * (Dn/EXTn)----------10-|P-EOC       |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. http://media.digikey.com/pdf/Data%20Sheets/Bosch/BMP085.pdf
 * BST-BMP085-DS000-03, Rev. 1.0, 01 July 2008.
 */
class BMP085 : private TWI::Driver {
public:
  /**
   * Oversampling modes (table, pp. 10).
   */
  enum Mode {
    ULTRA_LOW_POWER = 0,
    STANDARD = 1,
    HIGH_RESOLUTION = 2,
    ULTRA_HIGH_RESOLUTION = 3
  } __attribute__((packed));

  /**
   * Construct BMP085 driver with I2C address(0x77) and default
   * ULTRA_LOW_POWER mode.
   */
  BMP085() :
    TWI::Driver(0x77),
    m_mode(ULTRA_LOW_POWER),
    m_cmd(0),
    m_start(0),
    B5(0),
    m_pressure(0)
  {}

  /**
   * Initiate device driver. Load calibration coefficients from device.
   * Return true if successful otherwise false.
   * @param[in] mode oversampling mode (Default ULTRA_LOW_POWER).
   * @return boolean.
   */
  bool begin(Mode mode = ULTRA_LOW_POWER);

  /**
   * Issue a sample raw temperature sensor request. Return true(1) if
   * successful otherwise false.
   * @return bool
   */
  bool sample_temperature_request();

  /**
   * Read the raw temperature sensor. Will wait for the conversion to
   * complete. Return true(1) if successful otherwise false.
   * @return bool
   */
  bool read_temperature();

  /**
   * Sample the raw temperature sensor. Return true(1) if successful
   * otherwise false.
   * @return bool
   */
  bool sample_temperature()
  {
    return (sample_temperature_request() && read_temperature());
  }

  /**
   * Issue a sample request of the raw pressure sensor. Will wait for
   * the conversion to complete. Return true(1) if successful otherwise false.
   * @return bool
   */
  bool sample_pressure_request();

  /**
   * Read the raw pressure sensor. Return true(1) if successful
   * otherwise false.
   * @return bool
   */
  bool read_pressure();

  /**
   * Sample and read the raw pressure sensor. Return true(1) if
   * successful otherwise false.
   * @return bool
   */
  bool sample_pressure()
    __attribute__((always_inline))
  {
    return (sample_pressure_request() && read_pressure());
  }

  /**
   * Sample and read the raw temperature and pressure sensor. Return
   * true(1) if successful otherwise false.
   * @return bool
   */
  bool sample()
    __attribute__((always_inline))
  {
    return (sample_temperature() && sample_pressure());
  }

  /**
   * Calculate temperature from the latest raw sensor reading.
   * @return calculated temperature in steps of 0.1 C
   */
  int16_t temperature() const
    __attribute__((always_inline))
  {
    return ((B5 + 8) >> 4);
  }

  /**
   * Return latest calculated pressure from temperature and pressure
   * raw sensor data.
   * @return calculated pressure in steps of 1 Pa (0,01 hPa).
   */
  int32_t pressure() const
  {
    return (m_pressure);
  }

protected:
  /** Temperature conversion time max (ms). */
  static const uint8_t TEMP_CONV_MS = 5;

  /** Pressure conversion time max table (ms), index with mode. */
  static const uint8_t PRESSURE_CONV_MS[] __PROGMEM;

  /**
   * Calibration coefficients (chap. 3.4, pp. 11). Data from device is
   * in big-endian order.
   */
  struct param_t {
    int16_t ac1;
    int16_t ac2;
    int16_t ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
    int16_t mb;
    int16_t mc;
    int16_t md;
  };

  /** Calibration coefficients register address. */
  static const uint8_t COEFF_REG = 0xAA;

  /** Command register address. */
  static const uint8_t CMD_REG = 0xF4;

  /** Result register address. */
  static const uint8_t RES_REG = 0xF6;

  /** Temperature conversion command. */
  static const uint8_t TEMP_CONV_CMD = 0x2E;

  /** Pressure conversion command. */
  static const uint8_t PRESSURE_CONV_CMD = 0x34;

  /** Device calibration data. */
  param_t m_param;

  /** Pressure conversion mode. */
  Mode m_mode;

  /** Currrent command. */
  uint8_t m_cmd;

  /** Sample request start time (ms). */
  uint16_t m_start;

  /** Common intermediate temperature factor. */
  int32_t B5;

  /** Latest calculated pressure. */
  int32_t m_pressure;
};

/**
 * Print the latest reading to the given output stream.
 * @param[in] outs output stream.
 * @param[in] bmp instance.
 * @return output stream.
 */
extern IOStream& operator<<(IOStream& outs, BMP085& bmp);

#endif
