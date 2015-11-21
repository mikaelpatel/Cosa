/**
 * @file CC1101.hh
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

#ifndef COSA_CC1101_HH
#define COSA_CC1101_HH

#include "Cosa/SPI.hh"
#include "Cosa/OutputPin.hh"
#include "Cosa/ExternalInterrupt.hh"
#include "Cosa/Wireless.hh"

#if !defined(BOARD_ATTINYX5)

/**
 * Cosa Device Driver for Texas Instruments CC1101, Low-Power Sub-1
 * GHz RF Transceiver. Note that this device requires data in big
 * endian order.
 *
 * @section Circuit
 * This is the pin-out for the CC1101 module which is compatible with
 * the NRF24L01 module. CC1101 is a low voltage device (3V3) and
 * signals require level shifter (74HC4050 or 10K resistor). Special
 * care for MISO as this signal is multi-drop.
 * @code
 *                           CC1101
 *                       +------------+
 * (GND)---------------1-|GND         |
 * (3V3)---------------2-|VCC         |
 *                     3-|CDO0        |
 * (D10)------[ > ]----4-|CSN         |
 * (D13/SCK)--[ > ]----5-|SCK         |
 * (D11/MOSI)-[ > ]----6-|MOSI        |
 * (D12/MISO)-[ < ]----7-|MISO/GDO1   |
 * (D2/EXT0)--[ < ]----8-|GDO2        |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. Product Description, SWRS061H, Rev. H, 2012-10-09
 * http://www.ti.com/lit/ds/symlink/cc1101.pdf
 */
class CC1101 : private SPI::Driver, public Wireless::Driver {
public:
  /**
   * Maximum size of payload on device.
   */
  static const size_t DEVICE_PAYLOAD_MAX = 64;

  /**
   * Maximum size of payload. The device allows 64 bytes payload.
   * The length and destination addressing will require two bytes,
   * source address one byte, and port one byte. This gives a payload
   * max of 64 - 4 = 60.
   */
  static const size_t PAYLOAD_MAX = DEVICE_PAYLOAD_MAX - 4;

  /**
   * Construct C1101 device driver with given network and device
   * address. Connected to SPI bus and given chip select pin. Default
   * pins are Arduino Nano IO Shield for CC1101 module are D10 chip
   * select and D2/EXT0 external interrupt pin (TinyX4/Mega/Standard).
   * @param[in] net network address.
   * @param[in] dev device address.
   * @param[in] csn chip select pin (Default D2/D10/D53).
   * @param[in] irq interrupt pin (Default EXT0/EXT0/EXT4).
   */
#if defined(BOARD_ATTINYX4)
  CC1101(uint16_t net, uint8_t dev,
	 Board::DigitalPin csn = Board::D2,
	 Board::ExternalInterruptPin irq = Board::EXT0);
#elif defined(BOARD_ATMEGA2560)
  CC1101(uint16_t net, uint8_t dev,
	 Board::DigitalPin csn = Board::D53,
	 Board::ExternalInterruptPin irq = Board::EXT4);
#else
  CC1101(uint16_t net, uint8_t dev,
	 Board::DigitalPin csn = Board::D10,
	 Board::ExternalInterruptPin irq = Board::EXT0);
#endif

  /**
   * @override{Wireless::Driver}
   * Start and configure C1101 device driver. The configuration must
   * set GDO2 to assert on received message. This device pin is
   * assumed to be connected the device driver interrupt pin (EXTn).
   * Return true(1) if successful othewise false(0).
   * @param[in] config configuration vector (default NULL)
   * @return bool.
   */
  virtual bool begin(const void* config = NULL);

  /**
   * @override{Wireless::Driver}
   * Shutdown the device driver. Return true(1) if successful
   * otherwise false(0).
   * @return bool.
   */
  virtual bool end();

  /**
   * @override{Wireless::Driver}
   * Send message in given null terminated io vector. Returns number
   * of bytes sent. Returns error code(-1) if number of bytes is
   * greater than PAYLOAD_MAX. Return error code(-2) if fails to set
   * transmit mode.
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
   * set transmit mode. Note that port numbers (128 and higher are
   * reserved for system protocols).
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
   * @param[in] ms maximum time out period (Default blocking(0L)).
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
   * Set output power level (-30..10 dBm)
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
   * @override{Wireless::Driver}
   * Return link quality indicator from latest successful receive
   * message. Lower level is better quality.
   */
  virtual int link_quality_indicator()
  {
    return (m_recv_status.lqi);
  }

protected:
  /**
   * Transaction header (pp. 29). Note 16-bit configuration variables are
   * read/written in big endian order (MSB first) and require swapping.
   */
  union header_t {
    uint8_t as_uint8;		//!< 8-bit representation.
    struct {			//!< Bit-field representation (little endian).
      uint8_t reg:6;		//!< Register address.
      uint8_t burst:1;		//!< Burst(1) or Single(0) byte mode.
      uint8_t rw:1;		//!< Read(1) or Write(0).
    };

    /**
     * Construct header with given register address, burst and read
     * flag.
     * @param[in] addr register address.
     * @param[in] is_burst flag for burst mode.
     * @param[in] is_read flag for read mode.
     */
    header_t(uint8_t addr, uint8_t is_burst, uint8_t is_read)
    {
      reg = addr;
      burst = is_burst;
      rw = is_read;
    }

    /**
     * Cast header bit-field to byte.
     * @return byte representation.
     */
    operator uint8_t()
    {
      return (as_uint8);
    }
  };

  /**
   * Read single register value and status. Access status with
   * status(). Returns register value.
   * @param[in] reg register address.
   * @return value.
   */
  uint8_t read(uint8_t reg)
  {
    m_status = spi.transfer(header_t(reg, 0, 1));
    uint8_t res = spi.transfer(0);
    return (res);
  }

  /**
   * Read multiple register values into given buffer. Access status
   * with status().
   * @param[in] reg start register address.
   * @param[in] buf buffer to store register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void read(uint8_t reg, void* buf, size_t count)
  {
    m_status = spi.transfer(header_t(reg, 1, 1));
    spi.read(buf, count);
  }

  /**
   * Write single register value. Access status with status().
   * @param[in] reg register address.
   * @param[in] value to write to register.
   */
  void write(uint8_t reg, uint8_t value)
  {
    m_status = spi.transfer(header_t(reg, 0, 0));
    spi.transfer(value);
  }

  /**
   * Write multiple register values from given buffer. Access status
   * with status().
   * @param[in] reg start register address.
   * @param[in] buf buffer with new register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void write(uint8_t reg, const void* buf, size_t count)
  {
    m_status = spi.transfer(header_t(reg, 1, 0));
    spi.write(buf, count);
  }

  /**
   * Write multiple register values from given buffer in program memory.
   * Access status with status().
   * @param[in] reg start register address.
   * @param[in] buf buffer in program memory with new register values.
   * @param[in] count size of buffer (and number of registers) to write
   */
  void write_P(uint8_t reg, const uint8_t* buf, size_t count)
  {
    m_status = spi.transfer(header_t(reg, 1, 0));
    spi.write_P(buf, count);
  }

  /**
   * Configuration Registers (Table 43, pp. 68).
   */
  enum Config {
    IOCFG2 = 0x00,		//!< GDO2 output pin configuration.
    IOCFG1 = 0x01,		//!< GDO1 output pin configuration.
    IOCFG0 = 0x02,		//!< GDO0 output pin configuration.
    FIFOTH = 0x03,		//!< RX FIFO and TX FIFO thresholds.
    SYNC1  = 0x04,		//!< Sync word, high byte.
    SYNC0  = 0x05,		//!< Sync word, low byte.
    PKTLEN = 0x06,		//!< Packet length.
    PKTCTR = 0x07,		//!< Packet automation control.
    PKTCTRL0 = 0x08,		//!< Packet automation control.
    ADDR = 0x09,		//!< Device address.
    CHANNR = 0x0A,		//!< Channel number.
    FSCTRL1 = 0x0B,		//!< Frequency synthesizer control.
    FSCTRL0 = 0x0C,		//!< Frequency synthesizer control.
    FREQ2 = 0x0D,		//!< Frequency control word, high byte.
    FREQ1 = 0x0E,		//!< Frequency control word, middle byte.
    FREQ0 = 0x0F,		//!< Frequency control word, low byte.
    MDMCFG4 = 0x10,		//!< Modem configuration.
    MDMCFG3 = 0x11,		//!< Modem configuration.
    MDMCFG2 = 0x12,		//!< Modem configuration.
    MDMCFG1 = 0x13,		//!< Modem configuration.
    MDMCFG0 = 0x14,		//!< Modem configuration.
    DEVIATN = 0x15,		//!< Modem deviation setting.
    MCSM2 = 0x16,	        //!< Main Radio Cntrl State Machine config.
    MCSM1 = 0x17,	        //!< Main Radio Cntrl State Machine config.
    MCSM0 = 0x18,	        //!< Main Radio Cntrl State Machine config.
    FOCCFG = 0x19,	        //!< Frequency Offset Compensation config.
    BSCFG = 0x1A,		//!< Bit Synchronization configuration.
    AGCCTRL2 = 0x1B,		//!< AGC control.
    AGCCTRL1 = 0x1C,		//!< AGC control.
    AGCCTRL0 = 0x1D,		//!< AGC control.
    WOREVT1 = 0x1E,		//!< High byte Event 0 timeout.
    WOREVT0 = 0x1F,		//!< Low byte Event 0 timeout.
    WORCTRL = 0x20,		//!< Wake On Radio control.
    FREND1 = 0x21,		//!< Front end RX configuration.
    FREND0 = 0x22,		//!< Front end TX configuration.
    FSCAL3 = 0x23,		//!< Frequency synthesizer calibration.
    FSCAL2 = 0x24,		//!< Frequency synthesizer calibration.
    FSCAL1 = 0x25,		//!< Frequency synthesizer calibration.
    FSCAL0 = 0x26,		//!< Frequency synthesizer calibration.
    RCCTRL1 = 0x27,		//!< RC oscillator configuration.
    RCCTRL0 = 0x28,		//!< RC oscillator configuration.
    FSTEST = 0x29,		//!< Frequency synthesizer cal control.
    PTEST = 0x2A,		//!< Production test.
    AGCTEST = 0x2B,		//!< AGC test.
    TEST2 = 0x2C,		//!< Various test settings.
    TEST1 = 0x2D,		//!< Various test settings.
    TEST0 = 0x2E,		//!< Various test settings.
    CONFIG_MAX = 0x29		//!< Number of configuration registers.
  } __attribute__((packed));

  /**
   * Read single configuration register value.
   * @param[in] reg register address.
   * @return value
   */
  uint8_t read(Config reg)
  {
    return (read((uint8_t) reg));
  }

  /**
   * Read given number of configuration register values into given
   * buffer.
   * @param[in] reg start register address.
   * @param[in] buf buffer to store register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void read(Config reg, void* buf, size_t count)
  {
    read((uint8_t) reg, buf, count);
  }

  /**
   * Write single configuration register value.
   * @param[in] reg register address.
   * @param[in] value to write to register.
   */
  void write(Config reg, uint8_t value)
  {
    write((uint8_t) reg, value);
  }

  /**
   * Write given number of configuration register values from given buffer.
   * @param[in] reg start register address.
   * @param[in] buf buffer with new register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void write(Config reg, const void* buf, size_t count)
  {
    write((uint8_t) reg, buf, count);
  }

  /**
   * Write multiple configuration register values from given buffer in
   * program memory.
   * @param[in] reg start register address.
   * @param[in] buf buffer in program memory with new register values.
   * @param[in] count size of buffer (and number of registers) to write
   */
  void write_P(Config reg, const uint8_t* buf, size_t count)
  {
    write_P((uint8_t) reg, buf, count);
  }

  /**
   * Data access registers (chap. FIFO and PATABLE Access, pp. 32-33).
   */
  enum Data {
    PATABLE = 0x3E,		//!< PA control table.
    TXFIFO = 0x3F,		//!< Transmitter FIFO.
    RXFIFO = 0x3F,		//!< Receiver FIFO.
  } __attribute__((packed));

  /**
   * Maximum size of PA table.
   */
  static const size_t PATABLE_MAX = 8;

  /**
   * Read value from data register.
   * @param[in] reg data register address.
   * @return value
   */
  uint8_t read(Data reg)
  {
    return (read((uint8_t) reg));
  }

  /**
   * Read data register values into given buffer.
   * @param[in] reg register address.
   * @param[in] buf buffer to store register values.
   * @param[in] count size of buffer and number of registers to read.
   */
  void read(Data reg, void* buf, size_t count)
  {
    read((uint8_t) reg, buf, count);
  }

  /**
   * Write value to data register.
   * @param[in] reg register address.
   * @param[in] value to write to register.
   */
  void write(Data reg, uint8_t value)
  {
    write((uint8_t) reg, value);
  }

  /**
   * Write given buffer to data register (Table/FIFO).
   * @param[in] reg data register address.
   * @param[in] buf buffer with data.
   * @param[in] count size of buffer and number of bytes to write.
   */
  void write(Data reg, const void* buf, size_t count)
  {
    write((uint8_t) reg, buf, count);
  }

  /**
   * Write fiven buffer in program memory to data register.
   * @param[in] reg data register address.
   * @param[in] buf buffer in program memory with new data to write.
   * @param[in] count size of buffer.
   */
  void write_P(Data reg, const uint8_t* buf, size_t count)
  {
    write_P((uint8_t) reg, buf, count);
  }

  /**
   * Status Registers (Table 44, pp. 69).
   */
  enum Status {
    PARTNUM = 0x30,		//!< Part number.
    VERSION = 0x31,		//!< Current version number.
    FREQEST = 0x32,		//!< Frequency offset estimate.
    LQI = 0x33,		        //!< Demodulator estimate for link quality.
    RSSI = 0x34,	        //!< Received signal strength indication.
    MARCSTATE = 0x35,	        //!< Control state machine state.
    WORTIME1 = 0x36,		//!< High byte of WOR timer.
    WORTIME0 = 0x37,		//!< Low byte of WOR timer.
    PKTSTATUS = 0x38,	        //!< Current GDOx status and packet status.
    VCO = 0x39,		        //!< Current setting from PLL cal module.
    TXBYTES = 0x3A,	        //!< Underflow and # of bytes in TXFIFO.
    RXBYTES = 0x3B,	        //!< Overflow and # of bytes in RXFIFO.
    BYTES_MASK = 0x7f,		//!< Mask # bytes.
    FIFO_MASK = 0x80,		//!< Mask fifo state.
    RCCTRL1_STATUS = 0x3C,	//!< Last RC oscillator calibration result.
    RCCTRL0_STATUS = 0x3D,	//!< Last RC oscillator calibration result.
    STATUS_MAX = 0x0E,		//!< Number of status registers.
  } __attribute__((packed));

  /**
   * Read single status register value.
   * @param[in] reg register address.
   * @return value
   */
  uint8_t read(Status reg)
  {
    uint8_t res;
    read((uint8_t) reg, &res, sizeof(res));
    return (res);
  }

  /**
   * Command Strobes (Table 42, pp. 67).
   */
  enum Command {
    SRES = 0x30,		//!< Reset chip.
    SFSTXON = 0x31,             //!< Enable and calibrate frequency synthesizer.
    SXOFF = 0x32,		//!< Turn off crystal oscillator.
    SCAL = 0x33,		//!< Calibrate frequency synthesizer.
    SRX = 0x34,			//!< Enable RX.
    STX = 0x35,	                //!< Enable TX.
    SIDLE = 0x36,               //!< Exit RX/TX.
    SAFC = 0x37,                //!< AFC adjustment of the frequency synthesizer.
    SWOR = 0x38,                //!< Start automatic Wake-on-Radio.
    SPWD = 0x39,	        //!< Power down mode when CSn goes high.
    SFRX = 0x3A,		//!< Flush the RX FIFO buffer.
    SFTX = 0x3B,		//!< Flush the TX FIFO buffer.
    SWORRST = 0x3C,		//!< Reset real time clock.
    SNOP = 0x3D			//!< No operation.
  } __attribute__((packed));

  /**
   * Issue given command to device. Check documentation for required
   * timing delay per command.
   * @param[in] cmd command.
   */
  void strobe(Command cmd);

  /**
   * Status Byte Summary (Table 23, pp. 31).
   */
  enum Mode {		  	//!< Main State Machine Mode.
    IDLE_MODE = 0,		//!< Idle state.
    RX_MODE,			//!< Receiver mode.
    TX_MODE,			//!< Transmit mode.
    FSTXON_MODE,		//!< Fast Transmit ready.
    CALIBRATION_MODE,		//!< Frequency synthesizer calibration running.
    SETTLING_MODE,		//!< PLL is settling.
    RXFIFO_OVERFLOW_MODE,	//!< RX FIFO has overflowed.
    TXFIFO_UNDERFLOW_MODE	//!< TX FIFO har underflowed.
  } __attribute__((packed));

  union status_t {
    uint8_t as_uint8;		//!< 8-bit representation.
    struct {			//!< Bit-field representation (little endian).
      uint8_t avail:4;		//!< Number of bytes in RX or TX FIFO.
      uint8_t mode:3;		//!< Current main state machine mode.
      uint8_t ready:1;		//!< Chip ready.
    };

    status_t(uint8_t value)
    {
      as_uint8 = value;
    }
  };

  /**
   * Get latest transaction status.
   * @return status
   */
  status_t status() const
  {
    return (m_status);
  }

  /**
   * Read status byte.
   * @param[in] fifo status (0 = write, 1 = read)
   * @return status
   */
  status_t read_status(uint8_t fifo = 1)
  {
    spi.acquire(this);
      spi.begin();
        m_status = spi.transfer(header_t(0,0,fifo));
      spi.end();
    spi.release();
    return (m_status);
  }

  /**
   * Await given mode.
   * @param[in] mode to await.
   */
  void await(Mode mode);

  /**
   * Main Radio Control State Machine State (pp. 93).
   */
  enum State {			  	//!< State (Figure 24, pp 50).
    SLEEP_STATE = 0x00,			//!< SLEEP.
    IDLE_STATE = 0x01,			//!< IDLE.
    XOFF_STATE = 0x02,			//!< XOFF.
    VCOON_MC_STATE = 0x03,		//!< MANCAL.
    REGON_MC_STATE = 0x04,		//!< MANCAL.
    MANCAL_STATE = 0x05,		//!< MANCAL.
    VCOON_STATE = 0x06,			//!< FS_WAKEUP.
    REGON_STATE = 0x07,			//!< FS_WAKEUP.
    STARTCAL_STATE = 0x08,		//!< CALIBRATE.
    BWBOOST_STATE = 0x09,		//!< SETTLING.
    FS_LOCK_STATE = 0x0A,		//!< SETTLING.
    IFADCON_STATE = 0x0B,		//!< SETTLING.
    ENDCAL_STATE = 0x0C,		//!< CALIBRATE.
    RX_STATE = 0x0D,			//!< RX.
    RX_END_STATE = 0x0E,		//!< RX.
    RX_RST_STATE = 0x0F,		//!< RX.
    TXRX_SWITCH_STATE = 0x10,		//!< TXRX_SETTLING.
    RXFIFO_OVERFLOW_STATE = 0x11,	//!< RXFIFO_OVERFLOW.
    FSTXON_STATE = 0x12,		//!< FSTXON.
    TX_STATE = 0x13,			//!< TX.
    TX_END_STATE = 0x14,		//!< TX.
    RXTX_SWITCH_STATE = 0x15,		//!< RXTX_SETTLING.
    TXFIFO_UNDERFLOW_STATE = 0x16	//!< TXFIFO_UNDERFLOW.
  } __attribute__((packed));

  /**
   * Read Main Radio Control State Machine State.
   * @return state
   */
  State read_marc_state()
  {
    return ((State) read(MARCSTATE));
  }

  /**
   * Received Message Status Bytes (Table. 27/28, pp. 37).
   */
  union recv_status_t {
    uint8_t status[2];		//!< Two status bytes last in frame.
    struct {			//!< Bit-field representation (little endian).
      uint8_t rssi;		//!< RSSI value.
      uint8_t lqi:7;		//!< Link Quality Indication.
      uint8_t crc:1;		//!< CRC status.
    };
  };

  /**
   * Handler for interrupt pin. Service interrupt on incoming message
   * with valid checksum.
   */
  class IRQPin : public ExternalInterrupt {
    friend class CC1101;
  public:
    /**
     * Construct interrupt pin handler for CC1101 on message receive
     * interrupt.
     * @param[in] pin external interrupt pin.
     * @param[in] mode interrupt mode.
     * @param[in] rf device.
     */
    IRQPin(Board::ExternalInterruptPin pin, InterruptMode mode, CC1101* rf) :
      ExternalInterrupt(pin, mode, true),
      m_rf(rf)
    {}

    /**
     * @override{Interrupt::Handler}
     * Signal message has been receive and is available in receive fifo.
     * @param[in] arg (not used).
     */
    virtual void on_interrupt(uint16_t arg = 0);

  private:
    CC1101* m_rf;		//!< Device reference.
  };

private:
  /** Default configuration. */
  static const uint8_t config[] __PROGMEM;

  IRQPin m_irq;			//!< Interrupt pin and handler.
  status_t m_status;		//!< Status from latest transaction.
  recv_status_t m_recv_status;	//!< Status frm latest message receive.
};
#endif
#endif
