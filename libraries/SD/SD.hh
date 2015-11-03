/**
 * @file SD.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2015, Mikael Patel
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

#ifndef COSA_SD_HH
#define COSA_SD_HH

#include "Cosa/Types.h"
#include "Cosa/SPI.hh"

/**
 * Cosa SD low-level device driver class. Implements disk driver
 * connect/disconnect, erase, read and write block.
 *
 * @section References
 * 1. SD Specification, Part 1: Physical Layer, Simplified Specification,
 * Version 4.10, January 22, 2013. https://www.sdcard.org/downloads/pls/simplified_specs/part1_410.pdf
 */
class SD : private SPI::Driver {
public:
  /** Max size of block. */
  static const size_t BLOCK_MAX = 512;

  /** Supported card types. */
  enum CARD {
    TYPE_UNKNOWN = 0,
    TYPE_SD1 = 1,
    TYPE_SD2 = 2,
    TYPE_SDHC = 3
  } __attribute__((packed));

  /** CID, Card Identification register, table 5-2. */
  struct cid_t {
    uint8_t mid;		//!< Manufacturer ID.
    char oid[2];		//!< OEM/Application ID.
    char pnm[5];		//!< Product name.
    uint8_t prv; 		//!< Product revision.
    uint32_t psn;		//!< Product serial number.
    uint16_t mdt;		//!< Manufacturing date.
    uint8_t crc;		//!< CRC7 checksum.
  };

  /** CSD, Card-Specific Data register, version 1.00. */
  struct csd_v1_t {
    uint8_t reserved1:6;
    uint8_t csd_ver:2;
    uint8_t taac;
    uint8_t nsac;
    uint8_t tran_speed;
    uint8_t ccc_high;
    uint8_t read_bl_len:4;
    uint8_t ccc_low:4;
    uint8_t c_size_high:2;
    uint8_t reserved2:2;
    uint8_t dsr_imp:1;
    uint8_t read_blk_misalign:1;
    uint8_t write_blk_misalign:1;
    uint8_t read_bl_partial:1;
    uint8_t c_size_mid;
    uint8_t vdd_r_curr_max:3;
    uint8_t vdd_r_curr_min:3;
    uint8_t c_size_low :2;
    uint8_t c_size_mult_high:2;
    uint8_t vdd_w_cur_max:3;
    uint8_t vdd_w_curr_min:3;
    uint8_t sector_size_high:6;
    uint8_t erase_blk_en:1;
    uint8_t c_size_mult_low:1;
    uint8_t wp_grp_size:7;
    uint8_t sector_size_low:1;
    uint8_t write_bl_len_high:2;
    uint8_t r2w_factor:3;
    uint8_t reserved3:2;
    uint8_t wp_grp_enable:1;
    uint8_t reserved4:5;
    uint8_t write_partial:1;
    uint8_t write_bl_len_low:2;
    uint8_t reserved5:2;
    uint8_t file_format:2;
    uint8_t tmp_write_protect:1;
    uint8_t perm_write_protect:1;
    uint8_t copy:1;
    uint8_t file_format_grp:1;
    uint8_t crc;
  };

  /** CSD, Card-Specific Data register, version 2.00. */
  struct csd_v2_t {
    uint8_t reserved1:6;
    uint8_t csd_ver:2;
    uint8_t taac;
    uint8_t nsac;
    uint8_t tran_speed;
    uint8_t ccc_high;
    uint8_t read_bl_len:4;
    uint8_t ccc_low:4;
    uint8_t reserved2:4;
    uint8_t dsr_imp:1;
    uint8_t read_blk_misalign:1;
    uint8_t write_blk_misalign:1;
    uint8_t read_bl_partial:1;
    uint8_t c_size_high:6;
    uint8_t reserved3:2;
    uint8_t c_size_mid;
    uint8_t c_size_low;
    uint8_t sector_size_high:6;
    uint8_t erase_blk_en:1;
    uint8_t reserved4:1;
    uint8_t wp_grp_size:7;
    uint8_t sector_size_low:1;
    uint8_t write_bl_len_high:2;
    uint8_t r2w_factor:3;
    uint8_t reserved5:2;
    uint8_t wp_grp_enable:1;
    uint8_t reserved6:5;
    uint8_t write_partial:1;
    uint8_t write_bl_len_low:2;
    uint8_t reserved7:2;
    uint8_t file_format:2;
    uint8_t tmp_write_protect:1;
    uint8_t perm_write_protect:1;
    uint8_t copy:1;
    uint8_t file_format_grp:1;
    uint8_t crc;
  };

  /** CSD, Card-Specific Data register. */
  union csd_t {
    csd_v1_t v1;
    csd_v2_t v2;
  };

protected:
  /** Command Abbreviations. */
  enum CMD {
    GO_IDLE_STATE = 0,		//!< Reset the SD Memory Card.
    ALL_SEND_CID = 2,		//!< Sends host capacity support information.
    SEND_RELATIVE_ADDR = 3,	//!< Checks switchable function.
    SET_DSR = 4,		//!< Programs the DSR of all cards.
    SWITCH_FUNC = 6,		//!< Checks switchable function.
    SELECT_DESELECT_CARD = 7,	//!< Toggles card state (stdby/prog and disc).
    SEND_IF_COND = 8,		//!< Sends SD Memory Card interface condition.
    SEND_CSD = 9,		//!< Asks the selected card to send CSD.
    SEND_CID = 10,		//!< Asks the selected card to send CID.
    VOLTAGE_SWITCH = 11,	//!< Switch to 1V8 bus signaling level.
    STOP_TRANSMISSION = 12, 	//!< Stop Multiple Block Read.
    SEND_STATUS = 13,		//!< Asks the selected card status register.
    GO_INACTIVE_STATE = 15,	//!< Addressed card into inactive state.
    SET_BLOCKLEN = 16,		//!< Set block length (in bytes).
    READ_SINGLE_BLOCK = 17,	//!< Read block length bytes.
    READ_MULTIPLE_BLOCK = 18,	//!< Read blocks until STOP_TRANSMISSION.
    SEND_TUNING_BLOCK = 19,	//!< Send 64 byte tuning pattern to card.
    SPEED_CLASS_CONTROL = 20,	//!< Check speed class.
    SET_BLOCK_COUNT = 23,	//!< Specify block count for multiple block.
    WRITE_BLOCK = 24,		//!< Write block length bytes.
    WRITE_MULTIPLE_BLOCK = 25,	//!< Write block until STOP_TRANSMISSION.
    PROGRAM_CSD = 27,		//!< Set programmable bits in CSD.
    SET_WRITE_PROT = 28,	//!< Set write protect bit.
    CLR_WRITE_PROT = 29,	//!< Clears write protect bit.
    SEND_WRITE_PROT = 30,	//!< Read write protect bit.
    ERASE_WR_BLK_START = 32,	//!< Set first write block to be erased.
    ERASE_WR_BLK_END = 33,	//!< Set last write block to be erased.
    ERASE = 38,			//!< Erases selected write blocks.
    LOCK_UNLOCK = 40,		//!< Set/reset password or unlock/lock card.
    APP_CMD = 55,		//!< Next command is application specific command.
    GEN_CMD = 56,		//!< Data block for application specific command.
    READ_OCR = 58,		//!< Read OCR register of a card.
    CRC_ON_OFF = 59		//!< Turns the CRC on/off.
  } __attribute__((packed));

  /** Table 4-28: Application-Specific Commands. */
  enum ACMD {
    SET_BUS_WIDTH = 6,		//!< Defines the data bus width.
    SD_STATUS = 13,		//!< Card status.
    SEND_NUM_WR_BLOCKS = 22,	//!< Number of write blocks.
    SET_WR_BLK_ERASE_COUNT = 23, //!< Number of pre-erased write blocks.
    SD_SEND_OP_COND = 41,	 //!< Host capacity support information.
    SET_CLR_CARD_DETECT = 42,	 //!< Enable pull-up resistor on CD/DAT3 (pin 1).
    SEND_SCR = 51		 //!< Reads the Configuration Register.
  } __attribute__((packed));

  /** Card States. */
  enum STATE {
    IDLE_STATE = 0,
    READY_STATE = 1,
    IDENT_STATE = 2,
    STBY_STATE = 3,
    TRAN_STATE = 4,
    DATA_STATE = 5,
    RCV_STATE = 6,
    PRG_STATE = 7,
    DIS_STATE = 8
  } __attribute__((packed));;

  /** R1 (Status). */
  union R1 {
    uint8_t as_uint8;
    struct {
      uint8_t in_idle_state:1;
      uint8_t erase_reset:1;
      uint8_t illegal_command:1;
      uint8_t com_crc_error:1;
      uint8_t erase_sequence_error:1;
      uint8_t address_error:1;
      uint8_t parameter_error:1;
      uint8_t reserved:1;
    };
    R1(uint8_t value = 0) { as_uint8 = value; }
    bool is_error() const { return ((as_uint8 & 0x7e) != 0); }
    bool is_ready() const { return (as_uint8 == 0); }
  };

  /** R2 (Extended status). */
  union R2 {
    uint8_t as_uint8;
    struct {
      uint8_t card_is_locked:1;
      uint8_t wp_erased:1;
      uint8_t error:1;
      uint8_t cc_error:1;
      uint8_t card_ecc_failed:1;
      uint8_t wp_violation:1;
      uint8_t erase_param:1;
      uint8_t out_of_range:1;
    };
    R2(uint8_t value = 0) { as_uint8 = value; }
  };

  /** R3 (OCR register). */
  struct R3 {
    uint32_t vdd_voltage_window:24;
    uint8_t switch_to_1V8_accepted:1;
    uint8_t reserved:4;
    uint8_t uhs_ii_card:1;
    uint8_t card_capacity:1;
    uint8_t card_power_up:1;
  };

  /** R6 (Published RCA response). */
  union R6 {
    uint32_t as_uint32;
    struct {
      uint16_t rca;
      uint16_t status;
    };
    R6(uint32_t value = 0L) { as_uint32 = value; }
  };

  /** R7 (Card interface condition). */
  union R7 {
    uint32_t as_uint32;
    struct {
      uint8_t check_pattern;
      uint8_t voltage_accepted:4;
      uint32_t reserved:16;
      uint8_t command_version:4;
    };
    R7(uint32_t value = 0L) { as_uint32 = value; }
  };

  /** SD initialization check pattern. */
  static const uint8_t CHECK_PATTERN = 0xAA;

  enum TOKEN {
    DATA_START_BLOCK = 0XFE,
    WRITE_MULTIPLE_TOKEN = 0XFC,
    STOP_TRAN_TOKEN = 0XFD,
    DATA_RES_MASK = 0X1F,
    DATA_RES_ACCEPTED = 0X05,
    DATA_RES_CRC_ERR = 0X0B,
    DATA_RES_WR_ERR = 0X0E
  } __attribute__((packed));

  /** SD request block with command, arugment and check sum (CRC7). */
  struct request_t {
    uint8_t command;
    uint32_t arg;
    uint8_t crc;
  };

  /** Internal timeout periods. */
  static const uint16_t INIT_TIMEOUT = 2000;
  static const uint16_t ERASE_TIMEOUT = 10000;
  static const uint16_t READ_TIMEOUT = 300;
  static const uint16_t WRITE_TIMEOUT = 600;

  /** Internal number of init pulses. */
  static const uint8_t INIT_PULSES = 10;

  /** Internal number of retry. */
  static const uint8_t INIT_RETRY = 200;
  static const uint8_t RESPONSE_RETRY = 100;

  /** Response from latest command. */
  uint8_t m_response;

  /** Detected card type. */
  CARD m_type;

  /**
   * Send given command and argument. Returns R1 response byte.
   * @param[in] command to send.
   * @param[in] arg argument (default 0L).
   * @return response byte (R1).
   */
  uint8_t send(CMD command, uint32_t arg = 0L);

  /**
   * Send given command and argument. Resend if response any error state.
   * Return true if command was issued successfully otherwise false.
   * @param[in] ms number of milli-seconds to resend.
   * @param[in] command to send.
   * @param[in] arg argument (default 0L).
   * @return bool.
   */
  bool send(uint16_t ms, CMD command, uint32_t arg = 0L);

  /**
   * Send given application specific command and argument. Returns R1
   * response byte.
   * @param[in] command to send.
   * @param[in] arg argument (default 0L).
   * @return response byte (R1).
   */
  uint8_t send(ACMD command, uint32_t arg = 0L);

  /**
   * Send given application specific command and argument. Resend if
   * response any error state.
   * Return true if command was issued successfully otherwise false.
   * @param[in] ms number of milli-seconds to resend.
   * @param[in] command to send.
   * @param[in] arg argument (default 0L).
   * @return bool.
   */
  bool send(uint16_t ms, ACMD command, uint32_t arg = 0L);

  /**
   * Await the completion of a command. Wait for at most given period
   * in milli-seconds (default zero will block). Return true if
   * command was completed otherwise false if the time limit was exceeded.
   * @param[in] ms timeout period in number of milli-seconds (default 0).
   * @param[in] token to await (default any).
   * @return bool.
   */
  bool await(uint16_t ms = 0, uint8_t token = 0);

  /**
   * Receive 32-bit response from device.
   * @return long reponse.
   */
  uint32_t receive();

  /**
   * Send given command and argument and transfer data response into
   * given buffer with given number of bytes. Returns true if
   * successful otherwise false.
   * @param[in] command to send.
   * @param[in] arg argument.
   * @param[in] buf pointer to buffer for response data.
   * @param[in] count number of bytes.
   * @return bool.
   */
  bool read(CMD command, uint32_t arg, void* buf, size_t count);

public:
  /**
   * Construct Secure Disk low-level SPI device driver with given chip
   * select pin.
   * @param[in] csn chip select pin.
   */
#if defined(BOARD_ATTINYX5)
  SD(Board::DigitalPin csn = Board::D3) :
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV128_CLOCK, 0, SPI::MSB_ORDER, NULL),
    m_type(TYPE_UNKNOWN)
  {}
#elif defined(WICKEDDEVICE_WILDFIRE)
  SD(Board::DigitalPin csn = Board::D16) :
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV128_CLOCK, 0, SPI::MSB_ORDER, NULL),
    m_type(TYPE_UNKNOWN)
  {}
#else
  SD(Board::DigitalPin csn = Board::D8) :
    SPI::Driver(csn, SPI::ACTIVE_LOW, SPI::DIV128_CLOCK, 0, SPI::MSB_ORDER, NULL),
    m_type(TYPE_UNKNOWN)
  {}
#endif

  /**
   * Return detected card type.
   */
  CARD type() const
  {
    return (m_type);
  }

  /**
   * Initiate communication with memory card. Returns true if
   * successful and a memory card was detected, otherwise false.
   * @param[in] rate clock for device.
   * @return bool.
   */
  bool begin(SPI::Clock rate = SPI::DIV128_CLOCK);

  /**
   * Stop communication with memory card. Flush internal state and
   * allow card to be removed. Returns true if successful otherwise
   * false.
   * @return bool.
   */
  bool end();

  /**
   * Erase given block interval from start to end. Returns true if
   * successful otherwise false.
   * @param[in] start block address.
   * @param[in] end block address.
   * @return bool.
   */
  bool erase(uint32_t start, uint32_t end);

  /**
   * Read given block into given destination buffer. The buffer must
   * be able to hold BLOCK_MAX bytes. Returns true if successful
   * otherwise false.
   * @param[in] block address.
   * @param[in] dst pointer to destination buffer.
   * @return bool.
   */
  bool read(uint32_t block, uint8_t* dst)
    __attribute__((always_inline))
  {
    if (m_type != TYPE_SDHC) block <<= 9;
    return (read(READ_SINGLE_BLOCK, block, dst, BLOCK_MAX));
  }

  /**
   * Read card CID register. The CID contains card identification
   * information such as Manufacturer ID, Product name, Product serial
   * number and Manufacturing date.
   * @param[out] cid pointer to cid data store.
   * @return true for successful otherwise false.
   */
  bool read(cid_t* cid)
    __attribute__((always_inline))
  {
    return (read(SEND_CID, 0, cid, sizeof(cid_t)));
  }

  /**
   * Read card CSD register. The CSD contains that provides
   * information regarding access to the card's contents.
   * @param[out] csd pointer to csd data store.
   * @return true for successful otherwise false.
   */
  bool read(csd_t* csd)
    __attribute__((always_inline))
  {
    return (read(SEND_CSD, 0, csd, sizeof(csd_t)));
  }

  /**
   * Write given source buffer with BLOCK_MAX bytes to the given
   * block. Returns true if successful otherwise false.
   * @param[in] block address.
   * @param[in] src pointer to source buffer.
   * @return bool.
   */
  bool write(uint32_t block, const uint8_t* src);
};

#endif
