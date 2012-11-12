/**
 * @file Cosa/ADXL.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2012
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
 * Analog Devices ADXL345 Digital Accelerometer SPI driver.
 * http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
 *
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef __COSA_ADXL_H__
#define __COSA_ADXL_H__

#include "SPI.h"
#include "Pins.h"

class ADXL : private SPI {

private:
  /**
   * SPI commands (See fig. 39, pp. 16)
   */
  enum {
    WRITE_CMD = 0x00,		// Write command (6 bit addr)
    READ_CMD = 0x80,		// Read command
    MULTIPLE_BYTE = 0x40,	// Multiple byte
    REG_MASK = 0x3f
  };

protected:
  /**
   * Slave select pin (default is pin 10)
   */
  OutputPin _ss;

  /**
   * Registers Map (See tab. 19, pp. 23)
   */
  enum Register {
    DEVID = 0x00,		// Device ID
    THRESH_TAP = 0x1D,		// Tap threshold
    OFS = 0x1E,			// Offset (x, y, z)
    DUR = 0x21,			// Tap duration
    LATENT = 0x22,		// Tap latency
    WINDOW = 0x23,		// Tap window
    THRESH_ACT = 0x24,		// Activity threshold
    THRESH_INACT = 0x25, 	// Inactivity threshold
    TIME_INACT = 0x26,		// Inactivity time
    ACT_INACT_CTL = 0x27,	// Axis enable control for activity detection
    THRESH_FF = 0x28,		// Free-fall threshold
    TIME_FF = 0x29,		// Free-fall time
    TAP_AXES = 0x2A,		// Axis control for single tap/double tap
    ACT_TAP_STATUS = 0x2B,	// Source of single tap/double tap
    BW_RATE = 0x2C,		// Data rate and power mode control
    POWER_CTL = 0x2D,		// Power-saving features control
    INT_ENABLE = 0x2E,		// Interrupt enable control
    INT_MAP = 0x2F,		// Interrupt mapping control
    INT_SOURCE = 0x30,		// Source of interrupts
    DATA_FORMAT = 0x31,		// Data format control
    DATA = 0x32,		// Data (x, y, z)
    FIFO_CTL = 0x38,		// FIFO control
    FIFO_STATUS = 0x39		// FIFO status
  };

  /**
   * Register ACT_INACT_CTL bitfields
   */
  enum {
    ACT_AC_DC = 7, 		// AC/DC coupled activity 
    ACT_X_EN = 6,		// Activity x axis enable
    ACT_Y_EN = 5,		// Activity x axis enable
    ACT_Z_EN = 4,		// Activity x axis enable
    INACT_AC_DC = 3, 		// AC/DC coupled inactivity
    INACT_X_EN = 2,		// Inactivity x axis enable
    INACT_Y_EN = 1,		// Inactivity x axis enable
    INACT_Z_EN = 0		// Inactivity x axis enable
  };

  /**
   * Register TAP_AXES bitfields
   */
  enum {
    SUPPRES = 3,		// Suppress
    TAP_X_EN = 2,	        // Tap x enable
    TAP_Y_EN = 1,	        // Tap y enable
    TAP_Z_EN = 0,	        // Tap z enable
  };

  /**
   * Register ACT_TAP_STATUS bitfields
   */
  enum {
    ACT_X_SRC = 6, 		// Activity x event
    ACT_Y_SRC = 5, 		// Activity x event
    ACT_Z_SRC = 4, 		// Activity x event
    ASLEEP = 3,			// Device is asleep
    TAP_X_SRC = 2, 		// Activity x event
    TAP_Y_SRC = 1, 		// Activity x event
    TAP_Z_SRC = 0 		// Activity x event
  };

  /**
   * Register BW_RATE bitfields
   */
  enum {
    LOW_POWER = 4,		// Low power move
    RATE = 0			// Data rate (4 bits)
  };

  /**
   * Register POWER_CTL bitfields
   */
  enum {
    LINK = 5,			// Link mode select 
    AUTO_SLEEP = 4,		// Auto sleep enable
    MEASURE = 3,		// Measurement mode
    SLEEP = 2,			// Sleep mode
    WAKEUP = 0			// Wakeup frequency (2 bits)
  };

  /**
   * Register INT_ENABLE/INT_MAP/INT_SOURCE bitfields
   */
  enum {
    DATA_READY = 7,		// Data ready interrupt enable/map/source
    SINGLE_TAP = 6, 		// Single tap
    DOUBLE_TAP = 5,		// Double tap
    ACT = 4,			// Activity 
    INACT = 3, 			// Inactivity
    FREE_FALL = 2,		// Free fall
    WATERMARK = 1,		// Watermark
    OVERRUN = 0			// Overrun interrupt enable/map/source
  };

  /**
   * Register DATA_FORMAT bitfields
   */
  enum {
    SELF_TEST = 7, 		// Self-test force
    SPI_WIRE_MODE = 6,		// SPI wire mode
    INT_INVERT = 5,		// Interrupt active high/low
    FULL_RES = 3,		// Full resolution
    JUSTIFY = 2,		// Left justified/sign extend
    RANGE = 0			// Range 2-16 g (2 bits)
  };

  enum {
    RANGE_2G = 0,		// +-2g
    RANGE_4G = 1,		// +-4g
    RANGE_8G = 2,		// +-8g
    RANGE_16G = 3		// +-16g
  };

  /**
   * Register FIFO_CTL bitfields
   */
  enum {
    FIFO_MODE = 6,		// FIFO Mode
    TRIGGER = 5,		// Trigger event to interrupt pin
    SAMPLES = 0			// Number of samples (5 bits)
  };

  /**
   * Register FIFO_STATUS bitfields
   */
  enum {
    FIFO_TRIG = 7,		// FIFO trigger event occuring
    ENTRIES = 0			// Number of entries in FIFO (6 bits)
  };

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
  uint8_t read(Register reg);

  /**
   * Read contents of registers, multiple values from give address.
   * @param[in] reg register address.
   * @param[in] buffer storage.
   * @param[in] count number of bytes.
   */
  void read(Register reg, void* buffer, uint8_t count);

public:
  /**
   * Construct accelerometer object selected with given output pin.
   * @param[in] ss slave selection pin.
   */
  ADXL(uint8_t ss = 10);

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
   * @param[in] x axis offset
   * @param[in] y axis offset
   * @param[in] z axis offset
   */
  void calibrate(int8_t x, int8_t y, int8_t z);

  /**
   * Calibrate accelerometer by resetting offset and
   * using the current accelerometer values as offset.
   * (-sample/4) according to ADXL345 documentation.
   */
  void calibrate();

  /**
   * Accelerometer sample data structure (axis x, y, z)
   */
  struct sample_t {
    int x;
    int y;
    int z;
    
    /**
     * In debug mode, print sample to serial stream.
     */
    void print();

    /**
     * In debug mode, print sample to serial stream with new-line.
     */
    void println();
  };

  /**
   * Sample accelerometer. Return sample is given data structure
   * @param[in] s sample storage.
   */
  void sample(sample_t& s);
};

#endif
