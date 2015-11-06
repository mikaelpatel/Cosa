/**
 * @file Si70XX.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2015, Mikael Patel
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

#include "Si70XX.hh"

static uint8_t crc_update(uint8_t crc, uint8_t data)
{
  crc ^= data;
  for (uint8_t i = CHARBITS; i != 0; i--) {
    uint8_t msb = (crc & 0x80);
    crc <<= 1;
    if (msb) crc ^= 0x31;
  }
  return (crc);
}

bool
Si70XX::issue(uint8_t cmd)
{
  int count;

  twi.acquire(this);
  count = twi.write(cmd);
  twi.release();
  return (count == sizeof(cmd));
}

bool
Si70XX::read(uint16_t& value, bool check)
{
  uint8_t buf[3];
  int size;
  int count;

  size = check ? sizeof(buf) : sizeof(buf) - 1;
  for (int retry = 0; retry < 20; retry++) {
    twi.acquire(this);
    count = twi.read(buf, size);
    twi.release();
    if (count != -1) break;
    delay(1);
  }
  if (count != size) return (false);
  if (check) {
    uint8_t crc;
    crc = crc_update(0, buf[0]);
    crc = crc_update(crc, buf[1]);
    if (crc != buf[2]) return (false);
  }
  value = ((buf[0] << 8) | buf[1]);
  return (true);
}

bool
Si70XX::read(uint8_t cmd, uint8_t& value)
{
  uint8_t reg;
  int count;

  twi.acquire(this);
  twi.write(cmd);
  count = twi.read(&reg, sizeof(reg));
  twi.release();
  if (count != sizeof(reg)) return (false);
  value = reg;
  return (true);
}

bool
Si70XX::read_electronic_serial_number(uint8_t snr[8])
{
  uint8_t sna[8];
  uint8_t snb[6];
  uint16_t xmd;
  uint8_t crc;
  int count;
  size_t j;

  // Read SNA and check crc
  twi.acquire(this);
  xmd = 0xFA0F;
  count = twi.write(xmd);
  if (count != sizeof(xmd)) goto err;
  count = twi.read(sna, sizeof(sna));
  if (count != sizeof(sna)) goto err;
  crc = 0;
  j = 0;
  for (size_t i = 0; i < sizeof(sna);) {
    crc = crc_update(crc, sna[i]);
    snr[j++] = sna[i++];
    if (sna[i++] != crc) goto err;
  }

  // Read SNB and check crc
  xmd = 0xFCC9;
  count = twi.write(xmd);
  if (count != sizeof(xmd)) goto err;
  count = twi.read(&snb, sizeof(snb));
  if (count != sizeof(snb)) goto err;
  crc = 0;
  for (size_t i = 0; i < sizeof(snb); ) {
    crc = crc_update(crc, snb[i]);
    snr[j++] = snb[i++];
    crc = crc_update(crc, snb[i]);
    snr[j++] = snb[i++];
    if (snb[i++] != crc) goto err;
  }
  twi.release();

  return (true);

 err:
  twi.release();
  return (false);
}

bool
Si70XX::read_firmware_revision(uint8_t &rev)
{
  uint16_t xmd = 0x84B8;
  int count;
  twi.acquire(this);
  count = twi.write(xmd);
  count = twi.read(&rev, sizeof(rev));
  twi.release();
  return (count == sizeof(rev));
}

