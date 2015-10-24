/**
 * @file SD.cpp
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

#include "SD.hh"
#include "Cosa/RTT.hh"

// Configuration: Allow SPI transfer interleaving, table driven CRC.
#define USE_SPI_PREFETCH
#define USE_CRCTAB

static inline uint8_t crc7(const void* buf, size_t size)
  __attribute__((always_inline));

static inline uint8_t
crc7(const void* buf, size_t size)
{
  uint8_t* bp = (uint8_t*) buf;
  uint8_t crc = 0;
  while (size--) {
    uint8_t data = *bp++;
    data ^= crc << 1;
    if (data & 0x80) data ^= 9;
    crc = data ^ (crc & 0x78) ^ (crc << 4) ^ ((crc >> 3) & 0x0f);
  }
  crc = (crc << 1) ^ (crc << 4) ^ (crc & 0x70) ^ ((crc >> 3) & 0x0f);
  return (crc | 1);
}

#if defined(USE_CRCTAB)
static const uint16_t crctab[] __PROGMEM = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static inline uint16_t _crc_xmodem_update(uint16_t crc, uint8_t data)
  __attribute__((always_inline));

static inline uint16_t
_crc_xmodem_update(uint16_t crc, uint8_t data)
{
  return (pgm_read_word(&crctab[((crc >> 8) ^ data) & 0Xff]) ^ (crc << 8));
}

#else
#include <util/crc16.h>
#endif

uint8_t
SD::send(CMD command, uint32_t arg)
{
  // Build request with command, argument and add check-sum (CRC7)
  request_t request;
  request.command = (0x40 | command);
  request.arg = swap(arg);
  request.crc = crc7(&request, sizeof(request) - 1);

  // Issue the command; wait while busy
  while (spi.transfer(0xff) != 0xff)
    ;
  spi.transfer(&request, sizeof(request));
  if (command == STOP_TRANSMISSION) spi.transfer(0xff);

  // Wait for the response
  uint8_t response;
  for (uint8_t i = 0; i < RESPONSE_RETRY; i++) {
    response = spi.transfer(0xff);
    if ((response & 0x80) == 0) break;
  }
  return (m_response = response);
}

bool
SD::send(uint16_t ms, CMD command, uint32_t arg)
{
  uint16_t start = RTT::millis();
  do {
    uint8_t state = send(command, arg);
    if (state < IDENT_STATE) return (true);
  } while (((uint16_t) RTT::millis()) - start < ms);
  return (false);
}

uint8_t
SD::send(ACMD command, uint32_t arg)
{
  send(APP_CMD);
  return (send((CMD) command, arg));
}

bool
SD::send(uint16_t ms, ACMD command, uint32_t arg)
{
  uint16_t start = RTT::millis();
  do {
    send(APP_CMD);
    uint8_t state = send((CMD) command, arg);
    if (state < IDENT_STATE) return (true);
  } while (((uint16_t) RTT::millis()) - start < ms);
  return (false);
}

bool
SD::await(uint16_t ms, uint8_t token)
{
  uint16_t start = RTT::millis();
  do {
    uint8_t response = spi.transfer(0xff);
    if (response != 0xff) {
      m_response = response;
      return (token != 0 ? response == token : true);
    }
  } while ((ms == 0) || (((uint16_t) RTT::millis()) - start < ms));
  return (false);
}

uint32_t
SD::receive()
{
  univ32_t res;
#if defined(USE_SPI_PREFETCH)
  spi.transfer_start(0xff);
  res.as_uint8[3] = spi.transfer_next(0xff);
  res.as_uint8[2] = spi.transfer_next(0xff);
  res.as_uint8[1] = spi.transfer_next(0xff);
  res.as_uint8[0] = spi.transfer_await();
#else
  res.as_uint8[3] = spi.transfer(0xff);
  res.as_uint8[2] = spi.transfer(0xff);
  res.as_uint8[1] = spi.transfer(0xff);
  res.as_uint8[0] = spi.transfer(0xff);
#endif
  return (res.as_uint32);
}

bool
SD::read(CMD command, uint32_t arg, void* buf, size_t count)
{
  uint8_t* dst = (uint8_t*) buf;
  uint16_t crc = 0;
  bool res = false;
  uint8_t data;

  // Issue read command and receive data into buffer
  spi.acquire(this);
    spi.begin();
      if (send(command, arg)) goto error;
      if (!await(READ_TIMEOUT, DATA_START_BLOCK)) goto error;

#if defined(USE_SPI_PREFETCH)
      spi.transfer_start(0xff);
      while (--count) {
	data = spi.transfer_next(0xff);
	*dst++ = data;
	crc = _crc_xmodem_update(crc, data);
      }
      data = spi.transfer_await();
      *dst = data;
      crc = _crc_xmodem_update(crc, data);
#else
      do {
	uint8_t data = spi.transfer(0xff);
	*dst++ = data;
	crc = _crc_xmodem_update(crc, data);
      } while (--count);
#endif

      // Receive the check sum and check
      crc = _crc_xmodem_update(crc, spi.transfer(0xff));
      crc = _crc_xmodem_update(crc, spi.transfer(0xff));
      res = (crc == 0);

 error:
    spi.end();
  spi.release();
  return (res);
}

bool
SD::begin(SPI::Clock rate)
{
  bool res = false;
  uint32_t arg;
  R1 status;

  // Start with unknown card type
  m_type = TYPE_UNKNOWN;

  spi.acquire(this);
    spi.begin();
    // Card needs 74 cycles minimum to start up
      for (uint8_t i = 0; i < INIT_PULSES; i++) spi.transfer(0xff);

      // Reset card
      if (!send(INIT_TIMEOUT, GO_IDLE_STATE)) goto error;

      // Enable CRC
      status = send(CRC_ON_OFF, true);
      if (status.is_error()) goto error;

      // Check for version of SD card specification; 2.7-3.6V and check pattern
      m_type = TYPE_SD1;
      arg = (0x100 | CHECK_PATTERN);
      status = send(SEND_IF_COND, arg);
      if (status.in_idle_state) {
	R7 r7 = receive();
	if (r7.check_pattern != CHECK_PATTERN) goto error;
	m_type = TYPE_SD2;
      }

      // Tell the device that the host supports SDHC
      arg = (m_type == TYPE_SD1) ? 0L : 0X40000000L;
      for (uint8_t i = 0; i < INIT_RETRY; i++) {
	if (!send(INIT_TIMEOUT, SD_SEND_OP_COND, arg)) goto error;
	if (m_response == 0) break;
      }

      // Read OCR register and check type
      if (m_type == TYPE_SD2) {
	status = send(READ_OCR);
	if (status.is_error()) goto error;
	uint32_t ocr = receive();
	if ((ocr & 0xC0000000L) == 0xC0000000L) m_type = TYPE_SDHC;
      }

      // Set the request clock rate
      set_clock(rate);
      res = true;

 error:
    spi.end();
  spi.release();
  return (res);
}

bool
SD::end()
{
  return (true);
}

bool
SD::erase(uint32_t start, uint32_t end)
{
  bool res = false;

  // Check if block address should be mapped to byte address
  if (m_type != TYPE_SDHC) {
    start <<= 9;
    end <<= 9;
  }

  // Send commands for block erase
  spi.acquire(this);
    spi.begin();
      if (send(ERASE_WR_BLK_START, start)) goto error;
      if (send(ERASE_WR_BLK_END, end)) goto error;
      if (send(ERASE)) goto error;
      if (!await(ERASE_TIMEOUT)) goto error;
      res = true;
 error:
    spi.end();
  spi.release();
  return (res);
}

bool
SD::write(uint32_t block, const uint8_t* src)
{
  uint16_t crc = 0;
  uint16_t count = BLOCK_MAX;
  uint8_t status;
  uint8_t data;
  bool res = false;

  // Check for byte address adjustment
  if (m_type != TYPE_SDHC) block <<= 9;

  // Issue write block command, transfer block, calculate check sum
  spi.acquire(this);
    spi.begin();
      if (send(WRITE_BLOCK, block)) goto error;
      spi.transfer(DATA_START_BLOCK);

#if defined(USE_SPI_PREFETCH)
      data = *src++;
      spi.transfer_start(data);
      while (--count) {
	crc = _crc_xmodem_update(crc, data);
	data = *src++;
	spi.transfer_await();
	spi.transfer_start(data);
      }
      crc = _crc_xmodem_update(crc, data);
      spi.transfer_await();
#else
      do {
	data = *src++;
	spi.transfer(data);
	crc = _crc_xmodem_update(crc, data);
      } while (--count);
#endif

      // Transfer the check sum and receive data response token and check status
      spi.transfer(crc >> 8);
      spi.transfer(crc);
      status = spi.transfer(0xff);
      if ((status & DATA_RES_MASK) != DATA_RES_ACCEPTED) goto error;

      // Wait for the write operation to complete and check status
      if (!await(WRITE_TIMEOUT)) goto error;
      status = send(SEND_STATUS);
      if (status != 0) goto error;
      status = spi.transfer(0xff);
      res = (status == 0);

 error:
    spi.end();
  spi.release();
  return (res);
}

