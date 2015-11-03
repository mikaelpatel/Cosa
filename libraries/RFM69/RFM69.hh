/**
 * @file RFM69.hh
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

#ifndef COSA_RFM69_HH
#define COSA_RFM69_HH

#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Wireless.hh"
#if !defined(BOARD_ATTINYX5)

/**
 * Cosa Device Driver for RFM69W/HW, Low-Power Sub-1 GHz RF Transceiver.
 * Note that this device requires data in big endian order.
 *
 * @section Circuit
 * This is the pin-out for the RFM69W/HW module.
 * @code
 *                         RFM69W/HW
 *                       +------------+
 * (RST)---------------1-|RESET     NC|-16
 * (D2/EXT0)-----------2-|DIO0     NSS|-15---------------(D10)
 *                     3-|DIO1    MOSI|-14---------------(D11/MOSI)
 *                     4-|DIO2    MISO|-13---------------(D12/MISO)
 *                     5-|DIO3     SCK|-12---------------(D13/SCK)     V
 *                     6-|DIO4     GND|-11---------------(GND)         |
 *                     7-|DIO4     ANT|-10-----------------------------+
 * (3V3)---------------8-|VCC      GND|-9----------------(GND)
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Product datasheet, RFM69W ISM Transceiver Module V1.3,
 * http://www.hoperf.com/rf/fsk_module/RFM69W.htm
 * 2. Product datasheet, RFM69HW ISM Transceiver Module V1.3,
 * http://www.hoperf.com/rf/fsk_module/RFM69HW.htm
 */
class RFM69 : private SPI::Driver, public Wireless::Driver {
public:
  /**
   * Maximum size of frame header is dest(1), src(1), and port(1).
   */
  static const size_t HEADER_MAX = 3;

  /**
   * Maximum size of payload. The device allows 66 bytes payload.
   * Adjust for frame header.
   */
  static const size_t PAYLOAD_MAX = 66 - HEADER_MAX;

  /**
   * Construct RFM69 device driver with given network and device
   * address. Connected to SPI bus and given chip select pin. Default
   * pins are Arduino Nano IO Shield for RFM69 module are D10 chip
   * select (RFM69:NSS) and D2/EXT0 external interrupt pin (RFM69:DIO0).
   * @param[in] net network address.
   * @param[in] dev device address.
   * @param[in] csn chip select pin (Default D2/D10/D53).
   * @param[in] irq interrupt pin (Default EXT0).
   */
#if defined(BOARD_ATTINYX4)
  RFM69(uint16_t net, uint8_t dev,
	Board::DigitalPin csn = Board::D2,
	Board::ExternalInterruptPin irq = Board::EXT0);
#elif defined(BOARD_ATMEGA2560)
  RFM69(uint16_t net, uint8_t dev,
	Board::DigitalPin csn = Board::D53,
	Board::ExternalInterruptPin irq = Board::EXT4);
#else
  RFM69(uint16_t net, uint8_t dev,
	Board::DigitalPin csn = Board::D10,
	Board::ExternalInterruptPin irq = Board::EXT0);
#endif

  /**
   * @override{Wireless::Driver}
   * Start and configure RFM69 device driver. The configuration must
   * set DIO0 to assert on received message. This device pin is
   * assumed to be connected the device driver interrupt pin (EXTn).
   * Return true(1) if successful othewise false(0).
   * @param[in] config configuration vector (default NULL)
   */
  virtual bool begin(const void* config = NULL);

  /**
   * @override{Wireless::Driver}
   * Shut down the device driver. Return true(1) if successful
   * otherwise false(0).
   * @return bool
   */
  virtual bool end();

  /**
   * @override{Wireless::Driver}
   * Send message in given null terminated io vector. Returns number
   * of bytes sent. Returns error code(-1) if number of bytes is
   * greater than PAYLOAD_MAX. Return error code(-2) if fails to set
   * transmit mode and/or packet is available to receive.
   * @param[in] dest destination network address.
   * @param[in] port device port (or message type).
   * @param[in] vec null termianted io vector.
   * @return number of bytes send or negative error code.
   */
  virtual int send(uint8_t dest, uint8_t port, const iovec_t* vec);

  /**
   * @override{Wireless::Driver}
   * Send message in given buffer, with given number of bytes. Returns
   * number of bytes sent. Returns error code(-1) if number of bytes
   * is greater than PAYLOAD_MAX. Return error code(-2) if fails to
   * set transmit mode.
   * @param[in] dest destination network address.
   * @param[in] port device port (or message type).
   * @param[in] buf buffer to transmit.
   * @param[in] len number of bytes in buffer.
   * @return number of bytes send or negative error code.
   */
  virtual int send(uint8_t dest, uint8_t port, const void* buf, size_t len);

  /**
   * @override{Wireless::Driver}
   * Receive message and store into given buffer with given maximum
   * length. The source network address is returned in the parameter src.
   * Returns error code(-2) if no message is available and/or a
   * timeout occured. Returns error code(-1) if the buffer size if to
   * small for incoming message or if the receiver fifo has overflowed.
   * Otherwise the actual number of received bytes is returned
   * @param[out] src source network address.
   * @param[out] port device port (or message type).
   * @param[in] buf buffer to store incoming message.
   * @param[in] len maximum number of bytes to receive.
   * @param[in] ms maximum time out period.
   * @return number of bytes received or negative error code.
   */
  virtual int recv(uint8_t& src, uint8_t& port, void* buf, size_t len,
		   uint32_t ms = 0L);

  /**
   * @override{Wireless::Driver}
   * Set device in power down mode.
   */
  virtual void powerdown();

  /**
   * @override{Wireless::Driver}
   * Set device in wakeup on radio mode.
   */
  virtual void wakeup_on_radio();

  /**
   * @override{Wireless::Driver}
   * Set output power level [-18..13] dBm.
   * @param[in] dBm.
   */
  virtual void output_power_level(int8_t dBm);

  /**
   * @override{Wireless::Driver}
   * Return estimated input power level (dBm) from latest successful
   * message received.
   */
  virtual int input_power_level();

  /**
   * Sample internal digital thermometer and return in centigrade
   * Celsius.
   * @return temperature.
   */
  int temperature();

  /**
   * Recalibrate internal RC oscillator when device is used in an
   * environment with high temperature variation.
   */
  void recalibrate();

private:
  /**
   * Configuration and Status Registers (Table 23, pp. 60).
   */
  enum Reg {
    FIFO = 0x00,		//!< FIFO read/write access (66 byte).
    OP_MODE = 0x01,		//!< Operating modes of the transceiver.
    DATA_MODUL = 0x02,		//!< Data operation mode and modulation.
    BITRATE = 0x03,		//!< Bit Rate setting (16-bit).
    BITRATE_MSB = 0x03,		//!< Bit Rate setting (MSB).
    BITRATE_LSB = 0x04,		//!< Bit Rate setting (LSB).
    FDEV = 0x05,	        //!< Frequency Deviation setting (16-bit).
    FDEV_MSB = 0x05,	        //!< Frequency Deviation setting (MSB).
    FDEV_LSB = 0x06,	        //!< Frequency Deviation setting (LSB).
    FRF = 0x07,			//!< RF Carrier Frequency (24-bit).
    FRF_MSB = 0x07,		//!< RF Carrier Frequency (MSB).
    FRF_MID = 0x08,		//!< RF Carrier Frequency (MID).
    FRF_LSB = 0x09,		//!< RF Carrier Frequency (LSB).
    OSC1 = 0x0A,	        //!< RC Oscillators Settings.
    AFC_CTRL = 0x0B,		//!< AFC control in low modulation.
    LISTEN1 = 0x0D,		//!< Listen Mode settings.
    LISTEN2 = 0x0E,		//!< Listen Mode Idle duration.
    LISTEN3 = 0x0F,		//!< Listen Mode Rx duration.
    VERSION = 0x10,		//!< Version.
    PA_LEVEL = 0x11,	        //!< PA selection and Output Power control.
    PA_RAMP = 0x12,	        //!< Control of the PA ramp time in FSK mode.
    OCP = 0x13,			//!< Over Current Protection control.
    LNA = 0x18,			//!< LNA settings.
    RX_BW = 0x19,		//!< Channel Filter BW Control.
    AFC_BW = 0x1A,		//!< Channel Filter BW Control durint AFC.
    OOK_PEAK = 0x1B,            //!< OOK demodulator control in peak mode.
    OOK_AVG = 0x1C,             //!< OOK demodulator average threshold control.
    OOK_FIXED = 0x1D,  		//!< OOK demodulator fixed threshold control.
    AFC_FEI = 0x1E,		//!< AFC and FEI control and status.
    AFC = 0x1F,			//!< Frequency correction of the AFC (16-bit).
    AFC_MSB = 0x1F,	   	//!< Frequency correction of the AFC (MSB).
    AFC_LSB = 0x20,	   	//!< Frequency correction of the AFC (LSB).
    FEI = 0x21,			//!< Calculated frequency error (16-bit).
    FEI_MSB = 0x21,		//!< Calculated frequency error (MSB).
    FEI_LSB = 0x22,		//!< Calculated frequency error (LSB).
    RSSI_CONFIG = 0x23,		//!< RSSI-related settings.
    RSSI_VALUE = 0x24,		//!< RSSI value in dBm.
    DIO_MAPPING1 = 0x25,	//!< Mapping of pins DIO0 to DIO3.
    DIO_MAPPING2 = 0x26, 	//!< Mapping of pins DIO4 to DIO5, CLKOUT frequency.
    IRQ_FLAGS1 = 0x27, 		//!< PLL Lock state, Timeout, RSSI Threshold...
    IRQ_FLAGS2 = 0x28,	    	//!< FIFO handling flags...
    RSSI_THRESH = 0x29,		//!< RSSI Threshold control.
    RX_TIMEOUT1 = 0x2A,		//!< Time from Rx request to RSSI detection.
    RX_TIMEOUT2 = 0x2B,		//!< Time from RSSI detection and PayloadReady.
    PREAMBLE = 0x2C,		//!< Preamble length (16-bit).
    PREAMBLE_MSB = 0x2C,	//!< Preamble length (MSB).
    PREAMBLE_LSB = 0x2D,	//!< Preamble length (LSB).
    SYNC_CONFIG = 0x2E,		//!< Sync Word Recognition control.
    SYNC_VALUE = 0x2F,		//!< Sync Word bytes (up to 8 bytes).
    SYNC_VALUE1 = 0x2F,		//!< Byte 1 of Sync Word.
    SYNC_VALUE2 = 0x30,		//!< Byte 2 of Sync Word.
    SYNC_VALUE3 = 0x31,		//!< Byte 3 of Sync Word.
    SYNC_VALUE4 = 0x32,		//!< Byte 4 of Sync Word.
    SYNC_VALUE5 = 0x33,		//!< Byte 5 of Sync Word.
    SYNC_VALUE6 = 0x34,		//!< Byte 6 of Sync Word.
    SYNC_VALUE7 = 0x35,		//!< Byte 7 of Sync Word.
    SYNC_VALUE8 = 0x36,		//!< Byte 8 of Sync Word.
    PACKET_CONFIG1 = 0x37,	//!< Packet mode settings.
    PAYLOAD_LENGTH = 0x38,	//!< Payload length setting.
    NODE_ADDR = 0x39,		//!< Node address.
    BROADCAST_ADDR = 0x3A,	//!< Broadcast address.
    AUTO_MODES = 0x3B,		//!< Auto modes settings.
    FIFO_THRESHOLD = 0x3C,	//!< Fifo threshold, Tx start condition.
    PACKET_CONFIG2 = 0x3D,	//!< Package mode settings.
    CYPHER_KEY = 0x3E,		//!< Cypher key (16 bytes).
    TEMP1 = 0x4E,		//!< Temperature Sensor control.
    TEMP2 = 0x4F,		//!< Temperature readout.
    TEST_LNA = 0x58,		//!< Sensitivity boost.
    TEST_PA1 = 0x5A,		//!< High Power PA settings.
    TEST_PA2 = 0x5C,		//!< High Power PA settings.
    TEST_DAGC = 0x6F,		//!< Fading Margin Improvement.
    TEST_AFC = 0x71,		//!< AFC offset for low modulation index AFC.
  } __attribute__((packed));

  /**
   * Transaction header (figure 25, pp. 44). Register address and
   * read/write flag in most significant bit.
   */
  enum {
    REG_READ = 0x00,		//!< Read register.
    REG_WRITE = 0x80,		//!< Write register.
    REG_MASK = 0x7F		//!< Mask register.
  } __attribute__((packed));

  /**
   * Read single register value.
   * @param[in] reg register address.
   * @return value
   */
  uint8_t read(Reg reg)
  {
    spi.acquire(this);
      spi.begin();
        spi.transfer(REG_READ | reg);
	uint8_t res = spi.transfer(0);
      spi.end();
    spi.release();
    return (res);
  }

  /**
   * Read multiple register or fifo values into given buffer.
   * @param[in] reg start register or fifo address.
   * @param[in] buf buffer to store register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void read(Reg reg, void* buf, size_t count)
  {
    spi.acquire(this);
      spi.begin();
        spi.transfer(REG_READ | reg);
	spi.read(buf, count);
      spi.end();
    spi.release();
  }

  /**
   * Write single register value.
   * @param[in] reg register address.
   * @param[in] value to write to register.
   */
  void write(Reg reg, uint8_t value)
  {
    spi.acquire(this);
      spi.begin();
        spi.transfer(REG_WRITE | reg);
	spi.transfer(value);
      spi.end();
    spi.release();
  }

  /**
   * Write multiple register values or fifo from given buffer.
   * @param[in] reg start register address.
   * @param[in] buf buffer with new register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void write(Reg reg, const void* buf, size_t count)
  {
    spi.acquire(this);
      spi.begin();
        spi.transfer(REG_WRITE | reg);
	spi.write(buf, count);
      spi.end();
    spi.release();
  }

  /**
   * Write multiple register values from given buffer in program memory.
   * @param[in] reg start register address.
   * @param[in] buf buffer in program memory with new register values.
   * @param[in] count size of buffer (and number of registers) to write
   */
  void write_P(Reg reg, const uint8_t* buf, size_t count)
  {
    spi.acquire(this);
      spi.begin();
        spi.transfer(REG_WRITE | reg);
	spi.write_P(buf, count);
      spi.end();
    spi.release();
  }

  /**
   * Register OP_MODE bitfields (Table 24, pp. 63).
   */
  enum {
    SEQUENCER_OFF = 0x80,		//!< Controls the automatic sequencer.
    SEQUENCER_ON = 0x00,		//!<
    LISTEN_OFF = 0x00,			//!< Enables listen mode.
    LISTEN_ON = 0x40,			//!<
    LISTEN_ABORT = 0x20,		//!< Aborts listen mode with LISTEN_OFF.
    MODE_MASK = 0x1C			//!< Tranceiver operation modes.
  } __attribute__((packed));

  /**
   * Register DATA_MODUL bitfields (Table 24, pp. 63).
   */
  enum {
    PACKET_MODE = 0x00,				//!< Data processing mode.
    CONTINUOUS_MODE_WITH_BIT_SYNC = 0x40,	//!<
    CONTINUOUS_MODE_WITHOUT_BIT_SYNC = 0x60,	//!<
    FSK_MODULATION = 0x00,			//!< Modulation scheme.
    OOK_MODULATION = 0x80,			//!<
    FSK_NO_SHAPING = 0x00,			//!< Modulation shaping (FSK).
    FSK_BT_1_0 = 0x01,				//!< Gaussian filter, BT = 1.0.
    FSK_BT_0_5 = 0x02,				//!< BT = 0.5.
    FSK_BT_0_3 = 0x03,				//!< BT = 0.3.
    OOK_NO_SHAPING = 0x00,			//!< Modulation shaping (OOK).
    OOK_BS_1 = 0x01,				//!< f(cutoff) = BR.
    OOK_BS_2 = 0x02,				//!< f(cutoff) = 2*BR.
  } __attribute__((packed));

  /**
   * Register OSC1 bitfields (Table 24, pp. 64).
   */
  enum {
    RC_CAL_START = 0x80,	//!< Trigger RC oscillator calibration.
    RC_CAL_DONE = 0x40		//!< Calibration completed flag.
  } __attribute__((packed));

  /**
   * Register AFC_CTRL bitfields (Table 24, pp. 64).
   */
  enum {
    AFC_LOW_BETA_OFF = 0x00,	//!< Standard AFC routine.
    AFC_LOW_BETA_ON = 0x20,	//!< Improved AFC routine.
  } __attribute__((packed));

  /**
   * Register LISTEN1 bitfields (Table 24, pp. 65).
   */
  enum {
    RESOL_IDLE_64_US = 0x40,		//!< Resolution of Listen mode idle time.
    RESOL_IDLE_410_US = 0x80,		//!< Calibrated RC oscillator.
    RESOL_IDLE_262000_US = 0xC0,	//!<
    RESOL_RX_64_US = 0x10,		//!< Resolution of Listen mode Rx time.
    RESOL_RX_410_US = 0x20,		//!< Calibrated RC oscillator.
    RESOL_RX_262000_US = 0x30,		//!<
    CRITERIA_RSSI_THRESHOLD = 0x00, 	//!< Packet acceptance criteria.
    CRITERIA_RSSI_SYNC_THRESHOLD = 0x08,//!< RSSI, RSSI & SYNC_ADDR.
    END_MODE0 = 0x00,			//!< Action after acceptance.
    END_MODE1 = 0x02,			//!<
    END_MODE2 = 0x04			//!<
  } __attribute__((packed));

  /**
   * Register PA_LEVEL bitfields (Table 25, pp. 66).
   */
  enum {
    PA0_ON = 0x80,		//!< Enables PA0, connected to RFIO and LNA.
    PA0_OFF = 0x00,		//!<
    PA1_ON = 0x40,		//!< Enables PA1, on PA_BOOST pin.
    PA1_OFF = 0x00,		//!<
    PA2_ON = 0x20,		//!< Enables PA2, on PA_BOOST pin.
    PA2_OFF = 0x00,		//!<
    OUTPUT_POWER = 0,		//!< Output power setting, in 1 dB steps.
    OUTPUT_POWER_MASK = 0x1F,	//!< Bitfield position and mask (5-bits).
    FULL_OUTPUT_POWER = 0x1F	//!<
  } __attribute__((packed));

  /**
   * Register OCP bitfields (Table 25, pp. 66).
   */
  enum {
    OCP_ON = 0x10,		//!< Enables overload current protection.
    OCP_OFF = 0x00,		//!<
    OCP_TRIM = 0,		//!< Trimming of OCP current.
    OCP_TRIM_MASK = 0x0F	//!< Bitfield position and mask (4-bit).
  } __attribute__((packed));

  /**
   * Register LNA bitfields (Table 26, pp. 67).
   */
  enum {
    ZIN_50_OHM = 0x00, 		//!< Input impedance.
    ZIN_200_OHM = 0x80,		//!< 50 or 200 ohm.
    CURRENT_GAIN = 3,		//!< Current gain.
    CURRENT_GAIN_MASK = 0x07,	//!< Bitfield position and mask (3-bit).
    SELECT_GAIN = 0,		//!< Gain setting.
    SELECT_GAIN_MASK = 0x07	//!< Bitfield position and mask (3-bit).
  } __attribute__((packed));

  /**
   * Register RX_BW bitfields (Table 26, pp. 67).
   */
  enum {
    DCC_FREQ = 5,		//!< Cut-off frequency of the DC offset canceller.
    DCC_FREQ_MASK = 0x07,	//!< Bitfield position and mask (3-bit).
    BW_MANT_16 = 0x00,		//!< Channel filter bandwidth control.
    BW_MANT_20 = 0x08,		//!<
    BW_MANT_24 = 0x10,		//!<
    BW_EXP = 0,			//!<
    BW_EXP_MASK = 0x07,		//!< Bitfield position and mask (3-bit).
  } __attribute__((packed));

  /**
   * Register OOK_PEAK bitfields (Table 26, pp. 68).
   */
  enum {
    OOK_THRESHOLD_FIXED = 0x00,	 	//!< Select type of threshold; fixed.
    OOK_THRESHOLD_PEAK = 0x40,		//!< type = peak.
    OOK_THRESHOLD_AVG = 0x80,		//!< type = average.
    OOK_PEAK_THRESHOLD_STEP = 3, 	//!< Size of decrement (dB).
    OOK_PEAK_THRESHOLD_STEP_MASK = 0x7, //!< Bitfield position and mask (3-bit).
    OOK_PEAK_THRESHOLD_DECR = 0, 	//!< Period of decrement (chips).
    OOK_PEAK_THRESHOLD_DECR_MASK = 0x7  //!< Bitfield position and mask (3-bit).
  } __attribute__((packed));

  /**
   * Register OOK_AVG bitfields (Table 26, pp. 68).
   */
  enum {
    OOK_AVG_THRESHOLD_FILTER_32_PI = 0x00, //!< Filter coefficients.
    OOK_AVG_THRESHOLD_FILTER_8_PI = 0x40,  //!< 32,8,4,2 PI.
    OOK_AVG_THRESHOLD_FILTER_4_PI = 0x80,  //!<
    OOK_AVG_THRESHOLD_FILTER_2_PI = 0xC0   //!<
  } __attribute__((packed));

  /**
   * Register AFC_FEI bitfields (Table 26, pp. 68).
   */
  enum {
    FEI_DONE = 0x40,		//!< FEI has finished.
    FEI_START = 0x20,		//!< Trigger a FEI measurement.
    AFC_DONE = 0x10,		//!< AFC has finished.
    AFC_AUTO_CLEAR_ON = 0x08,	//!< Clear before a new AFC phase.
    AFC_AUTO_ON = 0x04,		//!< AFC each time Rx mode is entered.
    AFC_CLEAR = 0x02,		//!< Clears the AFC value.
    AFC_START = 0x01		//!< Trigger an AFC.
  } __attribute__((packed));

  /**
   * Register RSSI_CONFIG bitfields (Table 26, pp. 68).
   */
  enum {
    RSSI_DONE = 0x02,		//!< RSSI sampling has finished.
    RSSI_START = 0x01		//!< Trigger a RSSI measurement.
  } __attribute__((packed));

  /**
   * Register DIO_MAPPING1/2 bitfields (Table 27, pp. 69).
   */
  enum {
    DIO0_MAPPING1 = 6,		//!< Mapping pins DIO0..3.
    DIO1_MAPPING1 = 4,		//!< Bitfield positions and mask.
    DIO2_MAPPING1 = 2,		//!<
    DIO3_MAPPING1 = 0,		//!<
    DIO4_MAPPING2 = 6,		//!< Mapping pins DIO4..5.
    DIO5_MAPPING2 = 4,		//!<
    DIO_MAPPING_MASK = 0x3,	//!<
    CLK_OUT = 0,		//!< CLKOUT frequency.
    CLK_OUT_MASK = 0x7		//!< Bitfield position and mask.
  } __attribute__((packed));

  /**
   * Register IRQ_FLAGS1 bitfields (Table 27, pp. 69).
   */
  enum {
    MODE_READY = 0x80,		//!< Operation mode ready.
    RX_READY = 0x40,		//!< Set in RX mode.
    TX_READY = 0x20,		//!< Set in TX mode.
    PLL_LOCKED = 0x10,		//!< Set when the PLL is locked.
    RSSI_READY = 0x08,		//!< Set in RX mode when threhold is exceeded.
    TIMEOUT = 0x04,		//!< Set when a timeout occurs.
    AUTO_MODE = 0x02,		//!< Set when entering intermediate mode.
    SYNC_ADDR_MATCH = 0x01,	//!< Set when sync word and node addres matched.
  } __attribute__((packed));

  /**
   * Register IRQ_FLAGS2 bitfields (Table 27, pp. 70).
   */
  enum {
    FIFO_FULL = 0x80,		//!< Set when FIFO is full.
    FIFO_NOT_EMPTY = 0x40,	//!< Set when FIFO contains at least one byte.
    FIFO_LEVEL = 0x20, 		//!< Set when the FIFO threshold is exceeded.
    FIFO_OVERRUN = 0x10,	//!< Set when FIFO overrun occurs.
    PACKET_SENT = 0x08,		//!< Set in TX when complete packet is sent.
    PAYLOAD_READY = 0x04,	//!< Set in RX when the payload is ready.
    CRC_OK = 0x02,		//!< Set in RX when the CRC is valid.
  } __attribute__((packed));

  /**
   * Register SYNC_CONFIG bitfields (Table 28, pp. 71).
   */
  enum {
    SYNC_ON = 0x80,		//!< Sync word generation.
    SYNC_OFF = 0x00,		//!<
    FIFO_FILL_AUTO = 0x00,	//!< FIFO filling condition.
    FIFO_FILL_MANUAL = 0x40,	//!<
    SYNC_SIZE = 3,		//!< Number of bytes in sync word plus one.
    SYNC_SIZE_MASK = 0x7,	//!< Bitfield and position (3-bits).
    SYNC_TOL = 0,		//!< Number of tolerated bit errors in sync word.
    SYNC_TOL_MASK = 0x7		//!< Bitfield and position (3-bits).
  } __attribute__((packed));

  /**
   * Register PACKET_CONFIG1 bitfields (Table 28, pp. 72).
   */
  enum {
    FIXED_LENGTH = 0x00,	//!< Packet formats; fixed.
    VARIABLE_LENGTH = 0x80,	//!< variable length.
    DC_FREE_OFF = 0x00,		//!< DC free encoding.
    MANCHESTER = 0x20,		//!<
    WHITENING = 0x40,		//!<
    CRC_OFF = 0x00,		//!< CRC calculation/check.
    CRC_ON = 0x10,		//!<
    CRC_AUTO_CLEAR_OFF = 0x08,	//!< Do not clear FIFO. PAYLOAD_READY issued.
    CRC_AUTO_CLEAR_ON = 0x00,	//!< Clear FIFO and restart new packet reception.
    ADDR_FILTER_OFF = 0x00,	//!< Address filtering based on node/boardcast.
    ADDR_FILTER_ON = 0x04,	//!< Node and broadcast address.
    ADDR_FILTER_NODE = 0x02	//!< Node address only.
  } __attribute__((packed));

  /**
   * Register PACKET_CONFIG2 bitfields (Table 28, pp. 73).
   */
  enum {
    INTER_PACKET_RX_DELAY = 4,	//!< Delay between FIFO empty and RSSI measurement.
    INTER_PACKET_RX_DELAY_MASK = 0xF,
    RESTART_RX = 0x04,		//!< Forces the Receiver in WAIT mode/continuous.
    AUTO_RX_RESTART_ON = 0x02,	//!< Use RX delay.
    AUTO_RX_RESTART_OFF = 0x00,	//!< Allow RESTART_RX.
    AES_ON = 0x01,		//!< Use AES encryption/decryption.
    AES_OFF = 0x00		//!< Do not use AES.
  } __attribute__((packed));

  /**
   * Register FIFO_THRESHOLD bitfields (Table 28, pp. 73).
   */
  enum {
    TX_START_THRESHOLD = 0x00,	//!< Start packet transmission on threshold.
    TX_START_NOT_EMPTY = 0x80,	//!< Dito when data in FIFO.
    FIFO_THRESHOLD_MASK = 0x7F	//!< Threshold value mask.
  } __attribute__((packed));

  /**
   * Register TEMP1 bitfields (Table 29, pp. 74).
   */
  enum {
    TEMP_MEAS_START = 0x08,	//!< Trigger the temperture measurement.
    TEMP_MEAS_RUNNING = 0x04	//!< Temperature measurement in progress.
  } __attribute__((packed));

  /**
   * Register TEST_LNA/PA1/PA2/DAGC/AFC values (Table 30, pp. 74).
   */
  enum {
    TEST_LNA_NORMAL_SENSITIVITY = 0x1B,
    TEST_LNA_HIGH_SENSITIVITY = 0x2D,
    TEST_PA1_NORMAL_MODE = 0x55,
    TEST_PA1_BOOST_20_DB_MODE = 0x5D,
    TEST_PA2_NORMAL_MODE = 0x70,
    TEST_PA2_BOOST_20_DB_MODE = 0x7C,
    TEST_DAGC_NORMAL_MODE = 0x00,
    TEST_DAGC_IMPROVED_MARGIN_AFC_LOW_BETA_ON = 0x20,
    TEST_DAGC_IMPROVED_MARGIN_AFC_LOG_BETA_OFF = 0x30
  } __attribute__((packed));

  /**
   * Register OP_MODE bitfields (Table 24, pp. 63).
   */
  enum Mode {
    SLEEP_MODE = 0x00,
    STANDBY_MODE = 0x04,
    FREQUENCY_SYNTHESIZER_MODE = 0x08,
    TRANSMITTER_MODE = 0x0C,
    RECEIVER_MODE = 0x10
  } __attribute__((packed));

  /**
   * Set the given operation mode and wait for mode to become ready.
   * @param[in] mode to set.
   */
  void set(Mode mode);

  /**
   * Handler for interrupt pin. Service interrupt on incoming messages
   * with valid checksum or message transmission completed.
   */
  class IRQPin : public ExternalInterrupt {
  public:
    /**
     * Construct interrupt pin handler for RFM69 on payload receive
     * interrupt.
     * @param[in] pin external interrupt pin.
     * @param[in] mode interrupt mode.
     * @param[in] rf device.
     */
    IRQPin(Board::ExternalInterruptPin pin, InterruptMode mode, RFM69* rf) :
      ExternalInterrupt(pin, mode),
      m_rf(rf)
    {}

    /**
     * @override{Interrupt::Handler}
     * Signal message has been receive and is available in receive fifo.
     * Or message has been sent and transceiver is ready.
     * @param[in] arg (not used).
     */
    virtual void on_interrupt(uint16_t arg = 0);

    friend class RFM69;
  private:
    RFM69* m_rf;		//!< Device reference.
  };

  /** Default configuration. */
  static const uint8_t config[] __PROGMEM;

  IRQPin m_irq;			//!< Interrupt pin and handler.
  volatile bool m_done;		//!< Packet sent flag (may be set by ISR).
  Mode m_opmode;		//!< Current operation mode.
};
#endif
#endif
