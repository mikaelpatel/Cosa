/**
 * @file Cosa/Driver/SD.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2012-2013, Mikael Patel
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

#include "Cosa/Driver/SD.hh"
#include "Cosa/RTC.hh"
#include <util/crc16.h>

static uint8_t
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

uint8_t 
SD::send(CMD command, uint32_t arg)
{
  // Build request with command, argument and add check-sum (CRC7)
  request_t request;
  request.command = (0x40 | command);
  request.arg = swap((int32_t) arg);
  request.crc = crc7(&request, sizeof(request) - 1);

  // Issue the command; wait while busy
  while (spi.transfer(0xff) != 0xff);
  spi.transfer(&request, sizeof(request));
  if (command == STOP_TRANSMISSION) spi.transfer(0xff);

  // Wait for the response
  uint8_t response;
  for (uint8_t i = 0; i < 10; i++) {
    response = spi.transfer(0xff);
    if ((response & 0x80) == 0) break;
  }
  return (m_response = response);
}

bool
SD::send(uint16_t ms, CMD command, uint32_t arg)
{
  uint16_t start = RTC::millis();
  do {
    uint8_t status = send(command, arg);
    if (status < 2) return (true);
  } while (((uint16_t) RTC::millis()) - start < ms);
  return (false);
}

uint8_t
SD::send(ACMD command, uint32_t arg)
{
  send(APP_CMD, arg);
  return (send((CMD) command, arg));
}

bool
SD::send(uint16_t ms, ACMD command, uint32_t arg)
{
  uint16_t start = RTC::millis();
  do {
    uint8_t status = send(command, arg);
    if (status < 2) return (true);
  } while (((uint16_t) RTC::millis()) - start < ms);
  return (false);
}

bool
SD::await(uint16_t ms, uint8_t token)
{
  uint16_t start = RTC::millis();
  do {
    uint8_t response = spi.transfer(0xff);
    if (response != 0xff) {
      m_response = response;
      return (token != 0 ? response == token : true);
    }
  } while ((ms == 0) || (((uint16_t) RTC::millis()) - start < ms));
  return (false);
}

uint32_t 
SD::receive()
{
  univ32_t res;
  res.as_uint8[3] = spi.transfer(0xff);
  res.as_uint8[2] = spi.transfer(0xff);
  res.as_uint8[1] = spi.transfer(0xff);
  res.as_uint8[0] = spi.transfer(0xff);
  return (res.as_uint32);
}

bool 
SD::read(CMD command, uint32_t arg, void* buf, size_t count)
{
  uint8_t* dst = (uint8_t*) buf;
  uint16_t crc = 0;
  bool res = false;

  // Issue read command and receive data into buffer
  spi.begin(this);
  if (send(command, arg)) goto error;
  if (!await(READ_TIMEOUT, DATA_START_BLOCK)) goto error;
  do {
    uint8_t data = spi.transfer(0xff); 
    *dst++ = data;
    crc = _crc_xmodem_update(crc, data);
  } while (--count);
  
  // Receive the check sum and check
  crc = _crc_xmodem_update(crc, spi.transfer(0xff));
  crc = _crc_xmodem_update(crc, spi.transfer(0xff));
  res = (crc == 0);

 error:
  spi.end();
  return (res);
}

bool
SD::begin(SPI::Clock rate)
{
  uint8_t res = false;
  uint32_t arg;
  R1 status;
  
  /* Start with unknown card type */
  m_type = TYPE_UNKNOWN;
  
  /* Card needs 74 cycles minimum to start up */
  spi.begin(this);
  for (uint8_t i = 0; i < 10; ++i) spi.transfer(0xff);

  /* Reset card */
  if (!send(INIT_TIMEOUT, GO_IDLE_STATE)) goto error;

  /* Enable CRC */
  status = send(CRC_ON_OFF, true);
  if (!status.in_idle_state) goto error;
  
  /* Check for version of SD card specification; 2.7-3.6V and check pattern */
  m_type = TYPE_SD1;
  arg = (0x100 | CHECK_PATTERN);
  status = send(SEND_IF_COND, arg);
  if (status.in_idle_state) {
    R7 r7 = receive();
    if (r7.check_pattern != CHECK_PATTERN) goto error;
    m_type = TYPE_SD2;
  } 

  /* Tell the device that the host supports SDHC */
  arg = (m_type == TYPE_SD1) ? 0 : 0X40000000;
  if (!send(INIT_TIMEOUT, SD_SEND_OP_COND, arg)) goto error;
  if (m_type == TYPE_SD2) {
    status = send(READ_OCR);
    if (!status.in_idle_state) goto error;
    uint32_t ocr = receive();
    if ((ocr & 0xC0000000L) == 0xC0000000L) m_type = TYPE_SDHC;
  }
  
  // Set the request clock rate
  set_clock(rate);
  res = true;
  
 error:
  spi.end();
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
  spi.begin(this);
  if (send(ERASE_WR_BLK_START, start)) goto error;
  if (send(ERASE_WR_BLK_END, end)) goto error;
  if (send(ERASE)) goto error;
  if (!await(ERASE_TIMEOUT)) goto error;
  res = true;
  
 error:
  spi.end();
  return (res);
}

bool 
SD::write(uint32_t block, const uint8_t* src) 
{
  uint16_t crc = 0;
  uint16_t count = BLOCK_MAX;
  uint8_t status;
  bool res = false;

  // Check for byte address adjustment
  if (m_type != TYPE_SDHC) block <<= 9;

  // Issue write block command, transfer block, calculate check sum
  spi.begin(this);
  if (send(WRITE_BLOCK, block)) goto error;
  spi.transfer(DATA_START_BLOCK);
  do {
    uint8_t data = *src++;
    spi.transfer(data);
    crc = _crc_xmodem_update(crc, data);
  } while (--count);

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
  return (res);
}

