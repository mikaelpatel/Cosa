/**
 * @file HMC5883L.hh
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

#ifndef COSA_HMC5883L_HH
#define COSA_HMC5883L_HH

#include "Cosa/TWI.hh"
#include "Cosa/Power.hh"
#include "Cosa/IOStream.hh"

/**
 * Driver for the HMC5883L 3-Axis Digital Compass IC, a vector
 * magnetometer.
 *
 * @section Circuit
 * The GY-80 10DOF module with pull-up resistors (4K7) for TWI signals
 * and 3V3 internal voltage converter.
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
 *                    10-|P-EOC       |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Honeywell product description (Rev. E, February 2013).
 * http://www51.honeywell.com/aero/common/documents/
 * myaerospacecatalog-documents/Defense_Brochures-documents/
 * HMC5883L_3-Axis_Digital_Compass_IC.pdf
 */
class HMC5883L : public TWI::Driver {
public:
  /**
   * Configuration Register A (pp. 12).
   */
  enum Bias {			//!< Table 6: Measurement Modes.
    NORMAL_BIAS = 0x00,
    POSITIVE_BIAS = 0x01,
    NEGATIVE_BIAS = 0x02
  } __attribute__((packed));

  enum Rate {			//!< Table 5: Data Output Rates.
    OUTPUT_RATE_0_75_HZ,
    OUTPUT_RATE_1_5_HZ,
    OUTPUT_RATE_3_HZ,
    OUTPUT_RATE_7_5_HZ,
    OUTPUT_RATE_15_HZ,
    OUTPUT_RATE_30_HZ,
    OUTPUT_RATE_75_HZ
  } __attribute__((packed));

  enum Avg {			//!< Number of averaged samples per output.
    SAMPLES_AVG_1,
    SAMPLES_AVG_2,
    SAMPLES_AVG_4,
    SAMPLES_AVG_8
  } __attribute__((packed));

  /**
   * Configuration Register B (pp. 13)
   */
  enum Range {			//!< Table 9: Gain Setting.
    RANGE_0_88_GA,
    RANGE_1_3_GA,
    RANGE_1_9_GA,
    RANGE_2_5_GA,
    RANGE_4_0_GA,
    RANGE_4_7_GA,
    RANGE_5_6_GA,
    RANGE_8_1_GA
  } __attribute__((packed));

  /**
   * Mode Register (pp. 14).
   */
  enum Mode {
    CONTINOUS_MEASUREMENT_MODE,
    SINGLE_MEASUREMENT_MODE,
    IDLE_MEASUREMENT_MODE
  } __attribute__((packed));

  /**
   * Status Register (pp. 16).
   */
  struct status_t {
    uint8_t ready:1;
    uint8_t lock:1;
    uint8_t reserved:6;
    status_t()
    {
      ready = 0;
      lock = 0;
      reserved = 0;
    }
  };

  /**
   * Data output structure.
   */
  struct data_t {
    int16_t x;
    int16_t y;
    int16_t z;
    data_t()
    {
      x = y = z = 0;
    }
  };

public:
  /**
   * Construct HMC5883L device with bus address(0x1e) (pp. 11).
   */
  HMC5883L(uint8_t mode = SLEEP_MODE_IDLE) :
    TWI::Driver(0x1e),
    m_config(),
    m_mode(mode),
    m_overflow(false),
    m_output()
  {}

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
  bool end()
  {
    return (mode(IDLE_MEASUREMENT_MODE));
  }

  /**
   * Set await sleep mode.
   * @param[in] mode power sleep mode.
   */
  void await_mode(uint8_t mode)
  {
    m_mode = mode;
  }

  /**
   * Set device bias pin configuration. Call write_config() after
   * configuration changes.
   * @param[in] bias current configuration.
   */
  void bias(Bias bias)
  {
    m_config.MS = bias;
  }

  /**
   * Set device output rates in continous measurement mode. Call
   * write_config() after configuration changes.
   * @param[in] rate of data output.
   */
  void output_rate(Rate rate)
  {
    m_config.DO = rate;
  }

  /**
   * Set number of samples averaged per measurement output. Call
   * write_config() after configuration changes.
   * @param[in] avg number of samples.
   */
  void samples_avg(Avg avg)
  {
    m_config.MA = avg;
  }

  /**
   * Set gain configuration bits for a given recommended sensor field
   * range. Call write_config() after configuration changes.
   * @param[in] range gauss.
   */
  void range(Range range)
  {
    m_config.GN = range;
  }

  /**
   * Write configuration change to device. Return true(1) if successful
   * otherwise false(0). Should be called after configuration
   * changes.
   * @return bool
   */
  bool write_config();

  /**
   * Get output data from device driver.
   * @param[out] data.
   */
  void heading(data_t& data) const
  {
    data = m_output;
  }

  /**
   * Set device operation mode and write mode change to device. Return
   * true(1) if successful otherwise false(0). Must be called for each
   * sample in SINGLE_MEASUREMENT_MODE. There is a delay before the
   * output data becomes available from the device (rate setting).
   * Monitor with available() or the ready pin.
   * @param[in] mode of operation.
   * @return bool
   */
  bool mode(Mode mode);

  /**
   * Read status from device. Return true(1) if successful
   * otherwise false(0).
   * @param[out] status.
   * @return bool
   */
  bool read_status(status_t& status);

  /**
   * Return true(1) if output data is available otherwise false(0).
   * @return bool
   */
  bool available()
    __attribute__((always_inline))
  {
    status_t current;
    if (!read_status(current)) return (false);
    return (current.ready);
  }

  /**
   * Issue single measurement. Return true(1) if successful otherwise
   * false(0).
   * @return bool
   */
  bool sample_heading_request()
  {
    return (mode(SINGLE_MEASUREMENT_MODE));
  }

  /**
   * Wait for measurement to complete. Processor is put to sleep with
   * the mode given by set_sleep_mode() or constructor.
   */
  void await()
    __attribute__((always_inline))
  {
    while (!available()) Power::sleep(m_mode);
  }

  /**
   * Read output data from device. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  bool read_heading();

  /**
   * Read output data from device. Return true(1) if successful
   * otherwise false(0).
   * @param[out] data.
   * @return bool
   */
  bool read_heading(data_t& data)
    __attribute__((always_inline))
  {
    if (!read_heading()) return (false);
    heading(data);
    return (true);
  }

  /**
   * Returns true(1) if the latest reading contained an overflow on
   * any of the channels.
   * @return bool.
   */
  bool is_overflow() const
  {
    return (m_overflow);
  }

  /**
   * Convert the latest reading to milli-gauss using the current gain
   * setting and the gain table.
   */
  void to_milli_gauss();

protected:
  /**
   * Register List (Table 2, pp 11).
   */
  enum Register {
    CONFIG = 0x00,		//!< Configuration register A, B.
    MODE = 0x02,		//!< Mode register.
    OUTPUT = 0x03,		//!< Output data register X, Y, Z.
    STATUS = 0x09,		//!< Status register.
    IDENTITY = 0x0a		//!< Identity register(0-2).
  } __attribute__((packed));

  /** Gain conversion table. */
  static const uint16_t s_gain[] PROGMEM;

  /** Configuration registers */
  struct config_t {
    uint8_t MS:2;		//!< Measurement configuration bits.
    uint8_t DO:3;		//!< Data output rate.
    uint8_t MA:2;		//!< Number of samples to average.
    uint8_t reserved:6;		//!< Reserved for future use.
    uint8_t GN:3;		//!< Gain configuration bits.
    config_t()			//!< Default configuration.
    {
      MS = NORMAL_BIAS;
      DO = OUTPUT_RATE_15_HZ;
      MA = SAMPLES_AVG_1;
      reserved = 0;
      GN = RANGE_1_3_GA;
    }
  };
  /** Configuration mirror register. */
  config_t m_config;

  /** Sleep mode while waiting for data. */
  uint8_t m_mode;

  /** Overflow detected. */
  bool m_overflow;

  /** Output register latest read. */
  data_t m_output;
};

/**
 * Print the latest reading to the given output stream.
 * @param[in] outs output stream.
 * @param[in] compass instance.
 * @return output stream.
 */
extern IOStream& operator<<(IOStream& outs, HMC5883L& compass);

#endif
