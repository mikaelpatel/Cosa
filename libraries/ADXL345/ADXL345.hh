/**
 * @file ADXL345.hh
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

#ifndef COSA_ADXL345_HH
#define COSA_ADXL345_HH

#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa TWI driver for Analog Devices ADXL345 Digital Accelerometer.
 * http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
 * Rev. D, 2/13.
 *
 * @section Circuit
 * The GY-291 module with pull-up resistors (4K7) for TWI signals and
 * 3V3 internal voltage converter.
 * @code
 *                           GY-291
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (VCC)---------------2-|VCC         |
 *                     3-|CS          |
 *                     4-|A-INT1      |
 *                     5-|A-INT2      |
 *                     6-|SDO         |
 * (A4/SDA)------------7-|SDA         |
 * (A5/SCL)------------8-|SCL         |
 *                       +------------+
 * @endcode
 */
class ADXL345 : private TWI::Driver {
public:

  /**
   * Construct ADXL345 driver with normal or alternative address (pp. 18).
   * @param[in] use_alt_address.
   */
  ADXL345(bool use_alt_address = false) :
    TWI::Driver(use_alt_address ? 0x53 : 0x1d)
  {}

  /**
   * Start interaction with device. Set full resolution and 16G.
   * Single and double tap detection in XYZ-axis. Activity/inactivity
   * (5 seconds), and free fall detect. Power control with auto-sleep
   * and wakeup at 2 Hz. Interrupts enabled. Measurement turned on.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin();

  /**
   * Stop sequence of interaction with device. Turn off measurement.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Accelerometer offset calibration structure.
   */
  struct offset_t {
    int8_t x;
    int8_t y;
    int8_t z;
  };

  /**
   * Calibrate accelerometer with given offsets.
   * @param[in] x axis offset.
   * @param[in] y axis offset.
   * @param[in] z axis offset.
   */
  void calibrate(int8_t x, int8_t y, int8_t z)
    __attribute__((always_inline))
  {
    offset_t ofs;
    ofs.x = x;
    ofs.y = y;
    ofs.z = z;
    write(OFS, &ofs, sizeof(ofs));
  }

  /**
   * Calibrate accelerometer by resetting offset and
   * using the current accelerometer values as offset.
   * (-sample/4) according to ADXL345 documentation.
   */
  void calibrate();

  /**
   * Accelerometer sample data structure (axis x, y, z).
   */
  struct sample_t {
    int x;
    int y;
    int z;
  };

  /**
   * Sample accelerometer. Return sample is given data structure
   * @param[in] s sample storage.
   */
  void sample(sample_t& s)
    __attribute__((always_inline))
  {
    read(DATA, &s, sizeof(s));
  }

  /**
   * Register INT_ENABLE/INT_MAP/INT_SOURCE bitfields.
   */
  enum {
    DATA_READY = 7,		//!< Data ready interrupt enable/map/source.
    SINGLE_TAP = 6, 		//!< Single tap.
    DOUBLE_TAP = 5,		//!< Double tap.
    ACT = 4,			//!< Activity.
    INACT = 3, 			//!< Inactivity.
    FREE_FALL = 2,		//!< Free fall.
    WATERMARK = 1,		//!< Watermark.
    OVERRUN = 0			//!< Overrun interrupt enable/map/source.
  } __attribute__((packed));

  /**
   * Check for activity. Returns a bitset with current activity.
   * Ignore WATERMARK and OVERRUN.
   * @return activities
   */
  uint8_t is_activity();

protected:
  /**
   * Registers Map (See tab. 19, pp. 23).
   */
  enum Register {
    DEVID = 0x00,		//!< Device ID.
    THRESH_TAP = 0x1D,		//!< Tap threshold.
    OFS = 0x1E,			//!< Offset (x, y, z).
    DUR = 0x21,			//!< Tap duration.
    LATENT = 0x22,		//!< Tap latency.
    WINDOW = 0x23,		//!< Tap window.
    THRESH_ACT = 0x24,		//!< Activity threshold.
    THRESH_INACT = 0x25, 	//!< Inactivity threshold.
    TIME_INACT = 0x26,		//!< Inactivity time.
    ACT_INACT_CTL = 0x27,	//!< Axis enable control for activity detection.
    THRESH_FF = 0x28,		//!< Free-fall threshold.
    TIME_FF = 0x29,		//!< Free-fall time.
    TAP_AXES = 0x2A,		//!< Axis control for single tap/double tap.
    ACT_TAP_STATUS = 0x2B,	//!< Source of single tap/double tap.
    BW_RATE = 0x2C,		//!< Data rate and power mode control.
    POWER_CTL = 0x2D,		//!< Power-saving features control.
    INT_ENABLE = 0x2E,		//!< Interrupt enable control.
    INT_MAP = 0x2F,		//!< Interrupt mapping control.
    INT_SOURCE = 0x30,		//!< Source of interrupts.
    DATA_FORMAT = 0x31,		//!< Data format control.
    DATA = 0x32,		//!< Data (x, y, z).
    FIFO_CTL = 0x38,		//!< FIFO control.
    FIFO_STATUS = 0x39		//!< FIFO status.
  } __attribute__((packed));

  /** Register DEVID value (345). */
  static const uint8_t ID = 0xe5;

  /**
   * Register ACT_INACT_CTL bitfields.
   */
  enum {
    ACT_AC_DC = 7, 		//!< AC/DC coupled activity.
    ACT_X_EN = 6,		//!< Activity x axis enable.
    ACT_Y_EN = 5,		//!< Activity x axis enable.
    ACT_Z_EN = 4,		//!< Activity x axis enable.
    INACT_AC_DC = 3, 		//!< AC/DC coupled inactivity.
    INACT_X_EN = 2,		//!< Inactivity x axis enable.
    INACT_Y_EN = 1,		//!< Inactivity x axis enable.
    INACT_Z_EN = 0		//!< Inactivity x axis enable.
  } __attribute__((packed));

  /**
   * Register TAP_AXES bitfields.
   */
  enum {
    SUPPRES = 3,		//!< Suppress.
    TAP_X_EN = 2,	        //!< Tap x enable.
    TAP_Y_EN = 1,	        //!< Tap y enable.
    TAP_Z_EN = 0,	        //!< Tap z enable.
  } __attribute__((packed));

  /**
   * Register ACT_TAP_STATUS bitfields.
   */
  enum {
    ACT_X_SRC = 6, 		//!< Activity x event.
    ACT_Y_SRC = 5, 		//!< Activity y event.
    ACT_Z_SRC = 4, 		//!< Activity z event.
    ASLEEP = 3,			//!< Device is asleep.
    TAP_X_SRC = 2, 		//!< Tap x event.
    TAP_Y_SRC = 1, 		//!< Tap y event.
    TAP_Z_SRC = 0 		//!< Tap z event.
  } __attribute__((packed));

  /**
   * Register BW_RATE bitfields.
   */
  enum {
    LOW_POWER = 4,		//!< Low power move.
    RATE = 0,			//!< Data rate (4 bits).
    RATE_MASK = 0x0f		//!< Data rate mask.
  } __attribute__((packed));

  /**
   * Register POWER_CTL bitfields.
   */
  enum {
    LINK = 5,			//!< Link mode select.
    AUTO_SLEEP = 4,		//!< Auto sleep enable.
    MEASURE = 3,		//!< Measurement mode.
    SLEEP = 2,			//!< Sleep mode.
    WAKEUP = 0,			//!< Wakeup frequency (2 bits).
  } __attribute__((packed));
  enum {
    WAKEUP_8_HZ = 0,		//!< 8 hz.
    WAKEUP_4_HZ = 1,		//!< 4 hz.
    WAKEUP_2_HZ = 2,		//!< 2 hz.
    WAKEUP_1_HZ = 3,		//!< 1 hz.
    WAKEUP_MASK = 3		//!< Wakeup frequency mask.
  };

  /**
   * Register DATA_FORMAT bitfields.
   */
  enum {
    SELF_TEST = 7, 		//!< Self-test force.
    SPI_WIRE_MODE = 6,		//!< SPI wire mode.
    INT_INVERT = 5,		//!< Interrupt active high/low.
    FULL_RES = 3,		//!< Full resolution.
    JUSTIFY = 2,		//!< Left justified/sign extend.
    RANGE = 0			//!< Range 2-16 g (2 bits).
  } __attribute__((packed));
  enum {
    RANGE_2G = 0,		//!< +-2g.
    RANGE_4G = 1,		//!< +-4g.
    RANGE_8G = 2,		//!< +-8g.
    RANGE_16G = 3,		//!< +-16g.
    RANGE_MASK = 3		//!< Mask range field.
  } __attribute__((packed));

  /**
   * Register FIFO_CTL bitfields.
   */
  enum {
    FIFO_MODE = 6,		//!< FIFO Mode.
    FIFO_MASK = 0xc0,		//!< Dito Mask.
    TRIG = 5,			//!< Trigger event to interrupt pin.
    SAMPLES = 0,		//!< Number of samples (5 bits).
    SAMPLES_MASK = 0x1f		//!< Dito Mask.
  } __attribute__((packed));
  enum {
    BYPASS = 0x00,		//!< FIFO is bypassed.
    FIFO = 0x40,		//!< Collects up to 32 values.
    STREAM = 0x80,		//!< Holds the latest 32 values.
    TRIGGER = 0xc0		//!< Latest 32 values before trigger.
  };

  /**
   * Register FIFO_STATUS bitfields.
   */
  enum {
    FIFO_TRIG = 7,		//!< FIFO trigger event occuring.
    ENTRIES = 0,		//!< Number of entries in FIFO (6 bits).
    ENTRIES_MASK = 0x3f		//!< Dito Mask.
  } __attribute__((packed));

  /**
   * Write given value to register.
   * @param[in] reg register address.
   * @param[in] value register value.
   */
  void write(Register reg, uint8_t value);

  /**
   * Write multiple registers with values from give address.
   * @param[in] reg register address.
   * @param[in] buffer storage.
   * @param[in] count number of bytes.
   */
  void write(Register reg, void* buffer, uint8_t count);

  /**
   * Read contents of register.
   * @param[in] reg register address.
   * @return register value.
   */
  uint8_t read(Register reg)
  {
    uint8_t res;
    read(reg, &res, sizeof(res));
    return (res);
  }

  /**
   * Read contents of registers, multiple values from give address.
   * @param[in] reg register address.
   * @param[in] buffer storage.
   * @param[in] count number of bytes.
   */
  void read(Register reg, void* buffer, uint8_t count);

  /** 3-Axis setting (add or or values). */
  enum {
    X = 4,
    Y = 2,
    Z = 1
  } __attribute__((packed));
};

/**
 * Print the latest reading to the given output stream.
 * @param[in] outs output stream.
 * @param[in] accelerometer instance.
 * @return output stream.
 */
extern IOStream& operator<<(IOStream& outs, ADXL345& accelerometer);

#endif
