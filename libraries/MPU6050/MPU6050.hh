/**
 * @file MPU6050.hh
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

#ifndef COSA_MPU6050_HH
#define COSA_MPU6050_HH

#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa TWI driver for InvenSense, MPU6050, Motion Processing Unit,
 * 6-axis motion processor solution with gyroscope, accelerator and
 * extended I2C slave bus.
 *
 * @section Circuit
 * The MPU6050 module ITG/MPU with pull-up resistors (4K7) for TWI
 * signals and 3V3 internal voltage converter.
 * @code
 *                           ITG/MPU
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (A5/SCL)------------3-|SCL         |
 * (A4/SDA)------------4-|SDA         |
 *                     6-|XDA         |
 *                     7-|XCL         |
 *                     8-|AD0         |
 *                     9-|INT         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. MPU-6000 and MPU-6050 Register Map and Description, Rev. 4.2.
 * http://invensense.com/mems/gyro/documents/RM-MPU-6000A-00v4.2.pdf
 */
class MPU6050 : private TWI::Driver {
public:
  /**
   * Construct MPU6050 digital gyroscope driver with given
   * sub-address. Default is zero(0).
   * @param[in] subaddr sub-address (0..1, default 0)
   */
  MPU6050(uint8_t subaddr = 0) : TWI::Driver(0x68 | (subaddr != 0)) {}

  /**
   * Start interaction with device. Turn on measurements.
   * @param[in] clksel clock source (default PLL with X axis gyroscope)
   * @return true(1) if successful otherwise false(0)
   */
  bool begin(uint8_t clksel = CLKSEL_PLL_GYRO_X_REF);

  /**
   * Stop sequence of interaction with device. Turn off measurements
   * and power down.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Read temperature and return value in scale 0.1 Celcius.
   * @return temperature
   */
  int16_t read_temperature();

  /**
   * Accelerometer and gyroscope sample data structure (axis x, y, z).
   */
  struct sample_t {
    int16_t x;
    int16_t y;
    int16_t z;
  };

  /**
   * Motion Processing Unit sensor data; accelerometer, temperature,
   * and gyroscope.
   */
  struct motion_t {
    sample_t accel;
    int16_t temp;
    sample_t gyro;
  };

  /**
   * Read accelerometer and return values in given sample data
   * structure.
   * @param[in,out] m samples storage.
   */
  void read_motion(motion_t& m);

  /**
   * Read accelerometer and return values in given sample data
   * structure.
   * @param[in,out] s sample storage.
   */
  void read_accelerometer(sample_t& s);

  /**
   * Read gyroscope and return values in given data structure
   * @param[in,out] s sample storage.
   */
  void read_gyroscope(sample_t& s);

protected:
  /**
   * Register address map (See chap. 3 Register Map, pp. 6-7).
   */
  enum Register {
    SELF_TEST_X = 0x0d,		//!< Self Test Registers.
    SELF_TEST_Y = 0x0e,		//!< Self Test Registers.
    SELF_TEST_Z = 0x0f,		//!< Self Test Registers.
    SELF_TEST_A = 0x10,		//!< Self Test Registers.
    SMPRT_DIV = 0x19,		//!< Sample Rate Divider.
    CONFIG = 0x1a,		//!< Configuration.
    GYRO_CONFIG = 0x1b,		//!< Gyroscope Configuration.
    ACCEL_CONFIG = 0x1c,	//!< Accelerometer Configuration.
    FIFO_EN = 0x23,		//!< FIFO Enable.
    I2C_MST_CTRL = 0x24,	//!< I2C Master Control.
    I2C_SLV0_ADDR = 0x25,	//!< I2C Slave 0 Address.
    I2C_SLV0_REG = 0x26,	//!< I2C Slave 0 Register.
    I2C_SLV0_CTRL = 0x27,	//!< I2C Slave 0 Control.
    I2C_SLV1_ADDR = 0x28,	//!< I2C Slave 1 Address.
    I2C_SLV1_REG = 0x29,	//!< I2C Slave 1 Register.
    I2C_SLV1_CTRL = 0x2a,	//!< I2C Slave 1 Control.
    I2C_SLV2_ADDR = 0x2b,	//!< I2C Slave 2 Address.
    I2C_SLV2_REG = 0x2c,	//!< I2C Slave 2 Register.
    I2C_SLV2_CTRL = 0x2d,	//!< I2C Slave 2 Control.
    I2C_SLV3_ADDR = 0x2e,	//!< I2C Slave 3 Address.
    I2C_SLV3_REG = 0x2f,	//!< I2C Slave 3 Register.
    I2C_SLV3_CTRL = 0x30,	//!< I2C Slave 3 Control.
    I2C_SLV4_ADDR = 0x31,	//!< I2C Slave 4 Address.
    I2C_SLV4_REG = 0x32,	//!< I2C Slave 4 Register.
    I2C_SLV4_DO = 0x33,		//!< I2C Slave 4 Data Out.
    I2C_SLV4_CTRL = 0x34,	//!< I2C Slave 4 Control.
    I2C_SLV4_DI = 0x35,		//!< I2C Slave 4 Data In.
    I2C_MST_STATUS = 0x36,	//!< I2C Master Status.
    INT_PIN_CFG = 0x37,		//!< INT Pin/Bypass Enable Configuration.
    INT_ENABLE = 0x38,		//!< Interrupt Enable.
    INT_STATUS = 0x3a,		//!< Interrupt Status.
    ACCEL_OUT = 0x3b,		//!< Accelerometer Measurements.
    ACCEL_XOUT = 0x3b,		//!< X.
    ACCEL_XOUT_H = 0x3b,	//!< X MSB.
    ACCEL_XOUT_L = 0x3c,	//!< X LSB.
    ACCEL_YOUT = 0x3d,		//!< Y.
    ACCEL_YOUT_H = 0x3d,	//!< Y MSB.
    ACCEL_YOUT_L = 0x3e,	//!< Y LSB.
    ACCEL_ZOUT = 0x3f,		//!< Z.
    ACCEL_ZOUT_H = 0x3f,	//!< Z MSB.
    ACCEL_ZOUT_L = 0x40,	//!< Z LSB.
    TEMP_OUT = 0x41,		//!< Temperature Measurement.
    TEMP_OUT_H = 0x41,		//!< Temp MSB.
    TEMP_OUT_L = 0x42,		//!< Temp LSB.
    GYRO_OUT = 0x43,		//!< Gyroscope Measurement.
    GYRO_XOUT = 0x43,		//!< X.
    GYRO_XOUT_H = 0x43,		//!< X MSB.
    GYRO_XOUT_L = 0x44,		//!< X LSB.
    GYRO_YOUT = 0x45,		//!< Y.
    GYRO_YOUT_H = 0x45,		//!< Y MSB.
    GYRO_YOUT_L = 0x46,		//!< Y LSB.
    GYRO_ZOUT = 0x47,		//!< Z.
    GYRO_ZOUT_H = 0x47,		//!< Z MSB.
    GYRO_ZOUT_L = 0x48,		//!< Z LSB.
    EXT_SENS_DATA = 0x49,	//!< External Sensor Data.
    I2C_SLV0_DO = 0x63,		//!< I2C Slave 0 Data Out.
    I2C_SLV1_DO = 0x64,		//!< I2C Slave 1 Data Out.
    I2C_SLV2_DO = 0x65,		//!< I2C Slave 2 Data Out.
    I2C_SLV3_DO = 0x66,		//!< I2C Slave 3 Data Out.
    I2C_MST_DELAY_CTRL = 0x67, 	//!< I2C Master Delay Control.
    SIGNAL_PATH_RESET = 0x68,	//!< Signal Path Reset.
    USER_CTRL = 0x6a,		//!< User Control.
    PWR_MGMT_1 = 0x6b,		//!< Power Management 1.
    PWR_MGMT_2 = 0x6c,		//!< Power Management 2.
    FIFO_COUNT = 0x72,		//!< FIFO Count Registers.
    FIFO_COUNT_H = 0x72,	//!< FIFO Count Registers MSB.
    FIFO_COUNT_L = 0x73,	//!< FIFO Count Registers LSB.
    FIFO_R_W = 0x74,		//!< FIFO Read Write.
    WHO_AM_I = 0x75,		//!< Who Am I.
  } __attribute__((packed));

  /**
   * Register CONFIG bitfields.
   */
  union config_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t DLPF_CFG:3;	//!< Digital Low Pass Filter.
      uint8_t EXT_SYNC_SET:3;	//!< Frame Synchronization setting.
      uint8_t reserved:2;	//!< Reserved.
    };
    config_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register GYRO_CONFIG bitfields.
   */
  union gyro_config_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t reserved:3;	//!< Reserved.
      uint8_t FS_SEL:2;		//!< Full Scale Range.
      uint8_t ZG_ST:1;		//!< Z axis gyroscope selftest.
      uint8_t YG_ST:1;		//!< Y axis gyroscope selftest.
      uint8_t XG_ST:1;		//!< X axis gyroscope selftest.
    };
    gyro_config_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Full scale range of the gyroscope (pp. 14).
   */
  enum {
    FS_RANGE_250,
    FS_RANGE_500,
    FS_RANGE_1000,
    FS_RANGE_2000
  };

  /**
   * Register ACCEL_CONFIG bitfields.
   */
  union accel_config_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t reserved:3;	//!< Reserved.
      uint8_t AFS_SEL:2;	//!< Full Scale Range.
      uint8_t ZA_ST:1;		//!< Z axis accelerometer selftest.
      uint8_t YA_ST:1;		//!< Y axis accelerometer selftest.
      uint8_t XA_ST:1;		//!< X axis accelerometer selftest.
    };
    accel_config_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Full scale range of the accelerometer (pp. 15).
   */
  enum {
    AFS_RANGE_2G,
    AFS_RANGE_4G,
    AFS_RANGE_8G,
    AFS_RANGE_16G
  };

  /**
   * Register FIFO_EN bitfields.
   */
  union fifo_en_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t SLV0_FIFO_EN:1;	//!< Enable Slave 0 data.
      uint8_t SLV1_FIFO_EN:1;	//!< Enable Slave 1 data.
      uint8_t SLV2_FIFO_EN:1;	//!< Enable Slave 2 data.
      uint8_t ACCEL_FIFO_EN:1;	//!< Enable Accelerometer.
      uint8_t ZG_FIFO_EN:1;	//!< Enable Gyroscope Z.
      uint8_t YG_FIFO_EN:1;	//!< Enable Gyroscope Y.
      uint8_t XG_FIFO_EN:1;	//!< Enable Gyroscope X.
      uint8_t TEMP_FIFO_EN:1;	//!< Enable Temperature.
    };
    fifo_en_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register INT_PIN_CFG bitfields.
   */
  union int_pin_cfg_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t reserved:1;	//!< Reserved.
      uint8_t I2C_BYPASS_EN:1;	//!< Access aux I2C bus directly.
      uint8_t FSYNC_INT_EN:1;	//!< Enable FSYNC pint as interrupt.
      uint8_t FSYNC_INT_LEVEL:1;//!< Active high(0) or low(1).
      uint8_t INT_RD_CLEAR:1;	//!< Clear interrupt status on read.
      uint8_t LATCH_INT_EN:1;	//!< High until cleared.
      uint8_t INT_OPEN:1;	//!< Open drain.
      uint8_t INT_LEVEL:1;	//!< Active high(0) or low(1).
    };
    int_pin_cfg_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register INT_ENABLE bitfields.
   */
  union int_enable_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t DATA_RDY_EN:1;	//!< Enable Data Ready interrupt.
      uint8_t reserved1:2;	//!< Reserved-1.
      uint8_t I2C_MST_INT_EN:1;	//!< Enable I2C Master interrupt.
      uint8_t FIFO_OFLOW_EN:1;	//!< Enable FIFO buffer overflow.
      uint8_t reserved2:3;	//!< Reserved-2.
    };
    int_enable_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register INT_STATUS bitfields.
   */
  union int_status_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t DATA_RDY_INT:1;	//!< Data Ready interrupt.
      uint8_t reserved1:2;	//!< Reserved-1.
      uint8_t I2C_MST_INT:1;	//!< I2C Master interrupt.
      uint8_t FIFO_OFLOW_INT:1;	//!< FIFO buffer overflow.
      uint8_t reserved2:3;	//!< Reserved-2.
    };
    int_status_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register SIGNAL_PATH_RESET bitfields.
   */
  union signal_path_reset_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t TEMP_RESET:1;	//!< Reset temperature sensor.
      uint8_t ACCEL_RESET:1;	//!< Reset accelerometer.
      uint8_t GYRO_RESET:1;	//!< Reset gyroscope.
      uint8_t reserved:5;	//!< Reserved.
    };
    signal_path_reset_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register USER_CTRL bitfields.
   */
  union user_ctrl_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t SIG_COND_RESET:1;	//!< Reset all signal paths.
      uint8_t I2C_MST_RESET:1;	//!< Reset I2C Master.
      uint8_t FIFO_RESET:1;	//!< Reset FIFO buffer.
      uint8_t reserved1:1;
      uint8_t I2C_IF_DIS:1;	//!< Zero.
      uint8_t I2C_MST_EN:1;	//!< Enable I2C Master.
      uint8_t FIFO_EN:1;	//!< Enable FIFO.
      uint8_t reserved2:1;
    };
    user_ctrl_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register PWR_MGMT_1 bitfields.
   */
  union pwr_mgmt_1_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t CLKSEL:3;		//!< Clock select.
      uint8_t TEMP_DIS:1;	//!< Disable temperature sensor.
      uint8_t reserved:1;
      uint8_t CYCLE:1; 		//!< Sleep and wakeup.
      uint8_t SLP:1;		//!< Sleep.
      uint8_t DEVICE_RESET:1;	//!< Reset.
    };
    pwr_mgmt_1_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Clock Source (pp. 40).
   */
  enum {
    CLKSEL_INTERNAL_8MHZ,
    CLKSEL_PLL_GYRO_X_REF,
    CLKSEL_PLL_GYRO_Y_REF,
    CLKSEL_PLL_GYRO_Z_REF,
    CLKSEL_PLL_EXT_32KHZ_REF,
    CLKSEL_PLL_EXT_19MHZ_REF,
    CLKSEL_RESERVED,
    CLKSEL_STOP_CLOCKS
  };

  /**
   * Register PWR_MGMT_2 bitfields.
   */
  union pwr_mgmt_2_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t STBY_ZG:1;	//!< Standby mode.
      uint8_t STBY_YG:1;	//!< Standby mode.
      uint8_t STBY_XG:1;	//!< Standby mode.
      uint8_t STBY_ZA:1;	//!< Standby mode.
      uint8_t STBY_YA:1;	//!< Standby mode.
      uint8_t STBY_XA:1;	//!< Standby mode.
      uint8_t LP_WAKE_CTRL:2;	//!< Low Power wake-up frequency.
    };
    pwr_mgmt_2_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Wake-up Frequency (pp. 42).
   */
  enum {
    LP_WAKE_CTRL_1_25HZ,
    LP_WAKE_CTRL_5HZ,
    LP_WAKE_CTRL_20HZ,
    LP_WAKE_CTRL_40HZ,
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
  void write(Register reg, void* buffer, size_t count);

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
  void read(Register reg, void* buffer, size_t count);
};

/**
 * Read device and print sensor data to the given output stream.
 * @param[in] outs output stream.
 * @param[in] mpu instance.
 * @return output stream.
 */
extern IOStream& operator<<(IOStream& outs, MPU6050& mpu);

#endif
