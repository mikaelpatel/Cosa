/**
 * @file Cosa/TWI/Driver/HMC5883L.hh
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

#ifndef __COSA_TWI_DRIVER_HMC5883L_HH__
#define __COSA_TWI_DRIVER_HMC5883L_HH__

#include "Cosa/TWI.hh"

/**
 * Driver for the HMC5883L 3-Axis Digital Compass IC.
 *
 * @section See Also
 * See Honeywell product description (Rev. E, February 2013).
 */
class HMC5883L : public TWI::Driver {
public:
  // Data output structure 
  struct data_t {
    int16_t x;
    int16_t y;
    int16_t z;
  };
  
protected:
  /**
   * Two-wire address for HMC5883L (pp 11)
   */
  static const uint8_t ADDR = 0x1e;

  /**
   * Masks for the different configuration and mode fields
   */
  enum {
    BIAS_MASK = 0x03,
    OUTPUT_RATE_MASK = 0x1c,
    SAMPLES_AVG_MASK = 0x60,
    RANGE_MASK = 0xe0,
    MEASUREMENT_MODE_MASK = 0x03
  } __attribute__((packed));

  /**
   * Register List (Table 2, pp 11)
   */
  struct reg_t {
    uint8_t config[2];
    uint8_t mode;
    data_t output;
    uint8_t status;
    uint8_t id[3];
  };
  reg_t m_reg;

public:
  /**
   * Configuration Register A (pp. 12)
   */
  enum Bias {
    NORMAL_BIAS = 0x00,
    POSITIVE_BIAS = 0x01,
    NEGATIVE_BIAS = 0x02
  } __attribute__((packed));

  enum Rate {
    OUTPUT_RATE_0_75_HZ = 0x00,
    OUTPUT_RATE_1_5_HZ = 0x04,
    OUTPUT_RATE_3_HZ = 0x08,
    OUTPUT_RATE_7_5_HZ = 0x0c,
    OUTPUT_RATE_15_HZ = 0x10,
    OUTPUT_RATE_30_HZ = 0x14,
    OUTPUT_RATE_75_HZ = 0x18
  } __attribute__((packed));

  enum Avg {
    SAMPLES_AVG_1 = 0x00,
    SAMPLES_AVG_2 = 0x20,
    SAMPLES_AVG_4 = 0x40,
    SAMPLES_AVG_8 = 0x60
  } __attribute__((packed));
  
  /**
   * Configuration Register B (pp. 13)
   */
  enum Range {
    RANGE_0_88_GA = 0x00,
    RANGE_1_3_GA = 0x20,
    RANGE_1_9_GA = 0x40,
    RANGE_2_5_GA = 0x60,
    RANGE_4_0_GA = 0x80,
    RANGE_4_7_GA = 0xa0,
    RANGE_5_6_GA = 0xc0,
    RANGE_8_1_GA = 0xe0
  } __attribute__((packed));

  /**
   * Mode Register (pp. 14)
   */
  enum Mode {
    CONTINOUS_MEASUREMENT_MODE = 0x00,
    SINGLE_MEASUREMENT_MODE = 0x01,
    IDLE_MEASUREMENT_MODE = 0x02,
  } __attribute__((packed));

  /**
   * Status Register (pp. 16)
   */
  enum Status {
    READY_STATUS = 0x01,
    LOCK_STATUS = 0x02
  } __attribute__((packed));

  /**
   * Begin interaction with device. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  bool begin();
  
  /**
   * End interaction with device. Put into idle mode. Return true(1)
   * if successful otherwise false(0).
   * @return bool
   */
  bool end();
  
  /**
   * Set device bias pin configuration. Call write_config() after
   * configuration changes.
   * @param[in] bias current configuration.
   */
  void set_bias(Bias bias);

  /**
   * Set device output rates in continous measurement mode. Call
   * write_config() after configuration changes.
   * @param[in] rate of data output.
   */
  void set_output_rate(Rate rate);

  /**
   * Set number of samples averaged per measurement output. Call
   * write_config() after configuration changes.
   * @param[in] avg number of samples.
   */
  void set_sample_avg(Avg avg);

  /**
   * Set gain configuration bits for a given recommended sensor field
   * range. Call write_config() after configuration changes.
   * @param[in] range gauss.
   */
  void set_range(Range range);

  /**
   * Write configuration change to device. Return true(1) if successful
   * otherwise false(0). Should be called after configuration
   * changes. 
   * @return bool
   */
  bool write_config();

  /**
   * Set device operation mode and write mode change to device. Return
   * true(1) if successful otherwise false(0). Must be called for each
   * sample in SINGLE_MEASUREMENT_MODE. There is a delay before the
   * output data becomes available from the device (rate setting). 
   * Monitor with available() or the ready pin. 
   * @param[in] mode of operation.
   * @return bool
   */
  bool set_mode(Mode mode);

  /**
   * Read status from device. Return true(1) if successful
   * otherwise false(0).
   * @param[out] status.
   * @return bool
   */
  bool read_status(Status& status);

  /**
   * Return true(1) if output data is available otherwise false(0).
   * @return bool
   */
  bool available()
  {
    Status current;
    if (!read_status(current)) return (false);
    return ((current & HMC5883L::READY_STATUS) != 0);
  }
  
  /**
   * Read output data from device. Return true(1) if successful
   * otherwise false(0).
   * @param[out] data.
   * @return bool
   */
  bool read_data(data_t& data);
};

#endif
