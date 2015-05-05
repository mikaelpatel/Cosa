/**
 * @file L3G4200D.hh
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

#ifndef COSA_L3G4200D_HH
#define COSA_L3G4200D_HH

#include "Cosa/TWI.hh"
#include "Cosa/IOStream.hh"

/**
 * Cosa TWI driver for STMicroelectronics, L3G4200D, MEMS motion
 * sensor: ultra-stable three-axis digital output gyroscope.
 * Doc ID 17116 Rev 3.
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
 * (Dn/EXTn)-----------8-|T-INT1      |
 *                     9-|P-XCLR      |
 *                    10-|P-EOC       |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. http://www.st.com/st-web-ui/static/active/en/resource/technical/
 * document/datasheet/CD00265057.pdf
 */
class L3G4200D : private TWI::Driver {
public:
  /**
   * Construct L3G4200D digital gyroscope driver with given
   * sub-address. Default is zero(0).
   * @param[in] subaddr sub-address (0..1, default 0)
   */
  L3G4200D(uint8_t subaddr = 0) : TWI::Driver(0x68 | (subaddr != 0)) {}

  /**
   * Start interaction with device. Turn on measurements.
   * @return true(1) if successful otherwise false(0)
   */
  bool begin();

  /**
   * Stop sequence of interaction with device. Turn off measurements
   * and power down.
   * @return true(1) if successful otherwise false(0)
   */
  bool end();

  /**
   * Gyroscope sample data structure (axis x, y, z).
   */
  struct sample_t {
    int x;
    int y;
    int z;
  };

  /**
   * Sample gyroscope and return values in given data structure
   * @param[in] s sample storage.
   */
  void sample(sample_t& s)
    __attribute__((always_inline))
  {
    read(OUT, &s, sizeof(s));
  }

protected:
  /**
   * Register address map (See tab. 18, pp. 27).
   */
  enum Register {
    WHO_A_I = 0x0F,		//!< Device identification register.
    CTRL_REG1 = 0x20,		//!< Control register#1.
    CTRL_REG2 = 0x21,		//!< Control register#2.
    CTRL_REG3 = 0x22,		//!< Control register#3.
    CTRL_REG4 = 0x23,		//!< Control register#4.
    CTRL_REG5 = 0x24,		//!< Control register#5.
    REFERENCE = 0x25,		//!< Reference value for interrupt generation.
    OUT_TEMP = 0x26,		//!< Temperature data.
    STATUS_REG = 0x27,		//!< Status register.
    OUT = 0x28,			//!< X,Y,Z-axis angular rate data.
    OUT_X = 0x28,		//!< X axis angular rate data.
    OUT_X_L = 0x28,		//!< X low byte.
    OUT_X_H = 0x29,		//!< X high byte.
    OUT_Y = 0x2A,		//!< Y axis angular rate data.
    OUT_Y_L = 0x2A,		//!< Y low byte.
    OUT_Y_H = 0x2B,		//!< Y high byte.
    OUT_Z = 0x2C,		//!< Y axis angular rate data.
    OUT_Z_L = 0x2C,		//!< Z low byte.
    OUT_Z_H = 0x2D,		//!< Z high byte.
    FIFO_CTRL_REG = 0x2E,	//!< FIFO control register.
    FIFO_SRC_REG = 0x2F,	//!< FIFO status register.
    INT1_CFG = 0x30,		//!< Interrupt configuration.
    INT1_SRC = 0x31,		//!< Interrupt source register.
    INT1_TSH = 0x32,		//!< Interrupt threshold.
    INT1_TSH_XH = 0x32,		//!< X high threshold.
    INT1_TSH_XL = 0x33,		//!< X low threshold.
    INT1_TSH_YH = 0x34,		//!< Y high threshold.
    INT1_TSH_YL = 0x35,		//!< Y low threshold.
    INT1_TSH_ZH = 0x36,		//!< Z high threshold.
    INT1_TSH_ZL = 0x37,		//!< Z low threshold.
    INT1_DURATION = 0x38	//!< Wait duration.
  } __attribute__((packed));

  /** Auto increment address. */
  static const uint8_t AUTO_INC = 0x80;

  /**
   * Register CTRL_REG1 bitfields.
   */
  union ctrl_reg1_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t Xen:1;		//!< X axis enable.
      uint8_t Yen:1;		//!< Y axis enable.
      uint8_t Zen:1;		//!< Z axis enable.
      uint8_t PD:1;		//!< Power down enable.
      uint8_t BW:2;		//!< Bandwidth selection.
      uint8_t DR:2;		//!< Output Data Rate selection.
    };
    ctrl_reg1_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };
  enum {			//!< Data rate (table 22).
    DR_100_HZ,			//!< 100 Hz.
    DR_200_HZ,			//!< 200 Hz.
    DR_400_HZ,			//!< 400 Hz.
    DR_800_HZ			//!< 800 Hz.
  } __attribute__((packed));
  enum {			//!< Bandwidth selection (table 22).
    BW_12_5,			//!< Note: Higher with 800 Hz.
    BW_25,
    BW_50,
    BW_110
  } __attribute__((packed));

  /**
   * Register CTRL_REG2 bitfields.
   */
  union ctrl_reg2_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t HPCF:4;		//!< High Pass filter Cut Off frequency.
      uint8_t HPM:2;		//!< High Pass filter Mode Selection.
      uint8_t reserved:2;	//!< Zero.
    };
    ctrl_reg2_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };
  enum {			//!< High pass filter mode (table 26).
    HPM_NORMAL = 0,		//!< Normal moden (reset reading).
    HPM_MODE = 1,		//!< Reference signal for filtering.
    HPM_AUTORESET = 3		//!< Autoreset on interrupt event.
  } __attribute__((packed));

  /**
   * Register CTRL_REG3 bitfields.
   */
  union ctrl_reg3_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t I2_Empty:1;    	//!< FIFO Empty interrupt on DRDY/INT2.
      uint8_t I2_ORun:1;	//!< FIFO Overrun interrupt on DRDY/INT2.
      uint8_t I2_WTM:1;	     	//!< FIFO Watermark interrupt on DRDY/INT2.
      uint8_t I2_DRDY:1;	//!< Data Ready on DRFY/INT2.
      uint8_t PP_OD:1;	     	//!< Push-Pull/Open drain.
      uint8_t H_Lactive:1;	//!< Interrupt active configuration on INT1.
      uint8_t I1_Boot:1;	//!< Boot status available on INT1.
      uint8_t I1_Int1:1;	//!< Interrupt enable.
    };
    ctrl_reg3_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register CTRL_REG4 bitfields.
   */
  union ctrl_reg4_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t SIM:1;		//!< SPI Serial Mode selection.
      uint8_t ST:2;		//!< Seft Test Enable.
      uint8_t reserved:1;
      uint8_t FS:2;		//!< Full Scale selection.
      uint8_t BLE:1;		//!< Big/Little Endian Data Selection.
      uint8_t BDU:1;		//!< Block Data Update.
    };
    ctrl_reg4_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register CTRL_REG5 bitfields.
   */
  union ctrl_reg5_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t Out_Sel:2;	//!< Out Selection configuration.
      uint8_t INT1_Sel:2;	//!< INT1 Selection configuration.
      uint8_t HPen:1;		//!< High Pass filter Enable.
      uint8_t FIFO_EN:1;	//!< FIFO enable.
      uint8_t BOOT:1;		//!< Reboot memory content.
    };
    ctrl_reg5_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register STATUS_REG bitfields.
   */
  union status_reg_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t XDA:1;		//!< X axis new data available.
      uint8_t YDA:1;		//!< Y axis new data available.
      uint8_t ZDA:1;		//!< Z axis new data available.
      uint8_t XYZDA:1;		//!< X,Y,Z-axis new data available.
      uint8_t XOR:1;		//!< X axis data overrun.
      uint8_t YOR:1;		//!< Y axis data overrun.
      uint8_t ZOR:1;		//!< Z axis data overrun.
      uint8_t XYZOR:1;		//!< X,Y,Z-axis data overrun.
    };
    status_reg_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register FIFO_CTRL_REG bitfields.
   */
  union fifo_ctrl_reg_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t WTM:5;		//!< FIFO threshold. Watermark level setting.
      uint8_t FM:3;		//!< FIFO mode selection.
    };
    fifo_ctrl_reg_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };
  enum {			//!< FIFO mode configuration (table 45).
    FM_BYPASS,			//!< Bypass mode.
    FM_FIFO,			//!< FIFO mode.
    FM_STREAM,			//!< Stream.
    FM_STREAM_TO_FIFO,		//!< Stream-to-FIFO.
    FM_BYPASS_TO_STREAM		//!< Bypass-to-Stream.
  } __attribute__((packed));

  /**
   * Register FIFO_SRC_REG bitfields.
   */
  union fifo_src_reg_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t FSS:5;		//!< FIFO stored data level.
      uint8_t EMPTY:1;		//!< FIFO empty bit.
      uint8_t OVRN:1;		//!< Overrun bit status.
      uint8_t WTM:1;		//!< Watermark status.
    };
    fifo_src_reg_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register INT1_CFG bitfields.
   */
  union int1_cfg_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t XLIE:1;		//!< Enable interrupt generation on X low.
      uint8_t XHIE:1;		//!< Enable interrupt generation on X high.
      uint8_t YLIE:1;		//!< Enable interrupt generation on Y low.
      uint8_t YHIE:1;		//!< Enable interrupt generation on Y high.
      uint8_t ZLIE:1;		//!< Enable interrupt generation on Z low.
      uint8_t ZHIE:1;		//!< Enable interrupt generation on Z high.
      uint8_t LIR:1;		//!< Latch Interrup Request.
      uint8_t AND_OR:1;		//!< AND/OR combination of interrupt events.
    };
    int1_cfg_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register INT1_SRC bitfields.
   */
  struct int1_src_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t XL:1;		//!< X low.
      uint8_t XH:1;		//!< X high.
      uint8_t YL:1;		//!< Y low.
      uint8_t YH:1;		//!< Y high.
      uint8_t ZL:1;		//!< Z low.
      uint8_t ZH:1;		//!< Z high.
      uint8_t IA:1;		//!< Interrupt active.
      uint8_t reserved:1;
    };
    int1_src_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Register INT1_DURATION bitfields.
   */
  struct int1_duration_t {
    uint8_t as_uint8;		//!< As a byte.
    struct {			//!< As bitfields.
      uint8_t value:7;		//!< Duration value.
      uint8_t WAIT:1;		//!< Wait enable.
    };
    int1_duration_t(uint8_t value = 0)
    {
      as_uint8 = value;
    }
    operator uint8_t()
    {
      return (as_uint8);
    }
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
};

extern IOStream& operator<<(IOStream& outs, L3G4200D& gyroscope);

#endif
