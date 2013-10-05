/**
 * @file Cosa/TWI/Driver/BMP085.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2013, Mikael Patel
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

#ifndef __COSA_TWI_DRIVER_BMP085_HH__
#define __COSA_TWI_DRIVER_BMP085_HH__

#include "Cosa/TWI.hh"

/**
 * Cosa TWI driver for Bosch BMP085 Digital pressure sensor
 * http://media.digikey.com/pdf/Data%20Sheets/Bosch/BMP085.pdf
 * BST-BMP085-DS000-03, 01 July 2008.
 *
 * @see Acknowledgements
 * Inspired by the improved Adafruit BMP085 library by Michal
 * Canecky/Cano.  
 */
class BMP085 : private TWI::Driver {
public:
  /**
   * Oversampling modes (table, pp. 10)
   */
  enum Mode {
    ULTRA_LOW_POWER = 0,
    STANDARD = 1,
    HIGH_RESOLUTION = 2,
    ULTRA_HIGH_RESOLUTION = 3
  } __attribute__((packed));

protected:
  /**
   * Temperature conversion time max (us).
   */
  static const uint16_t TEMP_CONV_US = 4500;

  /**
   * Pressure conversion time max (us).
   */
  static const uint16_t PRESSURE_CONV_US[] __PROGMEM;

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

  /** Calibration coefficients register address */
  static const uint8_t COEFF_REG = 0xAA;
  /** Command register address */
  static const uint8_t CMD_REG = 0xF4;
  /** Result register address */
  static const uint8_t RES_REG = 0xF6;

  /** Temperature conversion command */
  static const uint8_t TEMP_CONV_CMD = 0x2E;
  /** Pressure conversion command */
  static const uint8_t PRESSURE_CONV_CMD = 0x34;

  /** Pressure conversion mode */
  Mode m_mode;
  /** Device calibration data */
  param_t m_param;

public:
  /**
   * Construct BMP085 driver with I2C address(0x77).
   * @param[in] mode of operation (default ULTRA_HIGH_RESOLUTION).
   */
  BMP085(Mode mode = ULTRA_HIGH_RESOLUTION) : 
    TWI::Driver(0x77),
    m_mode(mode)
  {}

  /**
   * Set mode of operation.
   * @param[in] mode.
   */
  void set(Mode mode)
  {
    m_mode = mode;
  }

  /**
   * Initiate device driver. Load calibration coefficients.
   */
  bool begin();

  /**
   * Sample the raw temperature sensor. Return true(1) if successful 
   * otherwise false.
   * @param[out] UT temperature reading.
   * @return bool
   */
  bool sample(int32_t& UT);

  /**
   * Sample the raw pressure sensor. Return true(1) if successful 
   * otherwise false.
   * @param[out] UP pressure reading.
   * @return bool
   */
  bool sample(uint32_t& UP);

  /**
   * Calculate temperature from raw sensor reading.
   * @param[in] UT raw temperature reading.
   * @return calculated temperature.
   */
  int16_t calculate(int32_t UT);

  /**
   * Calculate pressure from raw sensor readings.
   * @param[in] UP raw pressure reading.
   * @param[in] UT raw temperature reading.
   * @return calculated temperature.
   */
  uint32_t calculate(uint32_t UP, int32_t UT);
};
#endif
